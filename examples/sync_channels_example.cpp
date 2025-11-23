/**
 * Comprehensive Sync Channels Example
 */

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "lmcore/sync.h"

using namespace lmshao::lmcore::sync;

// Example 1: SPSC - Single producer, single consumer
void spsc_example()
{
    std::cout << "=== SPSC Channel Example ===\n";

    auto [tx, rx] = SpscChannel<int>(10);

    std::thread producer([tx = std::move(tx)]() mutable {
        for (int i = 0; i < 20; ++i) {
            tx->Send(i);
            std::cout << "SPSC Producer: sent " << i << "\n";
        }
        tx->Close();
    });

    std::thread consumer([rx = std::move(rx)]() mutable {
        while (auto val = rx->Recv()) {
            std::cout << "SPSC Consumer: received " << *val << "\n";
        }
    });

    producer.join();
    consumer.join();
    std::cout << "\n";
}

// Example 2: MPSC - Multiple producers, single consumer
void mpsc_example()
{
    std::cout << "=== MPSC Channel Example ===\n";

    auto [tx, rx] = MpscChannel<std::string>(10);

    // Clone sender for multiple producers
    auto tx1 = tx;
    auto tx2 = tx;

    std::thread producer1([tx = tx1]() mutable {
        for (int i = 0; i < 5; ++i) {
            tx->Send("P1-" + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::thread producer2([tx = tx2]() mutable {
        for (int i = 0; i < 5; ++i) {
            tx->Send("P2-" + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
    });

    std::thread consumer([rx = std::move(rx), tx]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        tx->Close(); // Close original sender
        while (auto val = rx->Recv()) {
            std::cout << "MPSC Consumer: received " << *val << "\n";
        }
    });

    producer1.join();
    producer2.join();
    consumer.join();
    std::cout << "\n";
}

// Example 3: SPMC - Single producer, multiple consumers
void spmc_example()
{
    std::cout << "=== SPMC Channel Example ===\n";

    auto [tx, rx] = SpmcChannel<int>(10);

    // Clone receiver for multiple consumers
    auto rx1 = rx;
    auto rx2 = rx;

    std::thread producer([tx = std::move(tx)]() mutable {
        for (int i = 0; i < 20; ++i) {
            tx->Send(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        tx->Close();
    });

    std::thread consumer1([rx = rx1]() mutable {
        while (auto val = rx->Recv()) {
            std::cout << "SPMC Consumer1: received " << *val << "\n";
        }
    });

    std::thread consumer2([rx = rx2]() mutable {
        while (auto val = rx->Recv()) {
            std::cout << "SPMC Consumer2: received " << *val << "\n";
        }
    });

    producer.join();
    consumer1.join();
    consumer2.join();
    std::cout << "\n";
}

// Example 4: MPMC - Multiple producers, multiple consumers
void mpmc_example()
{
    std::cout << "=== MPMC Channel Example ===\n";

    auto [tx, rx] = MpmcChannel<std::string>(10);

    // Clone for multiple producers
    auto tx1 = tx;
    auto tx2 = tx;

    // Clone for multiple consumers
    auto rx1 = rx;
    auto rx2 = rx;

    std::thread producer1([tx = tx1]() mutable {
        for (int i = 0; i < 5; ++i) {
            tx->Send("P1-" + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });

    std::thread producer2([tx = tx2]() mutable {
        for (int i = 0; i < 5; ++i) {
            tx->Send("P2-" + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    });

    std::thread consumer1([rx = rx1]() mutable {
        int count = 0;
        while (auto val = rx->Recv()) {
            std::cout << "MPMC Consumer1: received " << *val << "\n";
            if (++count >= 5)
                break;
        }
    });

    std::thread consumer2([rx = rx2]() mutable {
        int count = 0;
        while (auto val = rx->Recv()) {
            std::cout << "MPMC Consumer2: received " << *val << "\n";
            if (++count >= 5)
                break;
        }
    });

    producer1.join();
    producer2.join();
    tx->Close();
    consumer1.join();
    consumer2.join();
    std::cout << "\n";
}

// Example 5: Real-world scenario - Video streaming pipeline
void video_pipeline_example()
{
    std::cout << "=== Video Pipeline Example ===\n";

    struct Frame {
        int id;
        std::vector<uint8_t> data;
    };

    // Network -> Decoder (SPSC for low latency)
    auto [net_tx, dec_rx] = SpscChannel<Frame>(5);

    // Decoder -> Renderer (SPSC)
    auto [dec_tx, rend_rx] = SpscChannel<Frame>(5);

    // Network thread
    std::thread network([tx = std::move(net_tx)]() mutable {
        for (int i = 0; i < 10; ++i) {
            Frame frame{i, std::vector<uint8_t>(1024)};
            if (tx->TrySend(std::move(frame))) {
                std::cout << "Network: received frame " << i << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33)); // 30fps
        }
        tx->Close();
    });

    // Decoder thread
    std::thread decoder([rx = std::move(dec_rx), tx = std::move(dec_tx)]() mutable {
        while (auto frame = rx->Recv()) {
            std::cout << "Decoder: processing frame " << frame->id << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Decode time
            tx->Send(std::move(*frame));
        }
        tx->Close();
    });

    // Renderer thread
    std::thread renderer([rx = std::move(rend_rx)]() mutable {
        while (auto frame = rx->Recv()) {
            std::cout << "Renderer: displaying frame " << frame->id << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60fps
        }
    });

    network.join();
    decoder.join();
    renderer.join();
}

int main()
{
    spsc_example();
    mpsc_example();
    spmc_example();
    mpmc_example();
    video_pipeline_example();

    return 0;
}
