/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/time_utils.h>

#include "../test_framework.h"

using namespace lmshao::lmcore;

// Get current time in milliseconds
TEST(TimeUtils, GetCurrentTimeMs)
{
    int64_t ms1 = TimeUtils::GetCurrentTimeMs();
    TimeUtils::SleepMs(10);
    int64_t ms2 = TimeUtils::GetCurrentTimeMs();

    EXPECT_TRUE(ms2 > ms1);
    EXPECT_TRUE(ms2 - ms1 >= 10);
}

// Get current time in microseconds
TEST(TimeUtils, GetCurrentTimeUs)
{
    int64_t us1 = TimeUtils::GetCurrentTimeUs();
    TimeUtils::SleepMs(1);
    int64_t us2 = TimeUtils::GetCurrentTimeUs();

    EXPECT_TRUE(us2 > us1);
    EXPECT_TRUE(us2 - us1 >= 1000);
}

// Get current time in nanoseconds
TEST(TimeUtils, GetCurrentTimeNs)
{
    int64_t ns1 = TimeUtils::GetCurrentTimeNs();
    int64_t ns2 = TimeUtils::GetCurrentTimeNs();

    EXPECT_TRUE(ns2 >= ns1);
}

// Time unit conversions
TEST(TimeUtils, TimeUnitConversions)
{
    int64_t ms = TimeUtils::GetCurrentTimeMs();
    int64_t us = TimeUtils::GetCurrentTimeUs();
    int64_t ns = TimeUtils::GetCurrentTimeNs();

    // Microseconds should be roughly 1000x milliseconds
    EXPECT_TRUE(us / 1000 >= ms - 1 && us / 1000 <= ms + 1);

    // Nanoseconds should be roughly 1000000x milliseconds
    EXPECT_TRUE(ns / 1000000 >= ms - 1 && ns / 1000000 <= ms + 1);
}

// NTP timestamp
TEST(TimeUtils, GetNtpTimestamp)
{
    uint64_t ntp = TimeUtils::GetNtpTimestamp();
    EXPECT_TRUE(ntp > 0);

    // NTP timestamp should have both seconds and fraction parts
    uint32_t seconds = ntp >> 32;
    uint32_t fraction = ntp & 0xFFFFFFFF;

    EXPECT_TRUE(seconds > 0);
    // Fraction might be 0 if called exactly at second boundary
}

// Unix to NTP conversion
TEST(TimeUtils, UnixToNtp)
{
    // Test with known value: 2024-01-01 00:00:00 UTC
    // Unix timestamp: 1704067200000 ms (2024-01-01 00:00:00)
    int64_t unix_ms = 1704067200000LL;
    uint64_t ntp = TimeUtils::UnixToNtp(unix_ms);

    // Extract NTP seconds
    uint64_t ntp_seconds = ntp >> 32;

    // NTP seconds for 2024-01-01 should be around 3912873600
    // (2208988800 + 1704067200 - some timezone adjustment)
    EXPECT_TRUE(ntp_seconds > 3900000000ULL);
}

// NTP to Unix conversion
TEST(TimeUtils, NtpToUnix)
{
    int64_t original_ms = TimeUtils::GetCurrentTimeMs();
    uint64_t ntp = TimeUtils::UnixToNtp(original_ms);
    int64_t converted_ms = TimeUtils::NtpToUnix(ntp);

    // Should be very close (within a few ms due to rounding)
    EXPECT_TRUE(converted_ms >= original_ms - 2 && converted_ms <= original_ms + 2);
}

// Round trip Unix <-> NTP
TEST(TimeUtils, UnixNtpRoundTrip)
{
    int64_t original = 1704067200123LL; // 2024-01-01 00:00:00.123
    uint64_t ntp = TimeUtils::UnixToNtp(original);
    int64_t converted = TimeUtils::NtpToUnix(ntp);

    EXPECT_TRUE(converted >= original - 1 && converted <= original + 1);
}

// RTP timestamp for video (90kHz)
TEST(TimeUtils, GetRtpTimestampVideo)
{
    uint32_t rtp1 = TimeUtils::GetRtpTimestamp(90000);
    TimeUtils::SleepMs(100);
    uint32_t rtp2 = TimeUtils::GetRtpTimestamp(90000);

    // 100ms at 90kHz = 9000 ticks
    uint32_t diff = rtp2 - rtp1;
    EXPECT_TRUE(diff >= 8000 && diff <= 10000);
}

