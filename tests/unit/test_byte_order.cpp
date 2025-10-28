/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstring>

#include "../test_framework.h"
#include "lmcore/byte_order.h"

using namespace lmshao::lmcore;

// Test HostToNetwork and NetworkToHost conversions
TEST(ByteOrderTest, HostToNetwork16)
{
    uint16_t value = 0x1234;
    uint16_t network = ByteOrder::HostToNetwork16(value);
    uint16_t host = ByteOrder::NetworkToHost16(network);

    // After round-trip conversion, we should get the original value back
    EXPECT_EQ(value, host);
}

TEST(ByteOrderTest, HostToNetwork32)
{
    uint32_t value = 0x12345678;
    uint32_t network = ByteOrder::HostToNetwork32(value);
    uint32_t host = ByteOrder::NetworkToHost32(network);

    // After round-trip conversion, we should get the original value back
    EXPECT_EQ(value, host);
}

TEST(ByteOrderTest, HostToNetwork64)
{
    uint64_t value = 0x123456789ABCDEF0ULL;
    uint64_t network = ByteOrder::HostToNetwork64(value);
    uint64_t host = ByteOrder::NetworkToHost64(network);

    // After round-trip conversion, we should get the original value back
    EXPECT_EQ(value, host);
}

// Test WriteBE and ReadBE functions
TEST(ByteOrderTest, WriteBE16_ReadBE16)
{
    uint8_t buffer[2];
    uint16_t value = 0x1234;

    ByteOrder::WriteBE16(buffer, value);

    // In big-endian format, 0x1234 should be stored as [0x12, 0x34]
    EXPECT_EQ(buffer[0], 0x12);
    EXPECT_EQ(buffer[1], 0x34);

    // Read back and verify
    uint16_t read_value = ByteOrder::ReadBE16(buffer);
    EXPECT_EQ(read_value, value);
}

TEST(ByteOrderTest, WriteBE32_ReadBE32)
{
    uint8_t buffer[4];
    uint32_t value = 0x12345678;

    ByteOrder::WriteBE32(buffer, value);

    // In big-endian format, 0x12345678 should be stored as [0x12, 0x34, 0x56, 0x78]
    EXPECT_EQ(buffer[0], 0x12);
    EXPECT_EQ(buffer[1], 0x34);
    EXPECT_EQ(buffer[2], 0x56);
    EXPECT_EQ(buffer[3], 0x78);

    // Read back and verify
    uint32_t read_value = ByteOrder::ReadBE32(buffer);
    EXPECT_EQ(read_value, value);
}

TEST(ByteOrderTest, WriteBE64_ReadBE64)
{
    uint8_t buffer[8];
    uint64_t value = 0x123456789ABCDEF0ULL;

    ByteOrder::WriteBE64(buffer, value);

    // In big-endian format, bytes should be stored in order from MSB to LSB
    EXPECT_EQ(buffer[0], 0x12);
    EXPECT_EQ(buffer[1], 0x34);
    EXPECT_EQ(buffer[2], 0x56);
    EXPECT_EQ(buffer[3], 0x78);
    EXPECT_EQ(buffer[4], 0x9A);
    EXPECT_EQ(buffer[5], 0xBC);
    EXPECT_EQ(buffer[6], 0xDE);
    EXPECT_EQ(buffer[7], 0xF0);

    // Read back and verify
    uint64_t read_value = ByteOrder::ReadBE64(buffer);
    EXPECT_EQ(read_value, value);
}

// Test with edge cases
TEST(ByteOrderTest, ZeroValues)
{
    uint8_t buffer[8] = {0};

    ByteOrder::WriteBE16(buffer, 0);
    EXPECT_EQ(ByteOrder::ReadBE16(buffer), 0);

    ByteOrder::WriteBE32(buffer, 0);
    EXPECT_EQ(ByteOrder::ReadBE32(buffer), 0);

    ByteOrder::WriteBE64(buffer, 0);
    EXPECT_EQ(ByteOrder::ReadBE64(buffer), 0);
}

