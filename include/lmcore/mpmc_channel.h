/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_MPMC_CHANNEL_H
#define LMSHAO_LMCORE_MPMC_CHANNEL_H

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
class MpmcSender;

template <typename T>
class MpmcReceiver;

/**
 * @brief Create a bounded MPMC (Multi-Producer Multi-Consumer) channel.
 * @tparam T Element type
 * @param capacity Channel capacity
 * @return A pair of (Sender, Receiver)
 *
 * Similar to crossbeam-channel in Rust.
 * Multiple threads can send and receive concurrently.
 */
template <typename T>
std::pair<std::shared_ptr<MpmcSender<T>>, std::shared_ptr<MpmcReceiver<T>>> MpmcChannel(size_t capacity);

/**
 * @brief Lock-free MPMC circular queue implementation.
 * @tparam T Element type
 *
 * Uses CAS on both ends for coordination.
 */
template <typename T>
class MpmcCircularQueue : public NonCopyable {
public:
    explicit MpmcCircularQueue(size_t capacity)
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

        while (true) {
            uint64_t h = head_.load(std::memory_order_relaxed);
            uint64_t t = tail_.load(std::memory_order_acquire);

            if (t == h) {
                return std::nullopt;
            }

            auto &slot = buffer_[static_cast<size_t>(h % static_cast<uint64_t>(capacity_))];
            if (!slot.has_value()) {
                if (++spin_count > max_spin_count) {
                    std::this_thread::yield();
                    spin_count = 0;
                    h = head_.load(std::memory_order_relaxed);
                    t = tail_.load(std::memory_order_acquire);
                    if (t == h) {
                        return std::nullopt;
                    }
                    continue;
                }
                std::this_thread::yield();
                continue;
            }

            spin_count = 0;

            if (head_.compare_exchange_weak(h, h + 1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
                std::optional<T> result(std::move(slot.value()));
                slot.reset();
                return result;
            }
            spin_count = 0;
        }
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
 * @brief Sender half of a MPMC channel.
 * @tparam T Element type
 *
 * Can be cloned and used by multiple threads concurrently.
 */
template <typename T>
class MpmcSender : public NonCopyable {
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
    friend std::pair<std::shared_ptr<MpmcSender<U>>, std::shared_ptr<MpmcReceiver<U>>> MpmcChannel(size_t);

    explicit MpmcSender(std::shared_ptr<MpmcCircularQueue<T>> queue, std::shared_ptr<std::atomic<bool>> closed)
        : queue_(std::move(queue)), closed_(std::move(closed))
    {
    }

    std::shared_ptr<MpmcCircularQueue<T>> queue_;
    std::shared_ptr<std::atomic<bool>> closed_;
};

/**
 * @brief Receiver half of a MPMC channel.
 * @tparam T Element type
 *
 * Can be cloned and used by multiple threads concurrently.
 */
template <typename T>
class MpmcReceiver : public NonCopyable {
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
    friend std::pair<std::shared_ptr<MpmcSender<U>>, std::shared_ptr<MpmcReceiver<U>>> MpmcChannel(size_t);

    explicit MpmcReceiver(std::shared_ptr<MpmcCircularQueue<T>> queue, std::shared_ptr<std::atomic<bool>> closed)
        : queue_(std::move(queue)), closed_(std::move(closed))
    {
    }

    std::shared_ptr<MpmcCircularQueue<T>> queue_;
    std::shared_ptr<std::atomic<bool>> closed_;
};

template <typename T>
std::pair<std::shared_ptr<MpmcSender<T>>, std::shared_ptr<MpmcReceiver<T>>> MpmcChannel(size_t capacity)
{
    auto queue = std::make_shared<MpmcCircularQueue<T>>(capacity);
    auto closed = std::make_shared<std::atomic<bool>>(false);

    auto sender = std::shared_ptr<MpmcSender<T>>(new MpmcSender<T>(queue, closed));
    auto receiver = std::shared_ptr<MpmcReceiver<T>>(new MpmcReceiver<T>(queue, closed));

    return std::make_pair(std::move(sender), std::move(receiver));
}

} // namespace lmshao::lmcore::sync

#endif // LMSHAO_LMCORE_MPMC_CHANNEL_H
