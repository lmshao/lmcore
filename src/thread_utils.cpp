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
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace lmshao::lmcore {

namespace {

std::string TruncateThreadName(const std::string &name)
{
#if defined(__APPLE__)
    return name.substr(0, 63);
#elif defined(__linux__)
    return name.substr(0, 15);
#else
    return name;
#endif
}

#if defined(_WIN32)
std::wstring ToWideString(const std::string &text)
{
    return std::wstring(text.begin(), text.end());
}
#endif

} // namespace

void ThreadUtils::SetThreadName(const std::string &name)
{
#if defined(__APPLE__)
    pthread_setname_np(TruncateThreadName(name).c_str());
#elif defined(__linux__)
    pthread_setname_np(pthread_self(), TruncateThreadName(name).c_str());
#elif defined(_WIN32)
    std::wstring wideName = ToWideString(name);
    SetThreadDescription(GetCurrentThread(), wideName.c_str());
#else
    (void)name;
#endif
}

void ThreadUtils::SetThreadName(std::thread &thread, const std::string &name)
{
#if defined(__APPLE__)
    if (pthread_equal(thread.native_handle(), pthread_self()) != 0) {
        pthread_setname_np(TruncateThreadName(name).c_str());
    }
#elif defined(__linux__)
    pthread_setname_np(thread.native_handle(), TruncateThreadName(name).c_str());
#elif defined(_WIN32)
    std::wstring wideName = ToWideString(name);
    SetThreadDescription(thread.native_handle(), wideName.c_str());
#else
    (void)thread;
    (void)name;
#endif
}

} // namespace lmshao::lmcore
