/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2026 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_THREAD_UTILS_H
#define LMSHAO_LMCORE_THREAD_UTILS_H

#include <string>
#include <thread>

namespace lmshao::lmcore {

/**
 * @brief Thread management utilities
 *
 * Provides cross-platform thread naming and management utilities.
 *
 * Example usage:
 * @code
 *   // In thread function
 *   void WorkerThread() {
 *       ThreadUtils::SetThreadName("my_worker");
 *       // ... thread work ...
 *   }
 *
 *   // Or for a specific thread
 *   std::thread t(WorkerFunc);
 *   ThreadUtils::SetThreadName(t, "my_worker");
 * @endcode
 */
class ThreadUtils {
public:
    /**
     * @brief Set the name of the calling thread
     * @param name Thread name (max 15 characters on Linux, 63 on macOS)
     *
     * This function sets the name of the calling thread, which will be visible
     * in system tools like htop, top, and debuggers.
     *
     * Note: Thread names are limited by the underlying platform.
     *       Linux supports up to 15 characters and macOS up to 63 characters
     *       (excluding the null terminator). Longer names are automatically truncated.
     */
    static void SetThreadName(const std::string &name);

    /**
     * @brief Set the name of a specific thread
     * @param thread Thread object to name
     * @param name Thread name (max 15 characters on Linux, 63 on macOS)
     *
     * This function sets the name of a specific thread object.
     * On macOS, the platform API only supports naming the calling thread,
     * so this function is effective only when `thread` refers to the current thread.
     */
    static void SetThreadName(std::thread &thread, const std::string &name);
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_THREAD_UTILS_H
