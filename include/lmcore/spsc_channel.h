/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_SPSC_CHANNEL_H
#define LMSHAO_LMCORE_SPSC_CHANNEL_H

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

#include "noncopyable.h"

namespace lmshao::lmcore::sync {

template <typename T>
class SpscCircularQueue : public NonCopyable {
public:
    explicit SpscCircularQueue(size_t capacity)
        : capacity_(capacity == 0 ? 1 : capacity), buffer_(capacity_), head_(0), tail_(0)
    {
    }

    size_t Capacity() const { return capacity_; }

    size_t Size() const
    {
        const uint64_t h = head_.load(std::memory_order_relaxed);
        const uint64_t t = tail_.load(std::memory_order_relaxed);

        uint64_t diff = (t >= h) ? (t - h) : 0;
        uint64_t capacity_u64 = static_cast<uint64_t>(capacity_);
        if (diff > capacity_u64) {
            diff = capacity_u64;
        }

        return static_cast<size_t>(diff);
    }

    bool Empty() const
    {
        const uint64_t h = head_.load(std::memory_order_relaxed);
        const uint64_t t = tail_.load(std::memory_order_acquire);
        return t == h;
    }

    bool Full() const
    {
        const uint64_t h = head_.load(std::memory_order_acquire);
        const uint64_t t = tail_.load(std::memory_order_relaxed);
        return (t - h) >= static_cast<uint64_t>(capacity_);
    }

    bool TryPush(const T &value)
    {
        uint64_t t = tail_.load(std::memory_order_relaxed);
        uint64_t h = head_.load(std::memory_order_acquire);
        if ((t - h) >= static_cast<uint64_t>(capacity_)) {
            return false;
        }
        buffer_[static_cast<size_t>(t % static_cast<uint64_t>(capacity_))].emplace(value);
        tail_.store(t + 1, std::memory_order_release);
        return true;
    }

    bool TryPush(T &&value)
    {
        uint64_t t = tail_.load(std::memory_order_relaxed);
        uint64_t h = head_.load(std::memory_order_acquire);
        if ((t - h) >= static_cast<uint64_t>(capacity_)) {
            return false;
        }
        buffer_[static_cast<size_t>(t % static_cast<uint64_t>(capacity_))].emplace(std::move(value));
        tail_.store(t + 1, std::memory_order_release);
        return true;
    }

    bool TryPop(T &out)
    {
        uint64_t h = head_.load(std::memory_order_relaxed);
        uint64_t t = tail_.load(std::memory_order_acquire);
        if (t == h) {
            return false;
        }
        auto &slot = buffer_[static_cast<size_t>(h % static_cast<uint64_t>(capacity_))];
        out = std::move(slot.value());
        slot.reset();
        head_.store(h + 1, std::memory_order_release);
        return true;
    }

