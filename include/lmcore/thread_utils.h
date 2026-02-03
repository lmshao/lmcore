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
     * @param name Thread name (max 15 characters on Linux)
     *
     * This function sets the name of the calling thread, which will be visible
     * in system tools like htop, top, and debuggers.
     *
     * Note: On Linux, thread names are limited to 15 characters (excluding null terminator).
     *       Longer names will be automatically truncated.
     */
    static void SetThreadName(const std::string &name);

    /**
     * @brief Set the name of a specific thread
     * @param thread Thread object to name
     * @param name Thread name (max 15 characters on Linux)
     *
     * This function sets the name of a specific thread object.
     */
    static void SetThreadName(std::thread &thread, const std::string &name);
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_THREAD_UTILS_H
