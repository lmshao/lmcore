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

using lmshao::lmcore::sync::MpscChannel;

TEST(MpscChannel, BasicSendRecv)
{
    auto [tx, rx] = MpscChannel<int>(4);

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

TEST(MpscChannel, MultipleProducers)
{
    constexpr size_t num_producers = 4;
    constexpr size_t items_per_producer = 100;

    auto [tx, rx] = MpscChannel<size_t>(50);

    std::vector<std::thread> producers;
    for (size_t i = 0; i < num_producers; ++i) {
        auto tx_clone = tx;
        producers.emplace_back([tx = tx_clone, i]() mutable {
            for (size_t j = 0; j < items_per_producer; ++j) {
                while (!tx->TrySend(i * 1000 + j)) {
                    std::this_thread::yield();
                }
            }
        });
    }

    std::atomic<size_t> received{0};
    std::thread consumer([rx = std::move(rx), &received]() mutable {
        while (auto val = rx->Recv()) {
            received.fetch_add(1);
        }
    });

    // Wait for all producers to finish
    for (auto &t : producers) {
        t.join();
    }

    // Close channel to signal consumer to stop
    tx->Close();

    // Wait for consumer to finish
    consumer.join();

    EXPECT_EQ(received.load(), num_producers * items_per_producer);
}

TEST(MpscChannel, CloseChannel)
{
    auto [tx, rx] = MpscChannel<int>(4);

    tx->TrySend(42);
    tx->Close();

    EXPECT_TRUE(tx->IsClosed());
    EXPECT_TRUE(rx->IsClosed());

    auto val = rx->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 42);

    EXPECT_FALSE(tx->Send(99));
}

TEST(MpscChannel, ConcurrentSendRecv)
{
    constexpr size_t total_items = 1000;
    auto [tx, rx] = MpscChannel<size_t>(16);

    auto tx1 = tx;
    auto tx2 = tx;

    std::thread producer1([tx = tx1]() mutable {
        for (size_t i = 0; i < total_items / 2; ++i) {
            tx->Send(i);
        }
    });

    std::thread producer2([tx = tx2]() mutable {
        for (size_t i = total_items / 2; i < total_items; ++i) {
            tx->Send(i);
        }
    });

    std::atomic<size_t> count{0};
    std::thread consumer([rx = std::move(rx), &count]() mutable {
        while (rx->Recv()) {
            count.fetch_add(1);
        }
    });

    producer1.join();
    producer2.join();
    tx->Close();
    consumer.join();

    EXPECT_EQ(count.load(), total_items);
}

TEST(MpscChannel, SizeAndFull)
{
    auto [tx, rx] = MpscChannel<int>(3);

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
