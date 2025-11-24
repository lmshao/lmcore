/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_CIRCULAR_QUEUE_H
#define LMSHAO_LMCORE_CIRCULAR_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <optional>
#include <vector>

namespace lmshao::lmcore {

template <typename T>
class CircularQueue {
public:
    explicit CircularQueue(size_t capacity)
        : capacity_(capacity == 0 ? 1 : capacity), buffer_(capacity_), head_(0), tail_(0), size_(0)
    {
    }

    void Push(const T &value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this] { return size_ < capacity_; });

        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % capacity_;
        ++size_;

        not_empty_.notify_one();
    }

    void Push(T &&value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this] { return size_ < capacity_; });

        buffer_[tail_] = std::move(value);
        tail_ = (tail_ + 1) % capacity_;
        ++size_;

        not_empty_.notify_one();
    }

    bool TryPush(const T &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ >= capacity_) {
            return false;
        }

        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % capacity_;
        ++size_;

        not_empty_.notify_one();
        return true;
    }

    bool TryPush(T &&value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ >= capacity_) {
            return false;
        }

        buffer_[tail_] = std::move(value);
        tail_ = (tail_ + 1) % capacity_;
        ++size_;

        not_empty_.notify_one();
        return true;
    }

    bool ForcePush(const T &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        bool overwrote = (size_ >= capacity_);

        if (overwrote) {
            head_ = (head_ + 1) % capacity_;
        } else {
            ++size_;
        }

        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % capacity_;

        not_empty_.notify_one();
        return overwrote;
    }

    bool ForcePush(T &&value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        bool overwrote = (size_ >= capacity_);

        if (overwrote) {
            head_ = (head_ + 1) % capacity_;
        } else {
            ++size_;
        }

        buffer_[tail_] = std::move(value);
        tail_ = (tail_ + 1) % capacity_;

        not_empty_.notify_one();
        return overwrote;
    }

    T Pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this] { return size_ > 0; });

        T result = std::move(buffer_[head_]);
        head_ = (head_ + 1) % capacity_;
        --size_;

        not_full_.notify_one();
        return result;
    }

    std::optional<T> TryPop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ == 0) {
            return std::nullopt;
        }

        std::optional<T> result(std::move(buffer_[head_]));
        head_ = (head_ + 1) % capacity_;
        --size_;

        not_full_.notify_one();
        return result;
    }

    size_t Capacity() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return capacity_;
    }

    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_;
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_ == 0;
    }

    bool Full() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_ == capacity_;
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        head_ = 0;
        tail_ = 0;
        size_ = 0;
        not_full_.notify_all();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;

    size_t capacity_;
    std::vector<T> buffer_;
    size_t head_;
    size_t tail_;
    size_t size_;
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_CIRCULAR_QUEUE_H
