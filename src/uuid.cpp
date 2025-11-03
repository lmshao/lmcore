/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/uuid.h>

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <random>
#include <sstream>

namespace lmshao::lmcore {

static std::random_device rd;
static std::mt19937_64 gen(rd());
static std::uniform_int_distribution<uint64_t> dis;

std::string UUID::Generate(bool with_dashes)
{
    // Generate 128 random bits
    uint64_t high = dis(gen);
    uint64_t low = dis(gen);

    // Set version to 4 (random UUID)
    // Version is in the most significant 4 bits of the 7th byte
    high = (high & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;

    // Set variant to RFC 4122
    // Variant is in the most significant 2 bits of the 9th byte
    low = (low & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

    // Convert to hex string
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    if (with_dashes) {
        // Format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
        oss << std::setw(8) << ((high >> 32) & 0xFFFFFFFF) << "-";
        oss << std::setw(4) << ((high >> 16) & 0xFFFF) << "-";
        oss << std::setw(4) << (high & 0xFFFF) << "-";
        oss << std::setw(4) << ((low >> 48) & 0xFFFF) << "-";
        oss << std::setw(12) << (low & 0xFFFFFFFFFFFFULL);
    } else {
        // Format: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        oss << std::setw(16) << high;
        oss << std::setw(16) << low;
    }

    return oss.str();
}

std::string UUID::GenerateShort()
{
    uint32_t random = static_cast<uint32_t>(dis(gen));

    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << random;

    return oss.str();
}

bool UUID::Validate(const std::string &uuid)
{
    if (uuid.empty()) {
        return false;
    }

    // Check length
    if (uuid.length() == 36) {
        // Format with dashes: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        if (uuid[8] != '-' || uuid[13] != '-' || uuid[18] != '-' || uuid[23] != '-') {
            return false;
        }

        // Check hex digits
        for (size_t i = 0; i < uuid.length(); ++i) {
            if (i == 8 || i == 13 || i == 18 || i == 23) {
                continue; // Skip dashes
            }
            if (!std::isxdigit(static_cast<unsigned char>(uuid[i]))) {
                return false;
            }
        }
        return true;
    } else if (uuid.length() == 32) {
        // Format without dashes: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        for (char c : uuid) {
            if (!std::isxdigit(static_cast<unsigned char>(c))) {
                return false;
            }
        }
        return true;
    }

    return false;
}

std::string UUID::ToUpper(const std::string &uuid)
{
    std::string result = uuid;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string UUID::ToLower(const std::string &uuid)
{
    std::string result = uuid;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string UUID::AddDashes(const std::string &uuid)
{
    if (uuid.length() != 32) {
        return uuid; // Invalid length
    }

    // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -> xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    std::string result;
    result.reserve(36);

    result += uuid.substr(0, 8);
    result += '-';
    result += uuid.substr(8, 4);
    result += '-';
    result += uuid.substr(12, 4);
    result += '-';
    result += uuid.substr(16, 4);
    result += '-';
    result += uuid.substr(20, 12);

    return result;
}

std::string UUID::RemoveDashes(const std::string &uuid)
{
    std::string result;
    result.reserve(32);

    for (char c : uuid) {
        if (c != '-') {
            result += c;
        }
    }

    return result;
}

} // namespace lmshao::lmcore
