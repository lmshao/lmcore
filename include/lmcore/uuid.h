/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_UUID_H
#define LMSHAO_LMCORE_UUID_H

#include <string>

namespace lmshao::lmcore {

/**
 * @brief UUID (Universally Unique Identifier) generator
 *
 * Generates random UUIDs (Version 4) according to RFC 4122.
 * Commonly used for:
 * - RTSP Session IDs
 * - Unique identifiers
 * - Tracking/correlation IDs
 * - Database primary keys
 *
 * Example usage:
 * @code
 *   // Generate standard UUID v4
 *   std::string uuid = UUID::Generate();
 *   // Result: "550e8400-e29b-41d4-a716-446655440000"
 *
 *   // Generate short ID (8 characters)
 *   std::string short_id = UUID::GenerateShort();
 *   // Result: "a3f4b2c1"
 *
 *   // Generate without dashes
 *   std::string compact = UUID::Generate(false);
 *   // Result: "550e8400e29b41d4a716446655440000"
 *
 *   // Validate UUID
 *   bool valid = UUID::Validate("550e8400-e29b-41d4-a716-446655440000");
 *   // Result: true
 * @endcode
 */
class UUID {
public:
    /**
     * @brief Generate UUID v4 (random)
     * @param with_dashes Include dashes in format (default: true)
     * @return UUID string in format "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx" or
     *         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" if with_dashes is false
     *
     * Format: 8-4-4-4-12 hexadecimal digits
     * Version: 4 (random)
     * Variant: RFC 4122
     */
    static std::string Generate(bool with_dashes = true);

    /**
     * @brief Generate short unique ID (8 hex characters)
     * @return 8-character hexadecimal string
     *
     * Not a true UUID, but useful for shorter identifiers
     * Collision probability is much higher than full UUID
     */
    static std::string GenerateShort();

    /**
     * @brief Validate UUID format
     * @param uuid UUID string to validate
     * @return true if valid UUID format
     *
     * Accepts both formats:
     * - With dashes: "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
     * - Without dashes: "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
     */
    static bool Validate(const std::string &uuid);

    /**
     * @brief Convert UUID to uppercase
     * @param uuid UUID string
     * @return Uppercase UUID string
     */
    static std::string ToUpper(const std::string &uuid);

    /**
     * @brief Convert UUID to lowercase
     * @param uuid UUID string
     * @return Lowercase UUID string
     */
    static std::string ToLower(const std::string &uuid);

    /**
     * @brief Add dashes to UUID without dashes
     * @param uuid UUID string without dashes (32 hex chars)
     * @return UUID with dashes in standard format
     */
    static std::string AddDashes(const std::string &uuid);

    /**
     * @brief Remove dashes from UUID
     * @param uuid UUID string with dashes
     * @return UUID without dashes (32 hex chars)
     */
    static std::string RemoveDashes(const std::string &uuid);
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_UUID_H
