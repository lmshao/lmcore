/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_CRC32_H
#define LMSHAO_LMCORE_CRC32_H

#include <cstdint>
#include <string>
#include <vector>

namespace lmshao::lmcore {

/**
 * @brief CRC32 (Cyclic Redundancy Check) checksum calculator
 *
 * CRC32 is a checksum algorithm used to detect errors in data transmission or storage.
 * It's widely used in:
 * - ZIP/GZIP file format
 * - PNG image format
 * - Ethernet frames
 * - Network protocols
 *
 * Features:
 * - Fast computation using lookup table
 * - Detects common transmission errors
 * - Supports incremental calculation for large files
 *
 * Example usage:
 * @code
 *   // Simple one-shot calculation
 *   uint32_t crc = CRC32::Calculate("123456789");
 *   // Result: 0xCBF43926
 *
 *   // Incremental calculation for large files
 *   std::ifstream file("video.h264", std::ios::binary);
 *   CRC32::Context ctx;
 *   uint8_t buffer[8192];
 *   while (file.read((char*)buffer, sizeof(buffer))) {
 *       ctx.Update(buffer, file.gcount());
 *   }
 *   uint32_t file_crc = ctx.Final();
 * @endcode
 */
class CRC32 {
public:
    /**
     * @brief Calculate CRC32 checksum for binary data
     * @param data Pointer to binary data
     * @param len Length of data in bytes
     * @return CRC32 checksum value
     */
    static uint32_t Calculate(const uint8_t *data, size_t len);

    /**
     * @brief Calculate CRC32 checksum for vector of bytes
     * @param data Vector of binary data
     * @return CRC32 checksum value
     */
    static uint32_t Calculate(const std::vector<uint8_t> &data);

    /**
     * @brief Calculate CRC32 checksum for string
     * @param data String to calculate checksum for
     * @return CRC32 checksum value
     */
    static uint32_t Calculate(const std::string &data);

    /**
     * @brief Context for incremental CRC32 calculation
     *
     * Use Context when processing data in chunks (e.g., streaming, large files).
     * The context maintains internal state and automatically handles the CRC32
     * algorithm details (initialization and finalization).
     *
     * Example:
     * @code
     *   CRC32::Context ctx;
     *   ctx.Update(chunk1, len1);
     *   ctx.Update(chunk2, len2);
     *   uint32_t crc = ctx.Final();
     * @endcode
     */
    class Context {
    public:
        /**
         * @brief Constructor - initializes CRC32 context
         */
        Context() : crc_(0xFFFFFFFF) {}

        /**
         * @brief Update CRC32 with binary data
         * @param data Pointer to data chunk
         * @param len Length of data chunk
         */
        void Update(const uint8_t *data, size_t len);

        /**
         * @brief Update CRC32 with vector of bytes
         * @param data Vector of binary data
         */
        void Update(const std::vector<uint8_t> &data);

        /**
         * @brief Update CRC32 with string
         * @param data String data
         */
        void Update(const std::string &data);

        /**
         * @brief Finalize and get CRC32 checksum
         * @return Final CRC32 checksum value
         * @note Can be called multiple times without changing state
         */
        uint32_t Final();

        /**
         * @brief Reset context to initial state
         *
         * After reset, the context can be reused for a new calculation
         */
        void Reset() { crc_ = 0xFFFFFFFF; }

    private:
        uint32_t crc_;
    };

private:
    static void InitTable();
    static uint32_t UpdateInternal(uint32_t crc, const uint8_t *data, size_t len);
    static uint32_t table_[256];
    static bool table_initialized_;
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_CRC32_H
