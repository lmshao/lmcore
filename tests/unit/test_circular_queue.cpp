/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "../test_framework.h"
#include "lmcore/circular_queue.h"

using lmshao::lmcore::CircularQueue;

TEST(CircularQueue, BasicPushPop)
{
    CircularQueue<int> queue(4);

    EXPECT_TRUE(queue.Empty());
    EXPECT_FALSE(queue.Full());
    EXPECT_EQ(queue.Size(), 0);
    EXPECT_EQ(queue.Capacity(), 4);

    EXPECT_TRUE(queue.TryPush(1));
    EXPECT_TRUE(queue.TryPush(2));
    EXPECT_TRUE(queue.TryPush(3));

    EXPECT_FALSE(queue.Empty());
    EXPECT_FALSE(queue.Full());
    EXPECT_EQ(queue.Size(), 3);

    auto val = queue.TryPop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);

    val = queue.TryPop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 2);

    EXPECT_EQ(queue.Size(), 1);
}

TEST(CircularQueue, FullAndEmpty)
{
    CircularQueue<int> queue(3);

    EXPECT_TRUE(queue.TryPush(1));
    EXPECT_TRUE(queue.TryPush(2));
    EXPECT_TRUE(queue.TryPush(3));

    EXPECT_TRUE(queue.Full());
    EXPECT_FALSE(queue.TryPush(4));

    auto val = queue.TryPop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);

    EXPECT_FALSE(queue.Full());
    EXPECT_TRUE(queue.TryPush(4));

    queue.TryPop();
    queue.TryPop();
    queue.TryPop();

    EXPECT_TRUE(queue.Empty());
    val = queue.TryPop();
    EXPECT_FALSE(val.has_value());
}

