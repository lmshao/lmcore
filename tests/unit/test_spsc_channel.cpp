/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <atomic>
#include <thread>
#include <vector>

#include "../test_framework.h"
#include "lmcore/sync.h"

using lmshao::lmcore::sync::SpscChannel;

TEST(SpscChannel, BasicSendRecv)
{
    auto [sender, receiver] = SpscChannel<int>(4);

    EXPECT_TRUE(sender->TrySend(1));
    EXPECT_TRUE(sender->TrySend(2));
    EXPECT_TRUE(sender->TrySend(3));

    auto val = receiver->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);

    val = receiver->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 2);

    EXPECT_TRUE(sender->TrySend(4));
    EXPECT_TRUE(sender->TrySend(5));
    EXPECT_TRUE(sender->TrySend(6)); // Should succeed (capacity=4, 3 items)

    val = receiver->TryRecv();
    EXPECT_EQ(*val, 3);
    val = receiver->TryRecv();
    EXPECT_EQ(*val, 4);
    val = receiver->TryRecv();
    EXPECT_EQ(*val, 5);
    val = receiver->TryRecv();
    EXPECT_EQ(*val, 6);

    // Queue is now empty, fill it up to test full condition
    EXPECT_TRUE(sender->TrySend(7));
    EXPECT_TRUE(sender->TrySend(8));
    EXPECT_TRUE(sender->TrySend(9));
    EXPECT_TRUE(sender->TrySend(10));
    EXPECT_FALSE(sender->TrySend(11)); // Full (capacity=4, 4 items)

    // Queue is full, should be able to receive all 4 items
    val = receiver->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 7);
    val = receiver->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 8);
    val = receiver->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 9);
    val = receiver->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 10);

    // Now queue should be empty
    val = receiver->TryRecv();
    EXPECT_FALSE(val.has_value()); // Empty
}

TEST(SpscChannel, CloseChannel)
{
    auto [sender, receiver] = SpscChannel<int>(4);

    sender->TrySend(42);
    sender->Close();

    EXPECT_TRUE(sender->IsClosed());
    EXPECT_TRUE(receiver->IsClosed());

    // Can still receive buffered data
    auto val = receiver->TryRecv();
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 42);

    // Empty after close
    val = receiver->TryRecv();
    EXPECT_FALSE(val.has_value());

    // Send fails after close
    EXPECT_FALSE(sender->Send(99));
}

TEST(SpscChannel, BlockingSend)
{
    auto [sender, receiver] = SpscChannel<int>(2);

    sender->TrySend(1);
    sender->TrySend(2);

    std::atomic<bool> sent{false};

    std::thread producer([sender = std::move(sender), &sent]() mutable {
        sender->Send(3); // Blocks until space available
        sent.store(true);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(sent.load()); // Still blocked

    receiver->TryRecv(); // Free one slot

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(sent.load()); // Unblocked

    producer.join();

    auto val = receiver->TryRecv();
    EXPECT_EQ(*val, 2);
    val = receiver->TryRecv();
    EXPECT_EQ(*val, 3);
}

TEST(SpscChannel, BlockingRecv)
{
    auto [sender, receiver] = SpscChannel<int>(4);

    std::atomic<int> received{0};

    std::thread consumer([receiver = std::move(receiver), &received]() mutable {
        auto val = receiver->Recv(); // Blocks until data available
        if (val) {
            received.store(*val);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(received.load(), 0); // Still blocked

    sender->Send(42);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(received.load(), 42); // Received

    consumer.join();
}

TEST(SpscChannel, TrySendWhenFull)
{
    auto [sender, receiver] = SpscChannel<int>(2);

    EXPECT_TRUE(sender->TrySend(1));
    EXPECT_TRUE(sender->TrySend(2));
    EXPECT_FALSE(sender->TrySend(3)); // Full

    auto val = receiver->TryRecv();
    EXPECT_EQ(*val, 1);

    EXPECT_TRUE(sender->TrySend(3)); // Now has space

    val = receiver->TryRecv();
    EXPECT_EQ(*val, 2);
    val = receiver->TryRecv();
    EXPECT_EQ(*val, 3);
}

TEST(SpscChannel, ProducerConsumer)
{
    constexpr size_t total_items = 1000;
    constexpr size_t buffer_size = 16;

    auto [sender, receiver] = SpscChannel<size_t>(buffer_size);

    std::atomic<size_t> sum{0};

    // Producer
    std::thread producer([sender = std::move(sender)]() mutable {
        for (size_t i = 1; i <= total_items; ++i) {
            while (!sender->TrySend(i)) {
                std::this_thread::yield();
            }
        }
        sender->Close();
    });

    // Consumer
    std::thread consumer([receiver = std::move(receiver), &sum]() mutable {
        while (auto val = receiver->Recv()) {
            sum.fetch_add(*val, std::memory_order_relaxed);
        }
    });

    producer.join();
    consumer.join();

    const size_t expected = (total_items * (total_items + 1)) / 2;
    EXPECT_EQ(sum.load(), expected);
}

RUN_ALL_TESTS();
