/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/string_utils.h>

#include "../test_framework.h"

using namespace lmshao::lmcore;

// Split by char delimiter
TEST(StringUtils, SplitByChar)
{
    auto parts = StringUtils::Split("a,b,c", ',');
    EXPECT_EQ(3, parts.size());
    EXPECT_EQ("a", parts[0]);
    EXPECT_EQ("b", parts[1]);
    EXPECT_EQ("c", parts[2]);
}

// Split by string delimiter
TEST(StringUtils, SplitByString)
{
    auto parts = StringUtils::Split("foo::bar::baz", "::");
    EXPECT_EQ(3, parts.size());
    EXPECT_EQ("foo", parts[0]);
    EXPECT_EQ("bar", parts[1]);
    EXPECT_EQ("baz", parts[2]);
}

// Split with empty parts
TEST(StringUtils, SplitWithEmpty)
{
    auto parts = StringUtils::Split("a,,c", ',');
    EXPECT_EQ(3, parts.size());
    EXPECT_EQ("a", parts[0]);
    EXPECT_EQ("", parts[1]);
    EXPECT_EQ("c", parts[2]);
}

// Split skip empty
TEST(StringUtils, SplitSkipEmpty)
{
    auto parts = StringUtils::Split("a,,c", ',', true);
    EXPECT_EQ(2, parts.size());
    EXPECT_EQ("a", parts[0]);
    EXPECT_EQ("c", parts[1]);
}

// Split empty string
TEST(StringUtils, SplitEmptyString)
{
    auto parts = StringUtils::Split("", ',');
    // Splitting an empty string should produce no elements
    // getline will not extract any content from an empty input
    EXPECT_EQ(0, parts.size());
}

// Join strings
TEST(StringUtils, Join)
{
    std::vector<std::string> parts = {"foo", "bar", "baz"};
    std::string result = StringUtils::Join(parts, "-");
    EXPECT_EQ("foo-bar-baz", result);
}

// Join with empty separator
TEST(StringUtils, JoinEmptySeparator)
{
    std::vector<std::string> parts = {"a", "b", "c"};
    std::string result = StringUtils::Join(parts, "");
    EXPECT_EQ("abc", result);
}

// Join single element
TEST(StringUtils, JoinSingle)
{
    std::vector<std::string> parts = {"alone"};
    std::string result = StringUtils::Join(parts, ",");
    EXPECT_EQ("alone", result);
}

// Join empty vector
TEST(StringUtils, JoinEmpty)
{
    std::vector<std::string> parts;
    std::string result = StringUtils::Join(parts, ",");
    EXPECT_EQ("", result);
}

// Trim whitespace
TEST(StringUtils, Trim)
{
    EXPECT_EQ("hello", StringUtils::Trim("  hello  "));
    EXPECT_EQ("hello", StringUtils::Trim("hello  "));
    EXPECT_EQ("hello", StringUtils::Trim("  hello"));
    EXPECT_EQ("hello", StringUtils::Trim("hello"));
}

// Trim left
TEST(StringUtils, TrimLeft)
{
    EXPECT_EQ("hello  ", StringUtils::TrimLeft("  hello  "));
    EXPECT_EQ("hello", StringUtils::TrimLeft("  hello"));
}

// Trim right
TEST(StringUtils, TrimRight)
{
    EXPECT_EQ("  hello", StringUtils::TrimRight("  hello  "));
    EXPECT_EQ("hello", StringUtils::TrimRight("hello  "));
}

// Trim with tabs and newlines
TEST(StringUtils, TrimMixedWhitespace)
{
    EXPECT_EQ("hello", StringUtils::Trim("\t\n hello \r\n"));
}

// To lowercase
TEST(StringUtils, ToLower)
{
    EXPECT_EQ("hello", StringUtils::ToLower("HELLO"));
    EXPECT_EQ("hello", StringUtils::ToLower("HeLLo"));
    EXPECT_EQ("hello123", StringUtils::ToLower("HELLO123"));
}

// To uppercase
TEST(StringUtils, ToUpper)
{
    EXPECT_EQ("HELLO", StringUtils::ToUpper("hello"));
    EXPECT_EQ("HELLO", StringUtils::ToUpper("HeLLo"));
    EXPECT_EQ("HELLO123", StringUtils::ToUpper("hello123"));
}

// Starts with
TEST(StringUtils, StartsWith)
{
    EXPECT_TRUE(StringUtils::StartsWith("hello world", "hello"));
    EXPECT_TRUE(StringUtils::StartsWith("hello", "hello"));
    EXPECT_FALSE(StringUtils::StartsWith("hello", "world"));
    EXPECT_FALSE(StringUtils::StartsWith("hi", "hello"));
}

