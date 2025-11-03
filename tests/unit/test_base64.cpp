/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/base64.h>

#include "../test_framework.h"

using namespace lmshao::lmcore;

TEST(Base64, EncodeEmptyString)
{
    std::string result = Base64::Encode("");
    EXPECT_EQ("", result);
}

TEST(Base64, EncodeSimpleString)
{
    std::string result = Base64::Encode("Hello");
    EXPECT_EQ("SGVsbG8=", result);
}

TEST(Base64, EncodeWithPadding)
{
    EXPECT_EQ("YQ==", Base64::Encode("a"));
    EXPECT_EQ("YWI=", Base64::Encode("ab"));
    EXPECT_EQ("YWJj", Base64::Encode("abc"));
}

TEST(Base64, EncodeLongString)
{
    std::string input = "The quick brown fox jumps over the lazy dog";
    std::string result = Base64::Encode(input);
    EXPECT_EQ("VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==", result);
}

TEST(Base64, EncodeVector)
{
    std::vector<uint8_t> data = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
    std::string result = Base64::Encode(data);
    EXPECT_EQ("SGVsbG8=", result);
}

TEST(Base64, EncodeBinaryData)
{
    uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD};
    std::string result = Base64::Encode(data, sizeof(data));
    EXPECT_EQ("AAECA//+/Q==", result); // 修正期望值
}

TEST(Base64, DecodeEmptyString)
{
    auto result = Base64::Decode("");
    EXPECT_TRUE(result.empty());
}

TEST(Base64, DecodeSimpleString)
{
    auto result = Base64::Decode("SGVsbG8=");
    std::string decoded(result.begin(), result.end());
    EXPECT_EQ("Hello", decoded);
}

TEST(Base64, DecodeWithPadding)
{
    auto result1 = Base64::Decode("YQ==");
    EXPECT_EQ("a", std::string(result1.begin(), result1.end()));

    auto result2 = Base64::Decode("YWI=");
    EXPECT_EQ("ab", std::string(result2.begin(), result2.end()));

    auto result3 = Base64::Decode("YWJj");
    EXPECT_EQ("abc", std::string(result3.begin(), result3.end()));
}

TEST(Base64, DecodeLongString)
{
    std::string encoded = "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==";
    auto result = Base64::Decode(encoded);
    std::string decoded(result.begin(), result.end());
    EXPECT_EQ("The quick brown fox jumps over the lazy dog", decoded);
}

TEST(Base64, DecodeBinaryData)
{
    std::string encoded = "AAECA//+/Q=="; // 修正输入
    auto result = Base64::Decode(encoded);
    EXPECT_EQ(7, result.size());
    EXPECT_EQ(0x00, result[0]);
    EXPECT_EQ(0x01, result[1]);
    EXPECT_EQ(0x02, result[2]);
    EXPECT_EQ(0x03, result[3]);
    EXPECT_EQ(0xFF, result[4]);
    EXPECT_EQ(0xFE, result[5]);
    EXPECT_EQ(0xFD, result[6]);
}

TEST(Base64, EncodeDecodeRoundTrip)
{
    std::string original = "Base64 encode/decode test!@#$%^&*()";
    std::string encoded = Base64::Encode(original);
    auto decoded = Base64::Decode(encoded);
    std::string result(decoded.begin(), decoded.end());
    EXPECT_EQ(original, result);
}

TEST(Base64, EncodeDecode256Bytes)
{
    std::vector<uint8_t> original;
    for (int i = 0; i < 256; i++) {
        original.push_back(static_cast<uint8_t>(i));
    }

    std::string encoded = Base64::Encode(original);
    auto decoded = Base64::Decode(encoded);

    EXPECT_EQ(original.size(), decoded.size());
    for (size_t i = 0; i < original.size(); i++) {
        EXPECT_EQ(original[i], decoded[i]);
    }
}

RUN_ALL_TESTS()
