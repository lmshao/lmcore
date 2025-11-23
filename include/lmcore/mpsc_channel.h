/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_MPSC_CHANNEL_H
#define LMSHAO_LMCORE_MPSC_CHANNEL_H

#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

#include "noncopyable.h"

namespace lmshao::lmcore::sync {

// Forward declarations
template <typename T>
class MpscSender;

template <typename T>
class MpscReceiver;

/**
 * @brief Create a bounded MPSC (Multi-Producer Single-Consumer) channel.
 * @tparam T Element type
 * @param capacity Channel capacity
 * @return A pair of (Sender, Receiver)
 *
 * Similar to Rust's std::sync::mpsc::sync_channel.
 * Multiple threads can send concurrently, but only one thread should receive.
 */
template <typename T>
std::pair<std::shared_ptr<MpscSender<T>>, std::unique_ptr<MpscReceiver<T>>> MpscChannel(size_t capacity);

/**
 * @brief Lock-free MPSC circular queue implementation.
 * @tparam T Element type
 *
 * Uses CAS (Compare-And-Swap) for multi-producer coordination.
 */
template <typename T>
class MpscCircularQueue : public NonCopyable {
public:
    explicit MpscCircularQueue(size_t capacity)
        : capacity_(capacity == 0 ? 1 : capacity), buffer_(capacity_), head_(0), tail_(0)
    {
    }

    size_t Capacity() const { return capacity_; }

    size_t Size() const
    {
        const uint64_t h = head_.load(std::memory_order_relaxed);
        const uint64_t t = tail_.load(std::memory_order_acquire);

        uint64_t diff = (t >= h) ? (t - h) : 0;
        uint64_t capacity_u64 = static_cast<uint64_t>(capacity_);
        if (diff > capacity_u64) {
            diff = capacity_u64;
        }

        return static_cast<size_t>(diff);
    }

    bool Empty() const
    {
        uint64_t h = head_.load(std::memory_order_relaxed);
        uint64_t t = tail_.load(std::memory_order_acquire);
        return t == h;
    }

    bool Full() const
    {
        uint64_t h = head_.load(std::memory_order_acquire);
        uint64_t t = tail_.load(std::memory_order_relaxed);
        return (t - h) >= static_cast<uint64_t>(capacity_);
    }

