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
#include "lmcore/sync.h"

using lmshao::lmcore::sync::SpmcChannel;

TEST(SpmcChannel, BasicSendRecv)
{
    auto [tx, rx] = SpmcChannel<int>(4);

    EXPECT_TRUE(tx->TrySend(1));
    EXPECT_TRUE(tx->TrySend(2));

    auto val = rx->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);

    val = rx->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 2);

    val = rx->TryRecv();
    EXPECT_FALSE(val.has_value());
}

TEST(SpmcChannel, MultipleConsumers)
{
    constexpr size_t num_consumers = 4;
    constexpr size_t total_items = 100;

    auto [tx, rx] = SpmcChannel<size_t>(128);

    std::atomic<size_t> total_received{0};
    std::vector<std::thread> consumers;
    for (size_t i = 0; i < num_consumers; ++i) {
        auto rx_clone = rx;
        consumers.emplace_back([rx = rx_clone, &total_received, total_items]() mutable {
            size_t count = 0;
            while (auto val = rx->Recv()) {
                count++;
                total_received.fetch_add(1);
                if (count >= total_items)
                    break;
            }
        });
    }

    // Producer sends items
    std::thread producer([tx = std::move(tx)]() mutable {
        for (size_t i = 0; i < total_items; ++i) {
            tx->Send(i);
        }
        tx->Close();
    });

    producer.join();

    // Wait for consumers
    for (auto &t : consumers) {
        t.join();
    }

    EXPECT_EQ(total_received.load(), total_items);
}

TEST(SpmcChannel, CloseChannel)
{
    auto [tx, rx] = SpmcChannel<int>(4);

    tx->TrySend(42);
    tx->Close();

    EXPECT_TRUE(tx->IsClosed());
    EXPECT_TRUE(rx->IsClosed());

    auto val = rx->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 42);

    EXPECT_FALSE(tx->Send(99));
}

TEST(SpmcChannel, ConcurrentSendRecv)
{
    constexpr size_t total_items = 200;       // Reduced for faster testing
    auto [tx, rx] = SpmcChannel<size_t>(128); // Increased capacity to reduce contention

    auto rx1 = rx;
    auto rx2 = rx;

    std::thread producer([tx = std::move(tx)]() mutable {
        for (size_t i = 0; i < total_items; ++i) {
            tx->Send(i);
        }
        tx->Close();
    });

    std::atomic<size_t> count1{0};
    std::atomic<size_t> count2{0};

    std::thread consumer1([rx = rx1, &count1, total_items]() mutable {
        size_t local_count = 0;
        while (auto val = rx->Recv()) {
            count1.fetch_add(1);
            local_count++;
            // Safety limit to prevent infinite loop
            if (local_count >= total_items)
                break;
        }
    });

    std::thread consumer2([rx = rx2, &count2, total_items]() mutable {
        size_t local_count = 0;
        while (auto val = rx->Recv()) {
            count2.fetch_add(1);
            local_count++;
            // Safety limit to prevent infinite loop
            if (local_count >= total_items)
                break;
        }
    });

    producer.join();
    consumer1.join();
    consumer2.join();

    EXPECT_EQ(count1.load() + count2.load(), total_items);
}

TEST(SpmcChannel, SizeAndFull)
{
    auto [tx, rx] = SpmcChannel<int>(3);

    EXPECT_TRUE(rx->IsEmpty());
    EXPECT_FALSE(rx->IsFull());

    EXPECT_TRUE(tx->TrySend(1));
    EXPECT_FALSE(rx->IsEmpty());
    EXPECT_FALSE(rx->IsFull());

    EXPECT_TRUE(tx->TrySend(2));
    EXPECT_TRUE(tx->TrySend(3));
    EXPECT_TRUE(rx->IsFull());
    EXPECT_FALSE(tx->TrySend(4)); // Full

    auto val = rx->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);
    EXPECT_FALSE(rx->IsFull());
}

RUN_ALL_TESTS();