// Ends with
TEST(StringUtils, EndsWith)
{
    EXPECT_TRUE(StringUtils::EndsWith("hello world", "world"));
    EXPECT_TRUE(StringUtils::EndsWith("world", "world"));
    EXPECT_FALSE(StringUtils::EndsWith("world", "hello"));
    EXPECT_FALSE(StringUtils::EndsWith("hi", "hello"));
}

// Contains
TEST(StringUtils, Contains)
{
    EXPECT_TRUE(StringUtils::Contains("hello world", "hello"));
    EXPECT_TRUE(StringUtils::Contains("hello world", "world"));
    EXPECT_TRUE(StringUtils::Contains("hello world", "lo wo"));
    EXPECT_FALSE(StringUtils::Contains("hello", "world"));
}

// Equals ignore case
TEST(StringUtils, EqualsIgnoreCase)
{
    EXPECT_TRUE(StringUtils::EqualsIgnoreCase("hello", "HELLO"));
    EXPECT_TRUE(StringUtils::EqualsIgnoreCase("Hello", "hello"));
    EXPECT_TRUE(StringUtils::EqualsIgnoreCase("HeLLo", "hEllO"));
    EXPECT_TRUE(StringUtils::EqualsIgnoreCase("test123", "TEST123"));
    EXPECT_FALSE(StringUtils::EqualsIgnoreCase("hello", "world"));
    EXPECT_FALSE(StringUtils::EqualsIgnoreCase("hello", "hello "));
    EXPECT_FALSE(StringUtils::EqualsIgnoreCase("hello", "hell"));
}

// Replace first
TEST(StringUtils, Replace)
{
    EXPECT_EQ("baz bar foo", StringUtils::Replace("foo bar foo", "foo", "baz"));
    EXPECT_EQ("hello world", StringUtils::Replace("hello world", "xyz", "abc"));
}

// Replace all
TEST(StringUtils, ReplaceAll)
{
    EXPECT_EQ("baz bar baz", StringUtils::ReplaceAll("foo bar foo", "foo", "baz"));
    EXPECT_EQ("hello world", StringUtils::ReplaceAll("hello world", "xyz", "abc"));
    EXPECT_EQ("aaab2c3", StringUtils::ReplaceAll("a1b2c3", "1", "aa")); // Replace only '1'
    EXPECT_EQ("aaab2c", StringUtils::ReplaceAll("a1b2c", "1", "aa"));   // Fix expected value
}

// Replace with empty
TEST(StringUtils, ReplaceWithEmpty)
{
    EXPECT_EQ(" bar ", StringUtils::ReplaceAll("foo bar foo", "foo", ""));
}

// Pad right
TEST(StringUtils, PadRight)
{
    EXPECT_EQ("hello     ", StringUtils::Pad("hello", 10, ' ', false));
    EXPECT_EQ("123000", StringUtils::Pad("123", 6, '0', false));
}

// Pad left
TEST(StringUtils, PadLeft)
{
    EXPECT_EQ("     hello", StringUtils::Pad("hello", 10, ' ', true));
    EXPECT_EQ("000123", StringUtils::Pad("123", 6, '0', true));
}

// Pad no change
TEST(StringUtils, PadNoChange)
{
    EXPECT_EQ("hello", StringUtils::Pad("hello", 3, ' ', false));
    EXPECT_EQ("hello", StringUtils::Pad("hello", 5, ' ', false));
}

// Repeat string
TEST(StringUtils, Repeat)
{
    EXPECT_EQ("aaa", StringUtils::Repeat("a", 3));
    EXPECT_EQ("ababab", StringUtils::Repeat("ab", 3));
    EXPECT_EQ("", StringUtils::Repeat("x", 0));
    EXPECT_EQ("", StringUtils::Repeat("", 10));
}

// Complex workflow - parse CSV line
TEST(StringUtils, ParseCSVLine)
{
    std::string csv = "  John , Doe , 30 , Developer  ";
    auto fields = StringUtils::Split(csv, ',');

    EXPECT_EQ(4, fields.size());
    EXPECT_EQ("John", StringUtils::Trim(fields[0]));
    EXPECT_EQ("Doe", StringUtils::Trim(fields[1]));
    EXPECT_EQ("30", StringUtils::Trim(fields[2]));
    EXPECT_EQ("Developer", StringUtils::Trim(fields[3]));
}

// Complex workflow - format log message
TEST(StringUtils, FormatLogMessage)
{
    std::string level = StringUtils::ToUpper("info");
    std::string message = StringUtils::Trim("  server started  ");
    std::string formatted = "[" + level + "] " + message;

    EXPECT_EQ("[INFO] server started", formatted);
}

RUN_ALL_TESTS()