    bool TryPush(const T &value)
    {
        while (true) {
            uint64_t t = tail_.load(std::memory_order_relaxed);
            uint64_t h = head_.load(std::memory_order_acquire);

            if ((t - h) >= static_cast<uint64_t>(capacity_)) {
                return false;
            }

            if (tail_.compare_exchange_weak(t, t + 1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
                buffer_[static_cast<size_t>(t % static_cast<uint64_t>(capacity_))].emplace(value);
                return true;
            }
        }
    }

    bool TryPush(T &&value)
    {
        while (true) {
            uint64_t t = tail_.load(std::memory_order_relaxed);
            uint64_t h = head_.load(std::memory_order_acquire);

            if ((t - h) >= static_cast<uint64_t>(capacity_)) {
                return false;
            }

            if (tail_.compare_exchange_weak(t, t + 1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
                buffer_[static_cast<size_t>(t % static_cast<uint64_t>(capacity_))].emplace(std::move(value));
                return true;
            }
        }
    }

    std::optional<T> TryPop()
    {
        constexpr int max_spin_count = 100;
        int spin_count = 0;

        uint64_t h = head_.load(std::memory_order_relaxed);
        uint64_t t = tail_.load(std::memory_order_acquire);

        if (t == h) {
            return std::nullopt;
        }

        auto &slot = buffer_[static_cast<size_t>(h % static_cast<uint64_t>(capacity_))];
        while (!slot.has_value()) {
            if (++spin_count > max_spin_count) {
                std::this_thread::yield();
                spin_count = 0;
                h = head_.load(std::memory_order_relaxed);
                t = tail_.load(std::memory_order_acquire);
                if (t == h) {
                    return std::nullopt;
                }
                slot = buffer_[static_cast<size_t>(h % static_cast<uint64_t>(capacity_))];
            } else {
                std::this_thread::yield();
            }
        }

        std::optional<T> result(std::move(slot.value()));
        slot.reset();
        head_.store(h + 1, std::memory_order_release);
        return result;
    }

    void Clear()
    {
        uint64_t h = head_.load(std::memory_order_relaxed);
        uint64_t t = tail_.load(std::memory_order_relaxed);
        while (h != t) {
            buffer_[static_cast<size_t>(h % static_cast<uint64_t>(capacity_))].reset();
            ++h;
        }
        head_.store(t, std::memory_order_release);
    }

private:
    size_t capacity_;
    std::vector<std::optional<T>> buffer_;
    std::atomic<uint64_t> head_;
    std::atomic<uint64_t> tail_;
};

/**
 * @brief Sender half of a MPSC channel.
 * @tparam T Element type
 *
 * Can be cloned and used by multiple threads concurrently.
 */
template <typename T>
class MpscSender : public NonCopyable {
public:
    bool TrySend(T value) { return queue_->TryPush(std::move(value)); }

    bool Send(T value)
    {
        while (!closed_->load(std::memory_order_acquire)) {
            if (queue_->TryPush(std::move(value))) {
                return true;
            }
            std::this_thread::yield();
        }
        return false;
    }

    bool IsClosed() const { return closed_->load(std::memory_order_acquire); }

    void Close() { closed_->store(true, std::memory_order_release); }

private:
    template <typename U>
    friend std::pair<std::shared_ptr<MpscSender<U>>, std::unique_ptr<MpscReceiver<U>>> MpscChannel(size_t);

    explicit MpscSender(std::shared_ptr<MpscCircularQueue<T>> queue, std::shared_ptr<std::atomic<bool>> closed)
        : queue_(std::move(queue)), closed_(std::move(closed))
    {
    }

    std::shared_ptr<MpscCircularQueue<T>> queue_;
    std::shared_ptr<std::atomic<bool>> closed_;
};

/**
 * @brief Receiver half of a MPSC channel.
 * @tparam T Element type
 *
 * Only one thread should use this receiver.
 */
template <typename T>
class MpscReceiver : public NonCopyable {
public:
    std::optional<T> TryRecv() { return queue_->TryPop(); }

    std::optional<T> Recv()
    {
        while (!closed_->load(std::memory_order_acquire)) {
            auto val = queue_->TryPop();
            if (val.has_value()) {
                return val;
            }
            std::this_thread::yield();
        }
        return queue_->TryPop();
    }

    bool IsEmpty() const { return queue_->Empty(); }

    bool IsFull() const { return queue_->Full(); }

    bool IsClosed() const { return closed_->load(std::memory_order_acquire); }

private:
    template <typename U>
    friend std::pair<std::shared_ptr<MpscSender<U>>, std::unique_ptr<MpscReceiver<U>>> MpscChannel(size_t);

    explicit MpscReceiver(std::shared_ptr<MpscCircularQueue<T>> queue, std::shared_ptr<std::atomic<bool>> closed)
        : queue_(std::move(queue)), closed_(std::move(closed))
    {
    }

    std::shared_ptr<MpscCircularQueue<T>> queue_;
    std::shared_ptr<std::atomic<bool>> closed_;
};

template <typename T>
std::pair<std::shared_ptr<MpscSender<T>>, std::unique_ptr<MpscReceiver<T>>> MpscChannel(size_t capacity)
{
    auto queue = std::make_shared<MpscCircularQueue<T>>(capacity);
    auto closed = std::make_shared<std::atomic<bool>>(false);

    auto sender = std::shared_ptr<MpscSender<T>>(new MpscSender<T>(queue, closed));
    auto receiver = std::unique_ptr<MpscReceiver<T>>(new MpscReceiver<T>(queue, closed));

    return std::make_pair(std::move(sender), std::move(receiver));
}

} // namespace lmshao::lmcore::sync

#endif // LMSHAO_LMCORE_MPSC_CHANNEL_H
