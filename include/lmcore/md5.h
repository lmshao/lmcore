/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_MD5_H
#define LMSHAO_LMCORE_MD5_H

#include <cstdint>
#include <string>
#include <vector>

namespace lmshao::lmcore {

/**
 * @brief MD5 (Message-Digest Algorithm 5) hash calculator
 *
 * MD5 is a widely used cryptographic hash function that produces a 128-bit (16-byte)
 * hash value, typically expressed as a 32-character hexadecimal string.
 *
 * Common use cases:
 * - File integrity verification
 * - Data fingerprinting
 * - Non-cryptographic checksums
 * - Cache keys
 *
 * Security note:
 * MD5 is NOT cryptographically secure and should NOT be used for:
 * - Password hashing (use bcrypt/Argon2 instead)
 * - Digital signatures
 * - SSL certificates
 * For security-critical applications, use SHA-256 or stronger algorithms.
 *
 * Example usage:
 * @code
 *   // Calculate MD5 for string
 *   std::string hash = MD5::Calculate("Hello World");
 *   // Result: "b10a8db164e0754105b7a99be72e3fe5"
 *
 *   // Calculate MD5 for binary data
 *   uint8_t data[] = {0x01, 0x02, 0x03};
 *   std::string hash = MD5::Calculate(data, sizeof(data));
 *
 *   // Calculate MD5 for file
 *   std::string file_hash = MD5::CalculateFile("video.h264");
 *   if (file_hash.empty()) {
 *       // File not found or read error
 *   }
 * @endcode
 */
class MD5 {
public:
    /**
     * @brief Calculate MD5 hash for binary data
     * @param data Pointer to binary data
     * @param len Length of data in bytes
     * @return MD5 hash as 32-character hexadecimal string (lowercase)
     */
    static std::string Calculate(const uint8_t *data, size_t len);

    /**
     * @brief Calculate MD5 hash for vector of bytes
     * @param data Vector of binary data
     * @return MD5 hash as 32-character hexadecimal string (lowercase)
     */
    static std::string Calculate(const std::vector<uint8_t> &data);

    /**
     * @brief Calculate MD5 hash for string
     * @param data String to hash
     * @return MD5 hash as 32-character hexadecimal string (lowercase)
     */
    static std::string Calculate(const std::string &data);

    /**
     * @brief Calculate MD5 hash for file
     * @param path Path to file
     * @return MD5 hash as 32-character hexadecimal string, or empty string on error
     *
     * This method reads the file in chunks (8KB buffer) to handle large files
     * efficiently without loading the entire file into memory.
     *
     * Example:
     * @code
     *   std::string hash = MD5::CalculateFile("large_video.mp4");
     *   if (!hash.empty()) {
     *       std::cout << "MD5: " << hash << std::endl;
     *   }
     * @endcode
     */
    static std::string CalculateFile(const std::string &path);

private:
    struct Context {
        uint32_t state[4];
        uint32_t count[2];
        uint8_t buffer[64];
    };

    static void Init(Context &ctx);
    static void Update(Context &ctx, const uint8_t *data, size_t len);
    static void Final(Context &ctx, uint8_t digest[16]);
    static void Transform(uint32_t state[4], const uint8_t block[64]);
    static std::string DigestToHex(const uint8_t digest[16]);
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_MD5_H
