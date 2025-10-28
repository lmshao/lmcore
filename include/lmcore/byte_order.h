/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_BYTE_ORDER_H
#define LMSHAO_LMCORE_BYTE_ORDER_H

#include <cstdint>

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define LMCORE_LITTLE_ENDIAN 1
#elif defined(_MSC_VER) || defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64) ||          \
    defined(__amd64__) || defined(__aarch64__) || defined(_M_ARM64)
#define LMCORE_LITTLE_ENDIAN 1
#else
#define LMCORE_LITTLE_ENDIAN 0
#endif

namespace lmshao::lmcore {

/**
 * @brief Byte order conversion utilities
 */
class ByteOrder {
public:
    /**
     * @brief Check if system is little endian
     * @return true if little endian, false if big endian
     */
    static constexpr bool IsSystemLittleEndian() { return LMCORE_LITTLE_ENDIAN != 0; }

    /**
     * @brief Convert 16-bit value from host to network byte order
     * @param value Host byte order value
     * @return Network byte order value
     */
    static uint16_t HostToNetwork16(uint16_t value)
    {
#if LMCORE_LITTLE_ENDIAN
        return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
#else
        return value;
#endif
    }

    /**
     * @brief Convert 32-bit value from host to network byte order
     * @param value Host byte order value
     * @return Network byte order value
     */
    static uint32_t HostToNetwork32(uint32_t value)
    {
#if LMCORE_LITTLE_ENDIAN
        return ((value & 0xFF) << 24) | (((value >> 8) & 0xFF) << 16) | (((value >> 16) & 0xFF) << 8) |
               ((value >> 24) & 0xFF);
#else
        return value;
#endif
    }

    /**
     * @brief Convert 64-bit value from host to network byte order
     * @param value Host byte order value
     * @return Network byte order value
     */
    static uint64_t HostToNetwork64(uint64_t value)
    {
#if LMCORE_LITTLE_ENDIAN
        return ((value & 0xFFULL) << 56) | (((value >> 8) & 0xFFULL) << 48) | (((value >> 16) & 0xFFULL) << 40) |
               (((value >> 24) & 0xFFULL) << 32) | (((value >> 32) & 0xFFULL) << 24) |
               (((value >> 40) & 0xFFULL) << 16) | (((value >> 48) & 0xFFULL) << 8) | ((value >> 56) & 0xFFULL);
#else
        return value;
#endif
    }

    /**
     * @brief Convert 16-bit value from network to host byte order
     * @param value Network byte order value
     * @return Host byte order value
     */
    static uint16_t NetworkToHost16(uint16_t value)
    {
#if LMCORE_LITTLE_ENDIAN
        return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
#else
        return value;
#endif
    }

    /**
     * @brief Convert 32-bit value from network to host byte order
     * @param value Network byte order value
     * @return Host byte order value
     */
    static uint32_t NetworkToHost32(uint32_t value)
    {
#if LMCORE_LITTLE_ENDIAN
        return ((value & 0xFF) << 24) | (((value >> 8) & 0xFF) << 16) | (((value >> 16) & 0xFF) << 8) |
               ((value >> 24) & 0xFF);
#else
        return value;
#endif
    }

    /**
     * @brief Convert 64-bit value from network to host byte order
     * @param value Network byte order value
     * @return Host byte order value
     */
    static uint64_t NetworkToHost64(uint64_t value)
    {
#if LMCORE_LITTLE_ENDIAN
        return ((value & 0xFFULL) << 56) | (((value >> 8) & 0xFFULL) << 48) | (((value >> 16) & 0xFFULL) << 40) |
               (((value >> 24) & 0xFFULL) << 32) | (((value >> 32) & 0xFFULL) << 24) |
               (((value >> 40) & 0xFFULL) << 16) | (((value >> 48) & 0xFFULL) << 8) | ((value >> 56) & 0xFFULL);
#else
        return value;
#endif
    }

    /**
     * @brief Read 16-bit big-endian value from buffer
     * @param buffer Buffer to read from
     * @return Host byte order value
     */
    static uint16_t ReadBE16(const uint8_t *buffer) { return (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1]; }

    /**
     * @brief Read 32-bit big-endian value from buffer
     * @param buffer Buffer to read from
     * @return Host byte order value
     */
    static uint32_t ReadBE32(const uint8_t *buffer)
    {
        return (static_cast<uint32_t>(buffer[0]) << 24) | (static_cast<uint32_t>(buffer[1]) << 16) |
               (static_cast<uint32_t>(buffer[2]) << 8) | buffer[3];
    }

