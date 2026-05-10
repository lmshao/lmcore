# lmcore

lmcore 是一个现代 C++17 基础工具库，聚焦于系统级开发中经常重复出现的底层构件：缓冲区、哈希与编码工具、线程池、定时器、通道、对象池，以及一些跨平台小工具。

它的目标是保持轻量、低依赖，并且易于嵌入到更大的项目中，例如服务器、中间件和工程化工具。

## 特性

- 基于 C++17，尽量减少外部依赖
- 支持 Windows、Linux 和 macOS 跨平台构建与使用
- 提供并发原语，包括线程池、任务队列和 Rust 风格有界通道
- 提供基于内部线程池实现的异步定时器
- 提供可复用的内存与对象抽象，例如 DataBuffer 和对象池
- 提供 Base64、Hex、CRC32、MD5、UUID、URL、字符串、字节序和时间相关工具
- 提供内存映射文件支持和日志辅助能力
- 仓库内包含单元测试和可直接运行的示例程序

## 模块概览

公开头文件位于 include/lmcore/。

### 并发模块

- sync.h：有界通道统一入口
- spsc_channel.h：单生产者单消费者通道
- mpsc_channel.h：多生产者单消费者通道
- spmc_channel.h：单生产者多消费者通道
- mpmc_channel.h：多生产者多消费者通道
- thread_pool.h：支持可选串行标签调度的线程池
- task_queue.h：任务分发队列抽象
- async_timer.h：支持一次性和周期性任务的异步定时器
- circular_queue.h：供并发组件复用的有界队列工具

### 内存与数据模块

- data_buffer.h：支持池化分配的动态二进制缓冲区
- object_pool.h：通用对象池，以及专用于 DataBuffer 的 DataBufferPool
- mapped_file.h：内存映射文件封装

### 通用工具模块

- string_utils.h：字符串转换与处理工具
- time_utils.h：时间格式化与时间辅助函数
- thread_utils.h：线程命名与线程辅助函数
- uuid.h：UUID 生成与格式化
- url.h：URL 解析工具
- base64.h、hex.h、crc32.h、md5.h：编码与校验相关工具
- byte_order.h：字节序转换辅助函数
- logger.h、lmcore_logger.h：日志接口与实现辅助

## 仓库结构

```text
lmcore/
├── include/lmcore/    # 对外公开头文件
├── src/               # 具体实现
├── examples/          # 可运行示例程序
├── tests/unit/        # 单元测试
├── cmake/             # CMake 包配置模板
└── scripts/           # 格式化与维护脚本
```

## 环境要求

- CMake 3.10 或更高版本
- 支持 C++17 的编译器
  - GCC 8+
  - Clang 7+
  - 推荐 Visual Studio 2019 或更高版本

## 构建

### Linux 和 macOS

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
# 或者
cmake --build . --config Release
```

## CMake 选项

顶层构建提供以下配置项：

- BUILD_STATIC_LIBS：是否构建静态库，默认 ON
- BUILD_SHARED_LIBS：是否构建共享库，默认 ON
- BUILD_TESTS：是否构建单元测试，默认 ON
- INSTALL_TO_USER_LOCAL：在类 Unix 系统下安装到 ~/.local，默认 OFF

示例：

```bash
# Debug 构建
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 仅构建静态库
cmake .. -DBUILD_SHARED_LIBS=OFF

# 不构建测试
cmake .. -DBUILD_TESTS=OFF
```

## 安装

当前仓库配置在 Unix-like 系统上支持安装。

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
sudo cmake --install .
```

如果希望安装到用户目录而不是系统路径：

```bash
cmake .. -DINSTALL_TO_USER_LOCAL=ON
cmake --build . --parallel
cmake --install .
```

在 Windows 上，当前仓库配置默认不提供 install 和 uninstall target，通常建议直接从构建目录消费该库。

## 在你的项目中使用

### 通过 add_subdirectory

```cmake
add_subdirectory(lmcore)

target_link_libraries(your_app PRIVATE lmcore)
```

### 通过 find_package

在 Linux 或 macOS 完成安装后：

```cmake
find_package(lmcore CONFIG REQUIRED)

target_link_libraries(your_app PRIVATE lmcore::lmcore)
```

## 快速开始

### Rust 风格有界通道

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

### 异步定时器

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

### DataBuffer 对象池

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

## 示例程序

仓库在 examples/ 目录下提供了独立可运行的示例：

- async_timer_example：演示一次性定时器、周期定时器、取消和并发执行
- object_pool_example：演示 DataBufferPool 的申请与复用
- spsc_channel_example：演示生产者/消费者流程与背压
- sync_channels_example：演示多种通道的使用方式

编译后的示例默认输出到 build/examples/。

## 测试

单元测试默认启用。

### 运行全部测试

Linux 和 macOS：

```bash
cd build
ctest --output-on-failure
```

Windows：

```powershell
cd build
ctest -C Debug --output-on-failure
```

### 当前测试覆盖范围

测试集中已经覆盖：

- 异步定时器
- 字节序辅助函数
- 环形队列
- CRC32、MD5、Base64 和 Hex 工具
- DataBuffer 和对象池
- 内存映射文件
- 四种通道类型：SPSC、MPSC、SPMC、MPMC
- 字符串、时间、URL 和 UUID 工具
- 任务队列和线程池

## API 入口

该库以头文件 API 为主。通常可以从以下入口开始：

- include/lmcore/sync.h：通道相关能力
- include/lmcore/async_timer.h：定时器能力
- include/lmcore/thread_pool.h：任务执行与线程池
- include/lmcore/object_pool.h：对象池能力
- include/lmcore/data_buffer.h：二进制数据缓冲

## 与 lmnet 的关系

当前工作区同时包含 lmnet。lmnet 构建在 lmcore 之上；如果你需要 TCP、UDP 或 UNIX 域套接字等网络能力，应使用 lmnet，而 lmcore 适合作为共享的基础工具层。

## 许可证

本项目基于 MIT License 发布，详见 LICENSE。
