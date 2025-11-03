/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/time_utils.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>

namespace lmshao::lmcore {

int64_t TimeUtils::GetCurrentTimeMs()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

int64_t TimeUtils::GetCurrentTimeUs()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

int64_t TimeUtils::GetCurrentTimeNs()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

uint64_t TimeUtils::GetNtpTimestamp()
{
    return UnixToNtp(GetCurrentTimeMs());
}

uint64_t TimeUtils::UnixToNtp(int64_t unix_time_ms)
{
    // Convert Unix time (ms since 1970) to NTP time (since 1900)
    uint64_t unix_seconds = unix_time_ms / 1000;
    uint64_t unix_ms_frac = unix_time_ms % 1000;

    // NTP seconds = Unix seconds + offset
    uint64_t ntp_seconds = unix_seconds + NTP_EPOCH_OFFSET;

    // Convert milliseconds to NTP fraction (32-bit, units of 1/(2^32) seconds)
    // fraction = (ms / 1000) * 2^32
    uint64_t ntp_fraction = (unix_ms_frac * 0x100000000ULL) / 1000;

    // Combine: high 32 bits = seconds, low 32 bits = fraction
    return (ntp_seconds << 32) | (ntp_fraction & 0xFFFFFFFF);
}

int64_t TimeUtils::NtpToUnix(uint64_t ntp_timestamp)
{
    // Extract NTP seconds and fraction
    uint64_t ntp_seconds = ntp_timestamp >> 32;
    uint64_t ntp_fraction = ntp_timestamp & 0xFFFFFFFF;

    // Convert NTP seconds to Unix seconds
    int64_t unix_seconds = ntp_seconds - NTP_EPOCH_OFFSET;

    // Convert NTP fraction to milliseconds
    // ms = (fraction * 1000) / 2^32
    int64_t unix_ms_frac = (ntp_fraction * 1000) / 0x100000000ULL;

    return unix_seconds * 1000 + unix_ms_frac;
}

uint32_t TimeUtils::GetRtpTimestamp(uint32_t clock_rate)
{
    // RTP timestamp is based on microseconds since some arbitrary epoch
    // For simplicity, use current time in microseconds
    int64_t us = GetCurrentTimeUs();

    // Convert to RTP clock units
    // RTP_timestamp = (us / 1000000) * clock_rate
    uint64_t rtp_timestamp = (static_cast<uint64_t>(us) * clock_rate) / 1000000ULL;

    return static_cast<uint32_t>(rtp_timestamp & 0xFFFFFFFF);
}

std::string TimeUtils::FormatTime(int64_t timestamp_ms, const std::string &format)
{
    time_t seconds = timestamp_ms / 1000;
    std::tm tm_info;

#ifdef _WIN32
    localtime_s(&tm_info, &seconds);
#else
    localtime_r(&seconds, &tm_info);
#endif

    char buffer[256];
    std::strftime(buffer, sizeof(buffer), format.c_str(), &tm_info);

    return std::string(buffer);
}

int64_t TimeUtils::ParseTime(const std::string &time_str, const std::string &format)
{
    std::tm tm_info = {};

#ifdef _WIN32
    std::istringstream ss(time_str);
    ss >> std::get_time(&tm_info, format.c_str());
    if (ss.fail()) {
        return -1;
    }
#else
    if (strptime(time_str.c_str(), format.c_str(), &tm_info) == nullptr) {
        return -1;
    }
#endif

    time_t seconds = std::mktime(&tm_info);
    if (seconds == -1) {
        return -1;
    }

    return static_cast<int64_t>(seconds) * 1000;
}

std::string TimeUtils::ToISO8601(int64_t timestamp_ms)
{
    time_t seconds = timestamp_ms / 1000;
    int ms = timestamp_ms % 1000;

    std::tm tm_info;
#ifdef _WIN32
    gmtime_s(&tm_info, &seconds);
#else
    gmtime_r(&seconds, &tm_info);
#endif

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm_info);

    // Add milliseconds if non-zero
    if (ms > 0) {
        char ms_buffer[16];
        snprintf(ms_buffer, sizeof(ms_buffer), ".%03dZ", ms);
        return std::string(buffer) + ms_buffer;
    } else {
        return std::string(buffer) + "Z";
    }
}

int64_t TimeUtils::FromISO8601(const std::string &iso_str)
{
    if (iso_str.empty()) {
        return -1;
    }

    std::tm tm_info = {};
    int ms = 0;

    // Try parsing with milliseconds: YYYY-MM-DDTHH:MM:SS.sssZ
    const char *result = nullptr;

#ifdef _WIN32
    std::istringstream ss(iso_str);
    ss >> std::get_time(&tm_info, "%Y-%m-%dT%H:%M:%S");
    if (!ss.fail()) {
        // Try to read milliseconds
        char dot;
        if (ss >> dot && dot == '.') {
            ss >> ms;
        }
        result = iso_str.c_str(); // Indicate success
    }
#else
    result = strptime(iso_str.c_str(), "%Y-%m-%dT%H:%M:%S", &tm_info);
    if (result != nullptr) {
        // Try to read milliseconds after the parsed part
        if (*result == '.') {
            ms = std::atoi(result + 1);
        }
    }
#endif

    if (result == nullptr) {
        return -1;
    }

    // Convert to UTC time
#ifdef _WIN32
    time_t seconds = _mkgmtime(&tm_info);
#else
    time_t seconds = timegm(&tm_info);
#endif

    if (seconds == -1) {
        return -1;
    }

    return static_cast<int64_t>(seconds) * 1000 + ms;
}

void TimeUtils::SleepMs(int64_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void TimeUtils::SleepUs(int64_t us)
{
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

} // namespace lmshao::lmcore
