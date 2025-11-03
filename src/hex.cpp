/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/hex.h>

#include <cctype>
#include <sstream>

namespace lmshao::lmcore {

static const char HEX_CHARS_UPPER[] = "0123456789ABCDEF";
static const char HEX_CHARS_LOWER[] = "0123456789abcdef";

std::string Hex::Encode(const uint8_t *data, size_t len, bool uppercase, char separator)
{
    if (!data || len == 0) {
        return "";
    }

    const char *hex_chars = uppercase ? HEX_CHARS_UPPER : HEX_CHARS_LOWER;
    std::string result;

    // Reserve space: 2 chars per byte + separators
    size_t reserve_size = len * 2;
    if (separator != '\0' && len > 1) {
        reserve_size += (len - 1); // Add space for separators
    }
    result.reserve(reserve_size);

    for (size_t i = 0; i < len; ++i) {
        if (i > 0 && separator != '\0') {
            result += separator;
        }
        uint8_t byte = data[i];
        result += hex_chars[(byte >> 4) & 0x0F]; // High nibble
        result += hex_chars[byte & 0x0F];        // Low nibble
    }

    return result;
}

std::string Hex::Encode(const std::vector<uint8_t> &data, bool uppercase, char separator)
{
    return Encode(data.data(), data.size(), uppercase, separator);
}

std::string Hex::Encode(const std::string &data, bool uppercase, char separator)
{
    return Encode(reinterpret_cast<const uint8_t *>(data.data()), data.size(), uppercase, separator);
}

std::vector<uint8_t> Hex::Decode(const std::string &hex)
{
    std::vector<uint8_t> result;
    Decode(hex, result);
    return result;
}

bool Hex::Decode(const std::string &hex, std::vector<uint8_t> &output)
{
    output.clear();

    if (hex.empty()) {
        return true;
    }

    // Extract only hex digits
    std::string clean_hex;
    clean_hex.reserve(hex.size());

    for (char c : hex) {
        if (std::isxdigit(static_cast<unsigned char>(c))) {
            clean_hex += c;
        }
        // Ignore separators and whitespace
    }

    // Must have even number of hex digits
    if (clean_hex.size() % 2 != 0) {
        return false;
    }

    output.reserve(clean_hex.size() / 2);

    for (size_t i = 0; i < clean_hex.size(); i += 2) {
        char high = clean_hex[i];
        char low = clean_hex[i + 1];

        // Convert hex chars to nibbles
        auto char_to_nibble = [](char c) -> int {
            if (c >= '0' && c <= '9')
                return c - '0';
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            return -1;
        };

        int high_nibble = char_to_nibble(high);
        int low_nibble = char_to_nibble(low);

        if (high_nibble < 0 || low_nibble < 0) {
            output.clear();
            return false;
        }

        uint8_t byte = static_cast<uint8_t>((high_nibble << 4) | low_nibble);
        output.push_back(byte);
    }

    return true;
}

bool Hex::IsValidHex(const std::string &hex, bool allow_separators)
{
    if (hex.empty()) {
        return false;
    }

    int hex_digit_count = 0;

    for (char c : hex) {
        if (std::isxdigit(static_cast<unsigned char>(c))) {
            hex_digit_count++;
        } else if (allow_separators && (c == ' ' || c == ':' || c == '-' || c == ',' || c == '\t')) {
            // Common separators are allowed
            continue;
        } else {
            // Invalid character
            return false;
        }
    }

    // Must have even number of hex digits to form complete bytes
    return hex_digit_count > 0 && hex_digit_count % 2 == 0;
}

} // namespace lmshao::lmcore
