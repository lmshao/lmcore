# lmcore

lmcore is a modern C++17 utility library focused on the low-level building blocks that show up repeatedly in systems code: buffers, hashing and encoding helpers, thread pools, timers, channels, object pools, and small cross-platform utilities.

It is designed to be small, dependency-light, and easy to embed into larger projects such as servers, middleware, and tools.

## Features

- C++17 codebase with minimal external dependencies
- Cross-platform support for Windows, Linux, and macOS
- Concurrent primitives including thread pool, task queue, and Rust-style bounded channels
- Asynchronous timer built on top of the internal thread pool
- Reusable memory and object abstractions such as DataBuffer and object pools
- Utility modules for Base64, Hex, CRC32, MD5, UUID, URL, string helpers, byte order, and time helpers
- Memory-mapped file support and logging helpers
- Unit tests and runnable examples included in the repository

## Module Overview

Public headers live under include/lmcore/.

### Concurrency

- sync.h: unified entry for bounded channels
- spsc_channel.h: single-producer single-consumer channel
- mpsc_channel.h: multi-producer single-consumer channel
- spmc_channel.h: single-producer multi-consumer channel
- mpmc_channel.h: multi-producer multi-consumer channel
- thread_pool.h: task execution with optional serial-tag ordering
- task_queue.h: task dispatch queue abstraction
- async_timer.h: one-shot and repeating timers executed asynchronously
- circular_queue.h: bounded queue utility used by concurrent components

### Memory and Data

- data_buffer.h: resizable binary buffer with pooled allocation helpers
- object_pool.h: generic object pool and specialized DataBufferPool
- mapped_file.h: memory-mapped file wrapper

### Utilities

- string_utils.h: string conversion and manipulation helpers
- time_utils.h: time formatting and time-related helpers
- thread_utils.h: thread naming and thread-related helpers
- uuid.h: UUID generation and formatting
- url.h: URL parsing helpers
- base64.h, hex.h, crc32.h, md5.h: encoding and checksum utilities
- byte_order.h: byte order conversion helpers
- logger.h, lmcore_logger.h: logging interfaces and implementation support

## Repository Layout

```text
lmcore/
├── include/lmcore/    # Public headers
├── src/               # Implementations
├── examples/          # Runnable sample programs
├── tests/unit/        # Unit tests
├── cmake/             # Package config templates
└── scripts/           # Formatting and maintenance helpers
```

## Requirements

- CMake 3.10 or newer
- A compiler with C++17 support
  - GCC 8+
  - Clang 7+
  - Visual Studio 2019 or newer recommended

## Build

### Linux and macOS

```bash
git clone https://github.com/lmshao/lmcore.git
cd lmcore
mkdir build
cd build
cmake ..
cmake --build . --parallel
```

### Windows

```powershell
git clone https://github.com/lmshao/lmcore.git
cd lmcore
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
# or
cmake --build . --config Release
```

## CMake Options

The top-level build exposes the following options:

- BUILD_STATIC_LIBS: build the static library, default ON
- BUILD_SHARED_LIBS: build the shared library, default ON
- BUILD_TESTS: build unit tests, default ON
- INSTALL_TO_USER_LOCAL: install to ~/.local on Unix-like systems, default OFF

Examples:

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build only the static library
cmake .. -DBUILD_SHARED_LIBS=OFF

# Build without tests
cmake .. -DBUILD_TESTS=OFF
```

## Install

Install support is enabled on Unix-like systems.

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
sudo cmake --install .
```

Install to a user-local prefix instead of a system path:

```bash
cmake .. -DINSTALL_TO_USER_LOCAL=ON
cmake --build . --parallel
cmake --install .
```

On Windows, install and uninstall targets are currently disabled in this repository configuration. The library is intended to be consumed directly from the build tree in that environment.

## Use in Your Project

### Via add_subdirectory

```cmake
add_subdirectory(lmcore)

target_link_libraries(your_app PRIVATE lmcore)
```

### Via find_package

After installing on Linux or macOS:

```cmake
find_package(lmcore CONFIG REQUIRED)

target_link_libraries(your_app PRIVATE lmcore::lmcore)
```

## Quick Start

### Rust-style bounded channel

```cpp
#include <iostream>
#include <thread>

#include "lmcore/sync.h"

using namespace lmshao::lmcore::sync;

int main()
{
	auto [sender, receiver] = SpscChannel<int>(16);

	std::thread producer([sender = std::move(sender)]() mutable {
		for (int i = 0; i < 5; ++i) {
			sender->Send(i);
		}
		sender->Close();
	});

	while (auto value = receiver->Recv()) {
		std::cout << "received: " << *value << std::endl;
	}

	producer.join();
	return 0;
}
```

### Async timer

```cpp
#include <chrono>
#include <iostream>
#include <thread>

#include "lmcore/async_timer.h"

using namespace lmshao::lmcore;

int main()
{
	AsyncTimer timer(2);
	timer.Start();

	timer.ScheduleOnce([]() { std::cout << "one-shot timer" << std::endl; }, 500);
	timer.ScheduleRepeating([]() { std::cout << "tick" << std::endl; }, 300, 100);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	timer.Stop();
	return 0;
}
```

### DataBuffer pool

```cpp
#include <iostream>

#include "lmcore/object_pool.h"

using namespace lmshao::lmcore;

int main()
{
	DataBufferPool pool(1024, 8);

	auto buffer = pool.Acquire();
	buffer->Assign("hello lmcore");

	std::cout << buffer->ToString() << std::endl;
	return 0;
}
```

## Examples

The repository contains standalone examples under examples/:

- async_timer_example: one-shot and repeating timers, cancellation, concurrency
- object_pool_example: DataBufferPool allocation and reuse
- spsc_channel_example: producer/consumer flow and backpressure
- sync_channels_example: broader channel usage patterns

Built examples are emitted into build/examples/.

## Testing

Unit tests are enabled by default.

### Run all tests

Linux and macOS:

```bash
cd build
ctest --output-on-failure
```

Windows:

```powershell
cd build
ctest -C Debug --output-on-failure
```

### Current test coverage

The test suite includes dedicated coverage for:

- async timer
- byte order helpers
- circular queue
- CRC32, MD5, Base64, and Hex helpers
- data buffer and object pool
- mapped file
- all four channel types: SPSC, MPSC, SPMC, MPMC
- string, time, URL, and UUID helpers
- task queue and thread pool

## API Surface

The primary API is header-driven. Start with these headers depending on the problem you are solving:

- include/lmcore/sync.h for channels
- include/lmcore/async_timer.h for timers
- include/lmcore/thread_pool.h for task execution
- include/lmcore/object_pool.h for pooling
- include/lmcore/data_buffer.h for binary payload handling

## Relationship to lmnet

This workspace also contains lmnet, which builds on top of lmcore. If you need networking primitives such as TCP, UDP, or UNIX domain sockets, use lmnet and keep lmcore as the shared utility layer.

## License

This project is licensed under the MIT License. See LICENSE for details.