TEST(ByteOrderTest, MaxValues)
{
    uint8_t buffer[8];

    uint16_t max16 = 0xFFFF;
    ByteOrder::WriteBE16(buffer, max16);
    EXPECT_EQ(buffer[0], 0xFF);
    EXPECT_EQ(buffer[1], 0xFF);
    EXPECT_EQ(ByteOrder::ReadBE16(buffer), max16);

    uint32_t max32 = 0xFFFFFFFF;
    ByteOrder::WriteBE32(buffer, max32);
    EXPECT_EQ(buffer[0], 0xFF);
    EXPECT_EQ(buffer[1], 0xFF);
    EXPECT_EQ(buffer[2], 0xFF);
    EXPECT_EQ(buffer[3], 0xFF);
    EXPECT_EQ(ByteOrder::ReadBE32(buffer), max32);

    uint64_t max64 = 0xFFFFFFFFFFFFFFFFULL;
    ByteOrder::WriteBE64(buffer, max64);
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(buffer[i], 0xFF);
    }
    EXPECT_EQ(ByteOrder::ReadBE64(buffer), max64);
}

// Test boundary values
TEST(ByteOrderTest, BoundaryValues16)
{
    uint8_t buffer[2];

    // Test 0x0100 (256)
    ByteOrder::WriteBE16(buffer, 0x0100);
    EXPECT_EQ(buffer[0], 0x01);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(ByteOrder::ReadBE16(buffer), 0x0100);

    // Test 0x00FF (255)
    ByteOrder::WriteBE16(buffer, 0x00FF);
    EXPECT_EQ(buffer[0], 0x00);
    EXPECT_EQ(buffer[1], 0xFF);
    EXPECT_EQ(ByteOrder::ReadBE16(buffer), 0x00FF);
}

TEST(ByteOrderTest, BoundaryValues32)
{
    uint8_t buffer[4];

    // Test 0x01000000
    ByteOrder::WriteBE32(buffer, 0x01000000);
    EXPECT_EQ(buffer[0], 0x01);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x00);
    EXPECT_EQ(buffer[3], 0x00);
    EXPECT_EQ(ByteOrder::ReadBE32(buffer), 0x01000000);

    // Test 0x000000FF
    ByteOrder::WriteBE32(buffer, 0x000000FF);
    EXPECT_EQ(buffer[0], 0x00);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x00);
    EXPECT_EQ(buffer[3], 0xFF);
    EXPECT_EQ(ByteOrder::ReadBE32(buffer), 0x000000FF);
}

// Test system endianness detection
TEST(ByteOrderTest, SystemEndianness)
{
    bool is_little = ByteOrder::IsSystemLittleEndian();

    // On x86/x64/ARM64 systems, this should be true
    // This test just ensures the function is callable and returns a boolean
    EXPECT_TRUE(is_little == true || is_little == false);
}

// Test that ReadBE correctly interprets manually set buffer bytes
TEST(ByteOrderTest, ReadBE16_ManualBuffer)
{
    uint8_t buffer[2] = {0xAB, 0xCD};
    uint16_t value = ByteOrder::ReadBE16(buffer);

    // 0xAB is high byte, 0xCD is low byte
    // So value should be 0xABCD
    EXPECT_EQ(value, 0xABCD);
}

TEST(ByteOrderTest, ReadBE32_ManualBuffer)
{
    uint8_t buffer[4] = {0x12, 0x34, 0x56, 0x78};
    uint32_t value = ByteOrder::ReadBE32(buffer);

    EXPECT_EQ(value, 0x12345678);
}

TEST(ByteOrderTest, ReadBE64_ManualBuffer)
{
    uint8_t buffer[8] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    uint64_t value = ByteOrder::ReadBE64(buffer);

    EXPECT_EQ(value, 0x123456789ABCDEF0ULL);
}

// Test multiple writes and reads in sequence
TEST(ByteOrderTest, SequentialOperations)
{
    uint8_t buffer[8];

    // Write and read multiple values
    for (uint16_t i = 0; i < 1000; i++) {
        ByteOrder::WriteBE16(buffer, i);
        uint16_t read = ByteOrder::ReadBE16(buffer);
        EXPECT_EQ(read, i);
    }
}