    std::optional<T> TryPop()
    {
        uint64_t h = head_.load(std::memory_order_relaxed);
        uint64_t t = tail_.load(std::memory_order_acquire);
        if (t == h) {
            return std::nullopt;
        }
        auto &slot = buffer_[static_cast<size_t>(h % static_cast<uint64_t>(capacity_))];
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

// Forward declarations
template <typename T>
class SpscSender;

template <typename T>
class SpscReceiver;

/**
 * @brief Create a bounded SPSC (Single-Producer Single-Consumer) channel.
 * @tparam T Element type
 * @param capacity Channel capacity
 * @return A pair of (Sender, Receiver)
 *
 * Similar to Rust's std::sync::mpsc::sync_channel for SPSC scenario.
 * The sender can only be used by one producer thread, and the receiver by one consumer thread.
 */
template <typename T>
std::pair<std::unique_ptr<SpscSender<T>>, std::unique_ptr<SpscReceiver<T>>> SpscChannel(size_t capacity);

/**
 * @brief Sender half of a SPSC channel.
 * @tparam T Element type
 *
 * The sender can be cloned (shares the underlying queue), but users must ensure
 * only one thread calls send/try_send at any time (SPSC constraint).
 */
template <typename T>
class SpscSender : public NonCopyable {
public:
    /**
     * @brief Try to send a value (non-blocking).
     * @param value Value to send
     * @return true if sent successfully, false if channel is full
     */
    bool TrySend(T value) { return queue_->TryPush(std::move(value)); }

    /**
     * @brief Send a value (blocking with spinning).
     * @param value Value to send
     * @return true if sent, false if channel is closed
     *
     * Blocks by spinning until space is available or channel is closed.
     */
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

    /**
     * @brief Check if channel is closed.
     */
    bool IsClosed() const { return closed_->load(std::memory_order_acquire); }

    /**
     * @brief Close the channel (prevents further sends).
     */
    void Close() { closed_->store(true, std::memory_order_release); }

private:
    template <typename U>
    friend std::pair<std::unique_ptr<SpscSender<U>>, std::unique_ptr<SpscReceiver<U>>> SpscChannel(size_t);

    explicit SpscSender(std::shared_ptr<SpscCircularQueue<T>> queue, std::shared_ptr<std::atomic<bool>> closed)
        : queue_(std::move(queue)), closed_(std::move(closed))
    {
    }

    std::shared_ptr<SpscCircularQueue<T>> queue_;
    std::shared_ptr<std::atomic<bool>> closed_;
};

/**
 * @brief Receiver half of a SPSC channel.
 * @tparam T Element type
 *
 * Only one consumer thread should use this receiver.
 */
template <typename T>
class SpscReceiver : public NonCopyable {
public:
    /**
     * @brief Try to receive a value (non-blocking).
     * @return std::optional<T> containing value if available, std::nullopt if empty
     */
    std::optional<T> TryRecv() { return queue_->TryPop(); }

    /**
     * @brief Receive a value (blocking with spinning).
     * @return std::optional<T> containing value, or std::nullopt if channel is closed and empty
     *
     * Blocks by spinning until a value is available or channel is closed.
     */
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

    /**
     * @brief Check if channel is empty.
     */
    bool IsEmpty() const { return queue_->Empty(); }

    /**
     * @brief Check if channel is full.
     */
    bool IsFull() const { return queue_->Full(); }

    /**
     * @brief Check if channel is closed.
     */
    bool IsClosed() const { return closed_->load(std::memory_order_acquire); }

private:
    template <typename U>
    friend std::pair<std::unique_ptr<SpscSender<U>>, std::unique_ptr<SpscReceiver<U>>> SpscChannel(size_t);

    explicit SpscReceiver(std::shared_ptr<SpscCircularQueue<T>> queue, std::shared_ptr<std::atomic<bool>> closed)
        : queue_(std::move(queue)), closed_(std::move(closed))
    {
    }

    std::shared_ptr<SpscCircularQueue<T>> queue_;
    std::shared_ptr<std::atomic<bool>> closed_;
};

/**
 * @brief Create a bounded SPSC channel.
 * @tparam T Element type
 * @param capacity Maximum number of elements the channel can hold
 * @return A pair of (Sender, Receiver)
 */
template <typename T>
std::pair<std::unique_ptr<SpscSender<T>>, std::unique_ptr<SpscReceiver<T>>> SpscChannel(size_t capacity)
{
    auto queue = std::make_shared<SpscCircularQueue<T>>(capacity);
    auto closed = std::make_shared<std::atomic<bool>>(false);

    auto sender = std::unique_ptr<SpscSender<T>>(new SpscSender<T>(queue, closed));
    auto receiver = std::unique_ptr<SpscReceiver<T>>(new SpscReceiver<T>(queue, closed));

    return std::make_pair(std::move(sender), std::move(receiver));
}

} // namespace lmshao::lmcore::sync

#endif // LMSHAO_LMCORE_SPSC_CHANNEL_H
