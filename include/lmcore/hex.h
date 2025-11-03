/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_HEX_H
#define LMSHAO_LMCORE_HEX_H

#include <cstdint>
#include <string>
#include <vector>

namespace lmshao::lmcore {

/**
 * @brief Hexadecimal encoding and decoding utility
 *
 * Converts binary data to/from hexadecimal string representation.
 * Commonly used for:
 * - Debug output (displaying binary data)
 * - Log messages
 * - Network packet inspection
 * - MAC addresses, UUIDs
 * - Checksum/hash display
 * - Configuration files
 *
 * Example usage:
 * @code
 *   // Encode binary to hex
 *   uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
 *   std::string hex = Hex::Encode(data, 4);
 *   // Result: "DEADBEEF" (uppercase by default)
 *
 *   // Encode with lowercase
 *   std::string hex_lower = Hex::Encode(data, 4, false);
 *   // Result: "deadbeef"
 *
 *   // Encode with separator
 *   std::string hex_sep = Hex::Encode(data, 4, true, ':');
 *   // Result: "DE:AD:BE:EF"
 *
 *   // Decode hex string
 *   std::vector<uint8_t> decoded = Hex::Decode("DEADBEEF");
 *   // Result: {0xDE, 0xAD, 0xBE, 0xEF}
 *
 *   // Decode with separators
 *   auto decoded2 = Hex::Decode("DE:AD:BE:EF");
 *   // Result: {0xDE, 0xAD, 0xBE, 0xEF}
 * @endcode
 */
class Hex {
public:
    /**
     * @brief Encode binary data to hexadecimal string
     * @param data Pointer to binary data
     * @param len Length of data in bytes
     * @param uppercase Use uppercase letters (default: true)
     * @param separator Optional separator character (e.g., ':', ' ', '-')
     * @return Hexadecimal string
     */
    static std::string Encode(const uint8_t *data, size_t len, bool uppercase = true, char separator = '\0');

    /**
     * @brief Encode vector of bytes to hexadecimal string
     * @param data Vector of binary data
     * @param uppercase Use uppercase letters (default: true)
     * @param separator Optional separator character
     * @return Hexadecimal string
     */
    static std::string Encode(const std::vector<uint8_t> &data, bool uppercase = true, char separator = '\0');

    /**
     * @brief Encode string to hexadecimal string
     * @param data String to encode
     * @param uppercase Use uppercase letters (default: true)
     * @param separator Optional separator character
     * @return Hexadecimal string
     */
    static std::string Encode(const std::string &data, bool uppercase = true, char separator = '\0');

    /**
     * @brief Decode hexadecimal string to binary data
     * @param hex Hexadecimal string (with or without separators)
     * @return Decoded binary data as vector
     *
     * Automatically handles common separators: space, colon, dash, comma
     * Case-insensitive (accepts both "DEADBEEF" and "deadbeef")
     * Invalid characters are ignored
     */
    static std::vector<uint8_t> Decode(const std::string &hex);

    /**
     * @brief Decode hexadecimal string to binary data (output parameter version)
     * @param hex Hexadecimal string
     * @param output Output vector to store decoded data
     * @return true on success, false on error
     */
    static bool Decode(const std::string &hex, std::vector<uint8_t> &output);

    /**
     * @brief Validate if string is valid hexadecimal
     * @param hex String to validate
     * @param allow_separators Allow separator characters (default: true)
     * @return true if valid hex string
     */
    static bool IsValidHex(const std::string &hex, bool allow_separators = true);
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_HEX_H
