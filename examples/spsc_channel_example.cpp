/**
 * SPSC Channel Example
 */

#include <chrono>
#include <iostream>
#include <thread>

#include "lmcore/sync.h"

using namespace lmshao::lmcore::sync;

// Example 1: Basic usage
void basic_example()
{
    std::cout << "=== Basic SPSC Channel Example ===\n";

    auto [sender, receiver] = SpscChannel<int>(10);

    // Producer thread
    std::thread producer([sender = std::move(sender)]() mutable {
        for (int i = 0; i < 20; ++i) {
            if (sender->TrySend(i)) {
                std::cout << "Sent: " << i << "\n";
            } else {
                std::cout << "Queue full, blocking send: " << i << "\n";
                sender->Send(i); // Blocking send
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        sender->Close();
    });

    // Consumer thread
    std::thread consumer([receiver = std::move(receiver)]() mutable {
        while (true) {
            if (auto val = receiver->TryRecv()) {
                std::cout << "Received: " << *val << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Slow consumer
            } else if (receiver->IsClosed()) {
                break;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();
}

// Example 2: Fast producer, slow consumer (demonstrates backpressure)
struct DataPacket {
    int sequence;
    uint64_t timestamp;
};

void backpressure_example()
{
    std::cout << "\n=== Backpressure Example ===\n";

    auto [sender, receiver] = SpscChannel<DataPacket>(5); // Small buffer

    std::atomic<int> dropped{0};

    // Fast producer
    std::thread producer([sender = std::move(sender), &dropped]() mutable {
        for (int i = 0; i < 50; ++i) {
            DataPacket packet{i, static_cast<uint64_t>(i * 33)};
            if (!sender->TrySend(packet)) {
                // Queue full, drop packet (or could use blocking Send)
                dropped.fetch_add(1);
                std::cout << "Producer: dropped packet " << i << " (queue full)\n";
            } else {
                std::cout << "Producer: sent packet " << i << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        sender->Close();
    });

    // Slow consumer
    std::thread consumer([receiver = std::move(receiver)]() mutable {
        while (auto packet = receiver->Recv()) {
            std::cout << "Consumer: processing packet " << packet->sequence << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Slow processing
        }
    });

    producer.join();
    consumer.join();

    std::cout << "Total dropped packets: " << dropped.load() << "\n";
}

// Example 3: Blocking receive
void blocking_receive_example()
{
    std::cout << "\n=== Blocking Receive Example ===\n";

    auto [sender, receiver] = SpscChannel<std::string>(3);

    std::thread producer([sender = std::move(sender)]() mutable {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        sender->Send("Hello");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        sender->Send("World");
        sender->Close();
    });

    std::thread consumer([receiver = std::move(receiver)]() mutable {
        std::cout << "Consumer: waiting for messages...\n";
        while (auto msg = receiver->Recv()) { // Blocks until data available or closed
            std::cout << "Consumer: got '" << *msg << "'\n";
        }
        std::cout << "Consumer: channel closed\n";
    });

    producer.join();
    consumer.join();
}

int main()
{
    basic_example();
    backpressure_example();
    blocking_receive_example();

    return 0;
}
