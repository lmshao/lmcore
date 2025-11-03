/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/md5.h>

#include <fstream>

#include "../test_framework.h"

using namespace lmshao::lmcore;

TEST(MD5, EmptyString)
{
    std::string result = MD5::Calculate("");
    EXPECT_EQ("d41d8cd98f00b204e9800998ecf8427e", result);
}

TEST(MD5, SimpleString)
{
    std::string result = MD5::Calculate("Hello");
    EXPECT_EQ("8b1a9953c4611296a827abf8c47804d7", result);
}

TEST(MD5, StandardTestVector1)
{
    std::string result = MD5::Calculate("a");
    EXPECT_EQ("0cc175b9c0f1b6a831c399e269772661", result);
}

TEST(MD5, StandardTestVector2)
{
    std::string result = MD5::Calculate("abc");
    EXPECT_EQ("900150983cd24fb0d6963f7d28e17f72", result);
}

TEST(MD5, StandardTestVector3)
{
    std::string result = MD5::Calculate("message digest");
    EXPECT_EQ("f96b697d7cb7938d525a2f31aaf161d0", result);
}

TEST(MD5, StandardTestVector4)
{
    std::string result = MD5::Calculate("abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ("c3fcd3d76192e4007dfb496cca67e13b", result);
}

TEST(MD5, StandardTestVector5)
{
    std::string result = MD5::Calculate("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    EXPECT_EQ("d174ab98d277d9f5a5611c2c9f419d9f", result);
}

TEST(MD5, StandardTestVector6)
{
    std::string input = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
    std::string result = MD5::Calculate(input);
    EXPECT_EQ("57edf4a22be3c955ac49da2e2107b67a", result);
}

TEST(MD5, LongString)
{
    std::string input = "The quick brown fox jumps over the lazy dog";
    std::string result = MD5::Calculate(input);
    EXPECT_EQ("9e107d9d372bb6826bd81d3542a419d6", result);
}

TEST(MD5, VectorVersion)
{
    std::vector<uint8_t> data = {'H', 'e', 'l', 'l', 'o'};
    std::string result = MD5::Calculate(data);
    EXPECT_EQ("8b1a9953c4611296a827abf8c47804d7", result);
}

TEST(MD5, BinaryData)
{
    uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD};
    std::string result = MD5::Calculate(data, sizeof(data));
    EXPECT_EQ(32, result.length());
    for (char c : result) {
        EXPECT_TRUE((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
    }
}

TEST(MD5, CalculateFile)
{
    const std::string test_file = "test_md5_file.txt";
    const std::string test_content = "MD5 file test content";

    {
        std::ofstream file(test_file);
        file << test_content;
    }

    std::string file_hash = MD5::CalculateFile(test_file);
    std::string string_hash = MD5::Calculate(test_content);

    EXPECT_EQ(string_hash, file_hash);

    std::remove(test_file.c_str());
}

TEST(MD5, CalculateNonExistentFile)
{
    std::string result = MD5::CalculateFile("non_existent_file_12345.txt");
    EXPECT_EQ("", result);
}

TEST(MD5, CalculateLargeFile)
{
    const std::string test_file = "test_md5_large_file.bin";
    const size_t file_size = 1024 * 1024;

    {
        std::ofstream file(test_file, std::ios::binary);
        for (size_t i = 0; i < file_size; i++) {
            uint8_t byte = static_cast<uint8_t>(i % 256);
            file.write(reinterpret_cast<const char *>(&byte), 1);
        }
    }

    std::string result = MD5::CalculateFile(test_file);
    EXPECT_EQ(32, result.length());

    std::remove(test_file.c_str());
}

TEST(MD5, DifferentInputDifferentHash)
{
    std::string hash1 = MD5::Calculate("Hello");
    std::string hash2 = MD5::Calculate("World");
    EXPECT_NE(hash1, hash2);
}

TEST(MD5, SameInputSameHash)
{
    std::string hash1 = MD5::Calculate("Test");
    std::string hash2 = MD5::Calculate("Test");
    EXPECT_EQ(hash1, hash2);
}

TEST(MD5, OutputFormat)
{
    std::string result = MD5::Calculate("test");
    EXPECT_EQ(32, result.length());
    for (char c : result) {
        EXPECT_TRUE((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
    }
}

RUN_ALL_TESTS()