    /**
     * @brief Read 64-bit big-endian value from buffer
     * @param buffer Buffer to read from
     * @return Host byte order value
     */
    static uint64_t ReadBE64(const uint8_t *buffer)
    {
        return (static_cast<uint64_t>(buffer[0]) << 56) | (static_cast<uint64_t>(buffer[1]) << 48) |
               (static_cast<uint64_t>(buffer[2]) << 40) | (static_cast<uint64_t>(buffer[3]) << 32) |
               (static_cast<uint64_t>(buffer[4]) << 24) | (static_cast<uint64_t>(buffer[5]) << 16) |
               (static_cast<uint64_t>(buffer[6]) << 8) | buffer[7];
    }

    /**
     * @brief Write 16-bit value to buffer in big-endian format
     * @param buffer Buffer to write to
     * @param value Host byte order value
     */
    static void WriteBE16(uint8_t *buffer, uint16_t value)
    {
        buffer[0] = (value >> 8) & 0xFF;
        buffer[1] = value & 0xFF;
    }

    /**
     * @brief Write 32-bit value to buffer in big-endian format
     * @param buffer Buffer to write to
     * @param value Host byte order value
     */
    static void WriteBE32(uint8_t *buffer, uint32_t value)
    {
        buffer[0] = (value >> 24) & 0xFF;
        buffer[1] = (value >> 16) & 0xFF;
        buffer[2] = (value >> 8) & 0xFF;
        buffer[3] = value & 0xFF;
    }

    /**
     * @brief Write 64-bit value to buffer in big-endian format
     * @param buffer Buffer to write to
     * @param value Host byte order value
     */
    static void WriteBE64(uint8_t *buffer, uint64_t value)
    {
        buffer[0] = (value >> 56) & 0xFF;
        buffer[1] = (value >> 48) & 0xFF;
        buffer[2] = (value >> 40) & 0xFF;
        buffer[3] = (value >> 32) & 0xFF;
        buffer[4] = (value >> 24) & 0xFF;
        buffer[5] = (value >> 16) & 0xFF;
        buffer[6] = (value >> 8) & 0xFF;
        buffer[7] = value & 0xFF;
    }

    /**
     * @brief Read 24-bit big-endian value from buffer
     * @param buffer Buffer to read from (3 bytes)
     * @return Host byte order value (32-bit with upper byte zero)
     */
    static uint32_t ReadBE24(const uint8_t *buffer)
    {
        return (static_cast<uint32_t>(buffer[0]) << 16) | (static_cast<uint32_t>(buffer[1]) << 8) | buffer[2];
    }

    /**
     * @brief Write 24-bit value to buffer in big-endian format
     * @param buffer Buffer to write to (3 bytes)
     * @param value Host byte order value (only lower 24 bits used)
     */
    static void WriteBE24(uint8_t *buffer, uint32_t value)
    {
        buffer[0] = (value >> 16) & 0xFF;
        buffer[1] = (value >> 8) & 0xFF;
        buffer[2] = value & 0xFF;
    }

    /**
     * @brief Swap bytes of 16-bit value
     * @param value Value to swap
     * @return Byte-swapped value
     */
    static uint16_t Swap16(uint16_t value) { return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF); }

    /**
     * @brief Swap bytes of 32-bit value
     * @param value Value to swap
     * @return Byte-swapped value
     */
    static uint32_t Swap32(uint32_t value)
    {
        return ((value & 0xFF) << 24) | (((value >> 8) & 0xFF) << 16) | (((value >> 16) & 0xFF) << 8) |
               ((value >> 24) & 0xFF);
    }

    /**
     * @brief Swap bytes of 64-bit value
     * @param value Value to swap
     * @return Byte-swapped value
     */
    static uint64_t Swap64(uint64_t value)
    {
        return ((value & 0xFFULL) << 56) | (((value >> 8) & 0xFFULL) << 48) | (((value >> 16) & 0xFFULL) << 40) |
               (((value >> 24) & 0xFFULL) << 32) | (((value >> 32) & 0xFFULL) << 24) |
               (((value >> 40) & 0xFFULL) << 16) | (((value >> 48) & 0xFFULL) << 8) | ((value >> 56) & 0xFFULL);
    }
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_BYTE_ORDER_H