// Test 24-bit big-endian operations
TEST(ByteOrderTest, ReadBE24_WriteBE24)
{
    uint8_t buffer[3];
    uint32_t value = 0x123456;

    ByteOrder::WriteBE24(buffer, value);

    // Check buffer bytes
    EXPECT_EQ(buffer[0], 0x12);
    EXPECT_EQ(buffer[1], 0x34);
    EXPECT_EQ(buffer[2], 0x56);

    // Read back
    uint32_t read_value = ByteOrder::ReadBE24(buffer);
    EXPECT_EQ(read_value, value);
}

TEST(ByteOrderTest, ReadBE24_MaxValue)
{
    uint8_t buffer[3] = {0xFF, 0xFF, 0xFF};
    uint32_t value = ByteOrder::ReadBE24(buffer);
    EXPECT_EQ(value, 0x00FFFFFF);
}

TEST(ByteOrderTest, WriteBE24_TruncatesUpperBits)
{
    uint8_t buffer[3];
    uint32_t value = 0x12345678;

    ByteOrder::WriteBE24(buffer, value);

    // Only lower 24 bits should be written
    EXPECT_EQ(buffer[0], 0x34);
    EXPECT_EQ(buffer[1], 0x56);
    EXPECT_EQ(buffer[2], 0x78);

    uint32_t read_value = ByteOrder::ReadBE24(buffer);
    EXPECT_EQ(read_value, 0x345678);
}

// Test byte swap functions
TEST(ByteOrderTest, Swap16)
{
    EXPECT_EQ(ByteOrder::Swap16(0x1234), 0x3412);
    EXPECT_EQ(ByteOrder::Swap16(0xABCD), 0xCDAB);
    EXPECT_EQ(ByteOrder::Swap16(0x0000), 0x0000);
    EXPECT_EQ(ByteOrder::Swap16(0xFFFF), 0xFFFF);

    // Double swap should return original
    uint16_t original = 0x1234;
    EXPECT_EQ(ByteOrder::Swap16(ByteOrder::Swap16(original)), original);
}

TEST(ByteOrderTest, Swap32)
{
    EXPECT_EQ(ByteOrder::Swap32(0x12345678), 0x78563412);
    EXPECT_EQ(ByteOrder::Swap32(0xABCDEF01), 0x01EFCDAB);
    EXPECT_EQ(ByteOrder::Swap32(0x00000000), 0x00000000);
    EXPECT_EQ(ByteOrder::Swap32(0xFFFFFFFF), 0xFFFFFFFF);

    // Double swap should return original
    uint32_t original = 0x12345678;
    EXPECT_EQ(ByteOrder::Swap32(ByteOrder::Swap32(original)), original);
}

TEST(ByteOrderTest, Swap64)
{
    EXPECT_EQ(ByteOrder::Swap64(0x123456789ABCDEF0ULL), 0xF0DEBC9A78563412ULL);
    EXPECT_EQ(ByteOrder::Swap64(0x0000000000000000ULL), 0x0000000000000000ULL);
    EXPECT_EQ(ByteOrder::Swap64(0xFFFFFFFFFFFFFFFFULL), 0xFFFFFFFFFFFFFFFFULL);

    // Double swap should return original
    uint64_t original = 0x123456789ABCDEF0ULL;
    EXPECT_EQ(ByteOrder::Swap64(ByteOrder::Swap64(original)), original);
}

TEST(ByteOrderTest, SwapRelationship)
{
    // On little-endian systems, Swap should equal HostToNetwork
#if LMCORE_LITTLE_ENDIAN
    uint16_t val16 = 0x1234;
    EXPECT_EQ(ByteOrder::Swap16(val16), ByteOrder::HostToNetwork16(val16));

    uint32_t val32 = 0x12345678;
    EXPECT_EQ(ByteOrder::Swap32(val32), ByteOrder::HostToNetwork32(val32));

    uint64_t val64 = 0x123456789ABCDEF0ULL;
    EXPECT_EQ(ByteOrder::Swap64(val64), ByteOrder::HostToNetwork64(val64));
#endif
}

RUN_ALL_TESTS()
