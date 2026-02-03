/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2026 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include "lmcore/thread_utils.h"

#if defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#if defined(__linux__)
#include <sys/prctl.h>
#endif
#endif

namespace lmshao::lmcore {

void ThreadUtils::SetThreadName(const std::string &name)
{
#if defined(__APPLE__)
    // macOS: pthread_setname_np takes only the name (sets current thread)
    constexpr size_t maxNameLen = 15;
    pthread_setname_np(name.substr(0, maxNameLen).c_str());
#elif defined(__linux__)
    // Linux: pthread_setname_np takes thread handle and name
    constexpr size_t maxNameLen = 15;
    pthread_setname_np(pthread_self(), name.substr(0, maxNameLen).c_str());
#elif defined(_WIN32)
    // Windows: Use SetThreadDescription (Windows 10 1607+)
    // Note: This requires conversion to wide string
    // For simplicity, we skip Windows implementation here
    // Can be added later if needed
    (void)name; // Suppress unused parameter warning
#else
    (void)name; // Suppress unused parameter warning
#endif
}

void ThreadUtils::SetThreadName(std::thread &thread, const std::string &name)
{
#if defined(__linux__) || defined(__APPLE__)
    constexpr size_t maxNameLen = 15;
    pthread_setname_np(thread.native_handle(), name.substr(0, maxNameLen).c_str());
#elif defined(_WIN32)
    (void)thread;
    (void)name;
#else
    (void)thread;
    (void)name;
#endif
}

} // namespace lmshao::lmcore
