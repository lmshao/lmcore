/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/crc32.h>

#include <cstring>

#include "../test_framework.h"

using namespace lmshao::lmcore;

TEST(CRC32, EmptyString)
{
    uint32_t result = CRC32::Calculate(reinterpret_cast<const uint8_t *>(""), 0);
    EXPECT_EQ(0, result);
}

TEST(CRC32, SimpleString)
{
    uint32_t result = CRC32::Calculate("123456789");
    EXPECT_EQ(0xCBF43926, result);
}

TEST(CRC32, StringVersion)
{
    std::string data = "123456789";
    uint32_t result = CRC32::Calculate(data);
    EXPECT_EQ(0xCBF43926, result);
}

TEST(CRC32, VectorVersion)
{
    std::vector<uint8_t> data = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint32_t result = CRC32::Calculate(data);
    EXPECT_EQ(0xCBF43926, result);
}

TEST(CRC32, BinaryData)
{
    uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    uint32_t result = CRC32::Calculate(data, sizeof(data));
    EXPECT_GT(result, 0);
}

TEST(CRC32, LongString)
{
    std::string data = "The quick brown fox jumps over the lazy dog";
    uint32_t result = CRC32::Calculate(data);
    EXPECT_EQ(0x414FA339, result);
}

TEST(CRC32, UpdateIncremental)
{
    std::string part1 = "Hello";
    std::string part2 = "World";

    CRC32::Context ctx;
    ctx.Update(part1);
    ctx.Update(part2);
    uint32_t crc = ctx.Final();

    uint32_t expected = CRC32::Calculate("HelloWorld");
    EXPECT_EQ(expected, crc);
}

TEST(CRC32, ContextReset)
{
    CRC32::Context ctx;
    ctx.Update("Hello");
    ctx.Reset();
    ctx.Update("World");
    uint32_t crc = ctx.Final();

    uint32_t expected = CRC32::Calculate("World");
    EXPECT_EQ(expected, crc);
}

TEST(CRC32, ContextMultipleFinal)
{
    CRC32::Context ctx;
    ctx.Update("Test");

    uint32_t crc1 = ctx.Final();
    uint32_t crc2 = ctx.Final();

    EXPECT_EQ(crc1, crc2);
}

TEST(CRC32, AllZeros)
{
    uint8_t data[100] = {0};
    uint32_t result = CRC32::Calculate(data, sizeof(data));
    EXPECT_GT(result, 0);
}

TEST(CRC32, AllOnes)
{
    uint8_t data[100];
    memset(data, 0xFF, sizeof(data));
    uint32_t result = CRC32::Calculate(data, sizeof(data));
    EXPECT_GT(result, 0);
}

TEST(CRC32, DifferentDataSameLength)
{
    uint32_t crc1 = CRC32::Calculate("abcdefgh");
    uint32_t crc2 = CRC32::Calculate("12345678");
    EXPECT_NE(crc1, crc2);
}

TEST(CRC32, SensitiveToOrder)
{
    uint32_t crc1 = CRC32::Calculate("abc");
    uint32_t crc2 = CRC32::Calculate("cba");
    EXPECT_NE(crc1, crc2);
}

TEST(CRC32, Large1MB)
{
    std::vector<uint8_t> data(1024 * 1024);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = static_cast<uint8_t>(i % 256);
    }

    uint32_t result = CRC32::Calculate(data);
    EXPECT_GT(result, 0);
}

RUN_ALL_TESTS()
