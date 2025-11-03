/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/uuid.h>

#include <set>

#include "../test_framework.h"

using namespace lmshao::lmcore;

// Generate UUID with dashes
TEST(UUID, GenerateWithDashes)
{
    std::string uuid = UUID::Generate();
    EXPECT_EQ(36, uuid.length());
    EXPECT_EQ('-', uuid[8]);
    EXPECT_EQ('-', uuid[13]);
    EXPECT_EQ('-', uuid[18]);
    EXPECT_EQ('-', uuid[23]);
    EXPECT_TRUE(UUID::Validate(uuid));
}

// Generate UUID without dashes
TEST(UUID, GenerateWithoutDashes)
{
    std::string uuid = UUID::Generate(false);
    EXPECT_EQ(32, uuid.length());
    EXPECT_TRUE(UUID::Validate(uuid));
}

// Generate short ID
TEST(UUID, GenerateShort)
{
    std::string short_id = UUID::GenerateShort();
    EXPECT_EQ(8, short_id.length());

    // Should be all hex digits
    for (char c : short_id) {
        EXPECT_TRUE(std::isxdigit(static_cast<unsigned char>(c)));
    }
}

// UUID uniqueness
TEST(UUID, Uniqueness)
{
    std::set<std::string> uuids;
    const int count = 1000;

    for (int i = 0; i < count; ++i) {
        uuids.insert(UUID::Generate());
    }

    // All should be unique
    EXPECT_EQ(count, uuids.size());
}

// Short ID uniqueness (may have collisions, but test reasonable uniqueness)
TEST(UUID, ShortIDUniqueness)
{
    std::set<std::string> ids;
    const int count = 100;

    for (int i = 0; i < count; ++i) {
        ids.insert(UUID::GenerateShort());
    }

    // Should have most IDs unique (allow a few collisions)
    EXPECT_GE(ids.size(), count - 5);
}

// Validate valid UUID with dashes
TEST(UUID, ValidateWithDashes)
{
    EXPECT_TRUE(UUID::Validate("550e8400-e29b-41d4-a716-446655440000"));
    EXPECT_TRUE(UUID::Validate("00000000-0000-0000-0000-000000000000"));
    EXPECT_TRUE(UUID::Validate("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"));
}

// Validate valid UUID without dashes
TEST(UUID, ValidateWithoutDashes)
{
    EXPECT_TRUE(UUID::Validate("550e8400e29b41d4a716446655440000"));
    EXPECT_TRUE(UUID::Validate("00000000000000000000000000000000"));
    EXPECT_TRUE(UUID::Validate("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"));
}

// Validate invalid UUIDs
TEST(UUID, ValidateInvalid)
{
    EXPECT_FALSE(UUID::Validate(""));                                      // Empty
    EXPECT_FALSE(UUID::Validate("invalid"));                               // Not hex
    EXPECT_FALSE(UUID::Validate("550e8400-e29b-41d4-a716"));               // Too short
    EXPECT_FALSE(UUID::Validate("550e8400e29b41d4a716"));                  // Too short
    EXPECT_FALSE(UUID::Validate("550e8400-e29b-41d4-a716-4466554400000")); // Too long
    EXPECT_FALSE(UUID::Validate("550e8400-e29b-41d4-a716-44665544000G"));  // Invalid char
    EXPECT_FALSE(UUID::Validate("550e8400e29b41d4a716446655440000extra")); // Too long
}

// Validate wrong dash positions
TEST(UUID, ValidateWrongDashes)
{
    EXPECT_FALSE(UUID::Validate("550e8400e-29b-41d4-a716-446655440000")); // Wrong position
    EXPECT_FALSE(UUID::Validate("550e8400-e29b41d4-a716-446655440000"));  // Missing dash
}

// To uppercase
TEST(UUID, ToUpper)
{
    std::string uuid = "550e8400-e29b-41d4-a716-446655440000";
    std::string upper = UUID::ToUpper(uuid);
    EXPECT_EQ("550E8400-E29B-41D4-A716-446655440000", upper);
}

// To lowercase
TEST(UUID, ToLower)
{
    std::string uuid = "550E8400-E29B-41D4-A716-446655440000";
    std::string lower = UUID::ToLower(uuid);
    EXPECT_EQ("550e8400-e29b-41d4-a716-446655440000", lower);
}

// Add dashes
TEST(UUID, AddDashes)
{
    std::string without = "550e8400e29b41d4a716446655440000";
    std::string with = UUID::AddDashes(without);
    EXPECT_EQ("550e8400-e29b-41d4-a716-446655440000", with);
    EXPECT_TRUE(UUID::Validate(with));
}

// Add dashes to invalid input
TEST(UUID, AddDashesInvalid)
{
    EXPECT_EQ("short", UUID::AddDashes("short")); // Too short, unchanged
}

// Remove dashes
TEST(UUID, RemoveDashes)
{
    std::string with = "550e8400-e29b-41d4-a716-446655440000";
    std::string without = UUID::RemoveDashes(with);
    EXPECT_EQ("550e8400e29b41d4a716446655440000", without);
    EXPECT_TRUE(UUID::Validate(without));
}

// Remove dashes from string without dashes
TEST(UUID, RemoveDashesNoDashes)
{
    std::string without = "550e8400e29b41d4a716446655440000";
    std::string result = UUID::RemoveDashes(without);
    EXPECT_EQ(without, result);
}

// Round trip: add and remove dashes
TEST(UUID, DashesRoundTrip)
{
    std::string original = "550e8400e29b41d4a716446655440000";
    std::string with_dashes = UUID::AddDashes(original);
    std::string back = UUID::RemoveDashes(with_dashes);
    EXPECT_EQ(original, back);
}

// Case conversion round trip
TEST(UUID, CaseRoundTrip)
{
    std::string original = "550e8400-e29b-41d4-a716-446655440000";
    std::string upper = UUID::ToUpper(original);
    std::string lower = UUID::ToLower(upper);
    EXPECT_EQ(original, lower);
}

// Generated UUID has correct version
TEST(UUID, CheckVersion4)
{
    std::string uuid = UUID::Generate(false);

    // Version 4 UUID has '4' at position 12 (13th character)
    // In the format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    // Without dashes: xxxxxxxxxxxx4xxxxxxxxxxxxxxx (position 12)
    EXPECT_EQ('4', uuid[12]);
}

// Generated UUID has correct variant
TEST(UUID, CheckVariant)
{
    std::string uuid = UUID::Generate(false);

    // RFC 4122 variant has bits 10xx in the high 2 bits of byte 8
    // In hex, this means the 17th character should be 8, 9, a, or b
    char variant_char = uuid[16];
    EXPECT_TRUE(variant_char == '8' || variant_char == '9' || variant_char == 'a' || variant_char == 'b');
}

// Real-world usage: RTSP Session ID
TEST(UUID, RTSPSessionID)
{
    // Generate a UUID for RTSP Session ID (uppercase, no dashes is common)
    std::string session_id = UUID::ToUpper(UUID::Generate(false));

    EXPECT_EQ(32, session_id.length());
    EXPECT_TRUE(UUID::Validate(session_id));

    // All uppercase
    for (char c : session_id) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            EXPECT_TRUE(std::isupper(static_cast<unsigned char>(c)));
        }
    }
}

RUN_ALL_TESTS()
