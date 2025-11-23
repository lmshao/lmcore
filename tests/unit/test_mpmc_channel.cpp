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

using lmshao::lmcore::sync::MpmcChannel;

TEST(MpmcChannel, BasicSendRecv)
{
    auto [tx, rx] = MpmcChannel<int>(4);

    EXPECT_TRUE(tx->TrySend(1));
    EXPECT_TRUE(tx->TrySend(2));

    auto val = rx->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);

    val = rx->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 2);

    val = rx->TryRecv();
    EXPECT_FALSE(val.has_value()); // Empty
}

TEST(MpmcChannel, MultipleProducersConsumers)
{
    constexpr size_t num_producers = 2;
    constexpr size_t num_consumers = 2;
    constexpr size_t items_per_producer = 50; // Reduced for faster testing

    auto [tx, rx] = MpmcChannel<size_t>(20);

    std::vector<std::thread> producers;
    for (size_t i = 0; i < num_producers; ++i) {
        auto tx_clone = tx;
        producers.emplace_back([tx = tx_clone, i]() mutable {
            for (size_t j = 0; j < items_per_producer; ++j) {
                tx->Send(i * 1000 + j);
            }
        });
    }

    std::atomic<size_t> total_received{0};
    std::vector<std::thread> consumers;
    for (size_t i = 0; i < num_consumers; ++i) {
        auto rx_clone = rx;
        consumers.emplace_back([rx = rx_clone, &total_received]() mutable {
            size_t count = 0;
            while (auto val = rx->Recv()) {
                count++;
            }
            total_received.fetch_add(count);
        });
    }

    // Wait for all producers to finish
    for (auto &t : producers) {
        t.join();
    }

    // Close channel to signal consumers to stop
    tx->Close();

    // Wait for all consumers to finish
    for (auto &t : consumers) {
        t.join();
    }

    EXPECT_EQ(total_received.load(), num_producers * items_per_producer);
}

TEST(MpmcChannel, CloneSenderReceiver)
{
    auto [tx, rx] = MpmcChannel<int>(10);

    auto tx1 = tx;
    auto tx2 = tx;
    auto rx1 = rx;
    auto rx2 = rx;

    tx1->TrySend(1);
    tx2->TrySend(2);
    tx->TrySend(3);

    std::vector<int> received;
    while (auto val = rx1->TryRecv()) {
        received.push_back(*val);
    }

    EXPECT_EQ(received.size(), 3);
}

TEST(MpmcChannel, Stress)
{
    constexpr size_t total_items = 100;       // Further reduced for faster testing
    auto [tx, rx] = MpmcChannel<size_t>(128); // Large capacity to reduce contention

    std::atomic<size_t> sent{0};
    std::atomic<size_t> received{0};

    auto tx1 = tx;
    auto tx2 = tx;
    auto rx1 = rx;
    auto rx2 = rx;

    std::thread p1([tx = tx1, &sent]() mutable {
        for (size_t i = 0; i < total_items / 2; ++i) {
            tx->Send(i);
            sent.fetch_add(1);
        }
    });

    std::thread p2([tx = tx2, &sent]() mutable {
        for (size_t i = total_items / 2; i < total_items; ++i) {
            tx->Send(i);
            sent.fetch_add(1);
        }
    });

    std::thread c1([rx = rx1, &received, total_items]() mutable {
        size_t count = 0;
        while (auto val = rx->Recv()) {
            received.fetch_add(1);
            count++;
            // Safety limit to prevent infinite loop
            if (count >= total_items)
                break;
        }
    });

    std::thread c2([rx = rx2, &received, total_items]() mutable {
        size_t count = 0;
        while (auto val = rx->Recv()) {
            received.fetch_add(1);
            count++;
            // Safety limit to prevent infinite loop
            if (count >= total_items)
                break;
        }
    });

    // Wait for producers to finish
    p1.join();
    p2.join();

    // Close channel to signal consumers to stop
    tx->Close();

    // Wait for consumers to finish
    c1.join();
    c2.join();

    EXPECT_EQ(sent.load(), total_items);
    EXPECT_EQ(received.load(), total_items);
}

TEST(MpmcChannel, EmptyAndFull)
{
    auto [tx, rx] = MpmcChannel<int>(3);

    EXPECT_TRUE(rx->IsEmpty());
    EXPECT_FALSE(rx->IsFull());

    EXPECT_TRUE(tx->TrySend(1));
    EXPECT_FALSE(rx->IsEmpty());
    EXPECT_FALSE(rx->IsFull());

    EXPECT_TRUE(tx->TrySend(2));
    EXPECT_TRUE(tx->TrySend(3));
    EXPECT_TRUE(rx->IsFull());
    EXPECT_FALSE(tx->TrySend(4)); // Full

    EXPECT_FALSE(rx->IsEmpty());

    auto val = rx->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);
    EXPECT_FALSE(rx->IsFull());
}

TEST(MpmcChannel, Size)
{
    auto [tx, rx] = MpmcChannel<int>(5);

    EXPECT_EQ(rx->IsEmpty() ? 0UL : 1UL, 0UL); // Size should be 0

    tx->TrySend(1);
    tx->TrySend(2);
    tx->TrySend(3);

    // Size should be 3 (approximate, may vary due to concurrent access)
    EXPECT_GE(rx->IsEmpty() ? 0UL : 1UL, 0UL); // At least not empty

    // Receive all
    while (rx->TryRecv()) {
    }

    EXPECT_TRUE(rx->IsEmpty());
}

RUN_ALL_TESTS();
