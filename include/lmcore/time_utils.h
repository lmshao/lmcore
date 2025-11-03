/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_TIME_UTILS_H
#define LMSHAO_LMCORE_TIME_UTILS_H

#include <cstdint>
#include <string>

namespace lmshao::lmcore {

/**
 * @brief Time and timestamp utilities
 *
 * Provides high-resolution timestamps, time formatting, and conversions
 * commonly used in multimedia applications (RTP, RTSP, etc.).
 *
 * Example usage:
 * @code
 *   // Get current timestamp
 *   int64_t ms = TimeUtils::GetCurrentTimeMs();
 *   int64_t us = TimeUtils::GetCurrentTimeUs();
 *
 *   // NTP timestamp for RTP/RTSP
 *   uint64_t ntp = TimeUtils::GetNtpTimestamp();
 *
 *   // RTP timestamp (90kHz clock)
 *   uint32_t rtp = TimeUtils::GetRtpTimestamp();
 *
 *   // Format timestamp
 *   std::string formatted = TimeUtils::FormatTime(ms, "%Y-%m-%d %H:%M:%S");
 *   // Result: "2024-11-04 00:04:00"
 *
 *   // ISO 8601 format (for RTSP Range header)
 *   std::string iso = TimeUtils::ToISO8601(ms);
 *   // Result: "2024-11-04T00:04:00Z"
 * @endcode
 */
class TimeUtils {
public:
    /**
     * @brief Get current time in milliseconds since epoch
     * @return Milliseconds since 1970-01-01 00:00:00 UTC
     */
    static int64_t GetCurrentTimeMs();

    /**
     * @brief Get current time in microseconds since epoch
     * @return Microseconds since 1970-01-01 00:00:00 UTC
     */
    static int64_t GetCurrentTimeUs();

    /**
     * @brief Get current time in nanoseconds since epoch
     * @return Nanoseconds since 1970-01-01 00:00:00 UTC
     */
    static int64_t GetCurrentTimeNs();

    /**
     * @brief Get NTP timestamp (RFC 1305)
     * @return 64-bit NTP timestamp (32-bit seconds + 32-bit fraction)
     *
     * NTP epoch: 1900-01-01 00:00:00 UTC
     * Used in RTP/RTCP protocols
     */
    static uint64_t GetNtpTimestamp();

    /**
     * @brief Get NTP timestamp from Unix timestamp
     * @param unix_time_ms Unix timestamp in milliseconds
     * @return 64-bit NTP timestamp
     */
    static uint64_t UnixToNtp(int64_t unix_time_ms);

    /**
     * @brief Convert NTP timestamp to Unix timestamp
     * @param ntp_timestamp 64-bit NTP timestamp
     * @return Unix timestamp in milliseconds
     */
    static int64_t NtpToUnix(uint64_t ntp_timestamp);

    /**
     * @brief Get RTP timestamp for current time
     * @param clock_rate RTP clock rate (default: 90000 for video)
     * @return 32-bit RTP timestamp
     *
     * Common clock rates:
     * - Video: 90000 Hz
     * - Audio (8kHz): 8000 Hz
     * - Audio (44.1kHz): 44100 Hz
     * - Audio (48kHz): 48000 Hz
     */
    static uint32_t GetRtpTimestamp(uint32_t clock_rate = 90000);

    /**
     * @brief Format Unix timestamp to string
     * @param timestamp_ms Unix timestamp in milliseconds
     * @param format Format string (strftime compatible)
     * @return Formatted time string
     *
     * Common format specifiers:
     * - %Y: Year (4 digits)
     * - %m: Month (01-12)
     * - %d: Day (01-31)
     * - %H: Hour (00-23)
     * - %M: Minute (00-59)
     * - %S: Second (00-59)
     */
    static std::string FormatTime(int64_t timestamp_ms, const std::string &format = "%Y-%m-%d %H:%M:%S");

    /**
     * @brief Parse time string to Unix timestamp
     * @param time_str Time string
     * @param format Format string (strptime compatible)
     * @return Unix timestamp in milliseconds, or -1 on error
     */
    static int64_t ParseTime(const std::string &time_str, const std::string &format);

    /**
     * @brief Convert Unix timestamp to ISO 8601 format
     * @param timestamp_ms Unix timestamp in milliseconds
     * @return ISO 8601 formatted string (e.g., "2024-11-04T00:04:00Z")
     *
     * Used in RTSP Range header
     */
    static std::string ToISO8601(int64_t timestamp_ms);

    /**
     * @brief Parse ISO 8601 time string to Unix timestamp
     * @param iso_str ISO 8601 formatted string
     * @return Unix timestamp in milliseconds, or -1 on error
     */
    static int64_t FromISO8601(const std::string &iso_str);

    /**
     * @brief Sleep for specified milliseconds
     * @param ms Milliseconds to sleep
     */
    static void SleepMs(int64_t ms);

    /**
     * @brief Sleep for specified microseconds
     * @param us Microseconds to sleep
     */
    static void SleepUs(int64_t us);

private:
    // NTP epoch offset: seconds between 1900-01-01 and 1970-01-01
    static constexpr uint64_t NTP_EPOCH_OFFSET = 2208988800ULL;
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_TIME_UTILS_H
