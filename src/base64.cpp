/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include "lmcore/base64.h"

namespace lmshao::lmcore {

static const char kBase64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool IsBase64Char(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string Base64::Encode(const uint8_t *data, size_t len)
{
    std::string result;
    result.reserve((len + 2) / 3 * 4);

    size_t i = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];

    while (len--) {
        char_array_3[i++] = *(data++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++) {
                result += kBase64Chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (size_t j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (size_t j = 0; j < i + 1; j++) {
            result += kBase64Chars[char_array_4[j]];
        }

        while (i++ < 3) {
            result += '=';
        }
    }

    return result;
}

std::string Base64::Encode(const std::vector<uint8_t> &data)
{
    return Encode(data.data(), data.size());
}

std::string Base64::Encode(const std::string &data)
{
    return Encode(reinterpret_cast<const uint8_t *>(data.c_str()), data.length());
}

std::vector<uint8_t> Base64::Decode(const std::string &encoded)
{
    std::vector<uint8_t> result;
    Decode(encoded, result);
    return result;
}

bool Base64::Decode(const std::string &encoded, std::vector<uint8_t> &output)
{
    size_t len = encoded.size();
    size_t i = 0;
    size_t in_ = 0;
    uint8_t char_array_4[4], char_array_3[3];

    output.clear();
    output.reserve(len / 4 * 3);

    while (len-- && (encoded[in_] != '=') && IsBase64Char(encoded[in_])) {
        char_array_4[i++] = encoded[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                char_array_4[i] = static_cast<uint8_t>(std::string(kBase64Chars).find(char_array_4[i]));
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; i < 3; i++) {
                output.push_back(char_array_3[i]);
            }
            i = 0;
        }
    }

    if (i) {
        for (size_t j = 0; j < i; j++) {
            char_array_4[j] = static_cast<uint8_t>(std::string(kBase64Chars).find(char_array_4[j]));
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        for (size_t j = 0; j < i - 1; j++) {
            output.push_back(char_array_3[j]);
        }
    }

    return true;
}

} // namespace lmshao::lmcore