// RTP timestamp for audio (8kHz)
TEST(TimeUtils, GetRtpTimestampAudio)
{
    uint32_t rtp1 = TimeUtils::GetRtpTimestamp(8000);
    TimeUtils::SleepMs(100);
    uint32_t rtp2 = TimeUtils::GetRtpTimestamp(8000);

    // 100ms at 8kHz = 800 ticks
    uint32_t diff = rtp2 - rtp1;
    EXPECT_TRUE(diff >= 700 && diff <= 900);
}

// Format time - default format
TEST(TimeUtils, FormatTimeDefault)
{
    // 2024-01-01 00:00:00 UTC
    int64_t ms = 1704067200000LL;
    std::string formatted = TimeUtils::FormatTime(ms);

    // Format: YYYY-MM-DD HH:MM:SS (local time)
    EXPECT_TRUE(formatted.find("2024") != std::string::npos ||
                formatted.find("2023") != std::string::npos); // Depends on timezone
}

// Format time - custom format
TEST(TimeUtils, FormatTimeCustom)
{
    int64_t ms = 1704067200000LL;
    std::string formatted = TimeUtils::FormatTime(ms, "%Y/%m/%d");

    EXPECT_TRUE(formatted.find("2024") != std::string::npos || formatted.find("2023") != std::string::npos);
    EXPECT_TRUE(formatted.find("/") != std::string::npos);
}

// ISO 8601 format
TEST(TimeUtils, ToISO8601)
{
    int64_t ms = 1704067200000LL; // 2024-01-01 00:00:00
    std::string iso = TimeUtils::ToISO8601(ms);

    EXPECT_TRUE(iso.find("2024-01-01") != std::string::npos);
    EXPECT_TRUE(iso.find("T") != std::string::npos);
    EXPECT_TRUE(iso.find("Z") != std::string::npos);
}

// ISO 8601 with milliseconds
TEST(TimeUtils, ToISO8601WithMilliseconds)
{
    int64_t ms = 1704067200123LL; // 2024-01-01 00:00:00.123
    std::string iso = TimeUtils::ToISO8601(ms);

    EXPECT_TRUE(iso.find(".123Z") != std::string::npos);
}

// Parse ISO 8601
TEST(TimeUtils, FromISO8601)
{
    std::string iso = "2024-01-01T00:00:00Z";
    int64_t ms = TimeUtils::FromISO8601(iso);

    EXPECT_TRUE(ms > 0);
    // Should be around 2024-01-01
    EXPECT_TRUE(ms > 1704000000000LL && ms < 1705000000000LL);
}

// Parse ISO 8601 with milliseconds
TEST(TimeUtils, FromISO8601WithMilliseconds)
{
    std::string iso = "2024-01-01T00:00:00.500Z";
    int64_t ms = TimeUtils::FromISO8601(iso);

    EXPECT_TRUE(ms > 0);
}

// Round trip ISO 8601
TEST(TimeUtils, ISO8601RoundTrip)
{
    int64_t original = 1704067200000LL;
    std::string iso = TimeUtils::ToISO8601(original);
    int64_t converted = TimeUtils::FromISO8601(iso);

    // Should match exactly (no ms in this case)
    EXPECT_EQ(original, converted);
}

// Parse invalid ISO 8601
TEST(TimeUtils, FromISO8601Invalid)
{
    EXPECT_EQ(-1, TimeUtils::FromISO8601(""));
    EXPECT_EQ(-1, TimeUtils::FromISO8601("invalid"));
    EXPECT_EQ(-1, TimeUtils::FromISO8601("2024-13-01T00:00:00Z")); // Invalid month
}

// Sleep milliseconds
TEST(TimeUtils, SleepMs)
{
    int64_t start = TimeUtils::GetCurrentTimeMs();
    TimeUtils::SleepMs(50);
    int64_t end = TimeUtils::GetCurrentTimeMs();

    int64_t elapsed = end - start;
    EXPECT_TRUE(elapsed >= 45 && elapsed <= 100); // Allow some tolerance
}

// Sleep microseconds
TEST(TimeUtils, SleepUs)
{
    int64_t start = TimeUtils::GetCurrentTimeUs();
    TimeUtils::SleepUs(10000); // 10ms
    int64_t end = TimeUtils::GetCurrentTimeUs();

    int64_t elapsed = end - start;
    EXPECT_TRUE(elapsed >= 9000 && elapsed <= 15000); // Allow tolerance
}

RUN_ALL_TESTS()