TEST(CircularQueue, BlockingPush)
{
    CircularQueue<int> queue(2);

    queue.TryPush(1);
    queue.TryPush(2);

    std::atomic<bool> pushed{false};

    std::thread producer([&queue, &pushed]() {
        queue.Push(3);
        pushed.store(true);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(pushed.load());

    queue.TryPop();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(pushed.load());

    producer.join();

    auto val = queue.TryPop();
    EXPECT_EQ(*val, 2);
    val = queue.TryPop();
    EXPECT_EQ(*val, 3);
}

TEST(CircularQueue, BlockingPop)
{
    CircularQueue<int> queue(4);

    std::atomic<int> received{0};

    std::thread consumer([&queue, &received]() {
        int val = queue.Pop();
        received.store(val);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(received.load(), 0);

    queue.Push(42);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(received.load(), 42);

    consumer.join();
}

TEST(CircularQueue, ForcePush)
{
    CircularQueue<int> queue(3);

    EXPECT_TRUE(queue.TryPush(1));
    EXPECT_TRUE(queue.TryPush(2));
    EXPECT_TRUE(queue.TryPush(3));

    EXPECT_TRUE(queue.Full());

    bool overwrote = queue.ForcePush(4);
    EXPECT_TRUE(overwrote);

    EXPECT_TRUE(queue.Full());
    EXPECT_EQ(queue.Size(), 3);

    auto val = queue.TryPop();
    EXPECT_EQ(*val, 2);

    val = queue.TryPop();
    EXPECT_EQ(*val, 3);

    val = queue.TryPop();
    EXPECT_EQ(*val, 4);

    EXPECT_TRUE(queue.Empty());

    overwrote = queue.ForcePush(5);
    EXPECT_FALSE(overwrote);

    val = queue.TryPop();
    EXPECT_EQ(*val, 5);
}

TEST(CircularQueue, MoveSemantics)
{
    CircularQueue<std::string> queue(3);

    std::string str1 = "hello";
    queue.Push(std::move(str1));
    EXPECT_TRUE(str1.empty());

    std::string str2 = "world";
    EXPECT_TRUE(queue.TryPush(std::move(str2)));
    EXPECT_TRUE(str2.empty());

    auto val = queue.TryPop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, "hello");

    val = queue.TryPop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, "world");
}

TEST(CircularQueue, Clear)
{
    CircularQueue<int> queue(5);

    queue.TryPush(1);
    queue.TryPush(2);
    queue.TryPush(3);

    EXPECT_EQ(queue.Size(), 3);
    EXPECT_FALSE(queue.Empty());

    queue.Clear();

    EXPECT_EQ(queue.Size(), 0);
    EXPECT_TRUE(queue.Empty());
    EXPECT_FALSE(queue.Full());

    auto val = queue.TryPop();
    EXPECT_FALSE(val.has_value());
}

TEST(CircularQueue, ProducerConsumer)
{
    constexpr size_t total_items = 100;
    CircularQueue<size_t> queue(10);

    std::atomic<size_t> sum{0};

    std::thread producer([&queue]() {
        for (size_t i = 1; i <= total_items; ++i) {
            queue.Push(i);
        }
    });

    std::thread consumer([&queue, &sum]() {
        for (size_t i = 0; i < total_items; ++i) {
            size_t val = queue.Pop();
            sum.fetch_add(val, std::memory_order_relaxed);
        }
    });

    producer.join();
    consumer.join();

    const size_t expected = (total_items * (total_items + 1)) / 2;
    EXPECT_EQ(sum.load(), expected);
}

TEST(CircularQueue, MultipleProducersSingleConsumer)
{
    constexpr size_t num_producers = 4;
    constexpr size_t items_per_producer = 25;
    CircularQueue<int> queue(50);

    std::atomic<size_t> received{0};
    std::atomic<size_t> sum{0};

    std::vector<std::thread> producers;
    for (size_t i = 0; i < num_producers; ++i) {
        producers.emplace_back([&queue, i]() {
            for (size_t j = 0; j < items_per_producer; ++j) {
                queue.Push(static_cast<int>(i * 1000 + j));
            }
        });
    }

    std::thread consumer([&queue, &received, &sum, num_producers, items_per_producer]() {
        for (size_t i = 0; i < num_producers * items_per_producer; ++i) {
            int val = queue.Pop();
            sum.fetch_add(val, std::memory_order_relaxed);
            received.fetch_add(1);
        }
    });

    for (auto &t : producers) {
        t.join();
    }

    consumer.join();

    EXPECT_EQ(received.load(), num_producers * items_per_producer);
}

TEST(CircularQueue, SingleProducerMultipleConsumers)
{
    constexpr size_t num_consumers = 4;
    constexpr size_t total_items = 100;
    CircularQueue<int> queue(20);

    std::atomic<size_t> received{0};
    std::atomic<size_t> sum{0};

    std::thread producer([&queue]() {
        for (int i = 1; i <= static_cast<int>(total_items); ++i) {
            queue.Push(i);
        }
    });

    std::vector<std::thread> consumers;
    for (size_t i = 0; i < num_consumers; ++i) {
        consumers.emplace_back([&queue, &received, &sum, total_items]() {
            while (received.load() < total_items) {
                auto val = queue.TryPop();
                if (val.has_value()) {
                    sum.fetch_add(*val, std::memory_order_relaxed);
                    received.fetch_add(1);
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    producer.join();

    while (received.load() < total_items) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (auto &t : consumers) {
        t.join();
    }

    const size_t expected = (total_items * (total_items + 1)) / 2;
    EXPECT_EQ(sum.load(), expected);
    EXPECT_EQ(received.load(), total_items);
}

TEST(CircularQueue, ZeroCapacity)
{
    CircularQueue<int> queue(0);
    EXPECT_EQ(queue.Capacity(), 1);

    EXPECT_TRUE(queue.TryPush(1));
    EXPECT_FALSE(queue.TryPush(2));

    auto val = queue.TryPop();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);
}

TEST(CircularQueue, CircularBehavior)
{
    CircularQueue<int> queue(3);

    queue.TryPush(1);
    queue.TryPush(2);
    queue.TryPush(3);

    queue.TryPop();
    queue.TryPop();

    queue.TryPush(4);
    queue.TryPush(5);

    EXPECT_EQ(queue.Size(), 3);

    auto val = queue.TryPop();
    EXPECT_EQ(*val, 3);
    val = queue.TryPop();
    EXPECT_EQ(*val, 4);
    val = queue.TryPop();
    EXPECT_EQ(*val, 5);
}

RUN_ALL_TESTS();
