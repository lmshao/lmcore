/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_BASE64_H
#define LMSHAO_LMCORE_BASE64_H

#include <cstdint>
#include <string>
#include <vector>

namespace lmshao::lmcore {

/**
 * @brief Base64 encoding and decoding utility
 *
 * Base64 is a binary-to-text encoding scheme that represents binary data in ASCII
 * string format. It's commonly used for:
 * - HTTP Basic Authentication
 * - Embedding binary data in JSON/XML
 * - Email attachments (MIME)
 * - Data URLs
 *
 * Example usage:
 * @code
 *   // Encode
 *   std::string encoded = Base64::Encode("Hello World");
 *   // Result: "SGVsbG8gV29ybGQ="
 *
 *   // Decode
 *   std::vector<uint8_t> decoded = Base64::Decode("SGVsbG8gV29ybGQ=");
 *   std::string text(decoded.begin(), decoded.end());
 *   // Result: "Hello World"
 *
 *   // Binary data
 *   uint8_t binary[] = {0x00, 0xFF, 0xAA, 0x55};
 *   std::string encoded_bin = Base64::Encode(binary, sizeof(binary));
 * @endcode
 */
class Base64 {
public:
    /**
     * @brief Encode binary data to Base64 string
     * @param data Pointer to binary data
     * @param len Length of data in bytes
     * @return Base64 encoded string
     */
    static std::string Encode(const uint8_t *data, size_t len);

    /**
     * @brief Encode vector of bytes to Base64 string
     * @param data Vector of binary data
     * @return Base64 encoded string
     */
    static std::string Encode(const std::vector<uint8_t> &data);

    /**
     * @brief Encode string to Base64 string
     * @param data String to encode
     * @return Base64 encoded string
     */
    static std::string Encode(const std::string &data);

    /**
     * @brief Decode Base64 string to binary data
     * @param encoded Base64 encoded string
     * @return Decoded binary data as vector
     */
    static std::vector<uint8_t> Decode(const std::string &encoded);

    /**
     * @brief Decode Base64 string to binary data (output parameter version)
     * @param encoded Base64 encoded string
     * @param output Output vector to store decoded data
     * @return true on success, false on error
     */
    static bool Decode(const std::string &encoded, std::vector<uint8_t> &output);
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_BASE64_H
