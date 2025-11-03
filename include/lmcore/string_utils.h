/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_STRING_UTILS_H
#define LMSHAO_LMCORE_STRING_UTILS_H

#include <string>
#include <vector>

namespace lmshao::lmcore {

/**
 * @brief String manipulation utilities
 *
 * Common string operations for parsing, formatting, and manipulation.
 *
 * Example usage:
 * @code
 *   // Split string
 *   std::vector<std::string> parts = StringUtils::Split("a,b,c", ',');
 *   // Result: {"a", "b", "c"}
 *
 *   // Join strings
 *   std::string joined = StringUtils::Join({"foo", "bar", "baz"}, "-");
 *   // Result: "foo-bar-baz"
 *
 *   // Trim whitespace
 *   std::string trimmed = StringUtils::Trim("  hello  ");
 *   // Result: "hello"
 *
 *   // Case conversion
 *   std::string upper = StringUtils::ToUpper("hello");
 *   // Result: "HELLO"
 *
 *   // String search
 *   bool starts = StringUtils::StartsWith("hello world", "hello");
 *   // Result: true
 *
 *   // Replace
 *   std::string replaced = StringUtils::ReplaceAll("foo bar foo", "foo", "baz");
 *   // Result: "baz bar baz"
 * @endcode
 */
class StringUtils {
public:
    /**
     * @brief Split string by delimiter
     * @param str String to split
     * @param delimiter Delimiter character
     * @param skip_empty Skip empty strings in result (default: false)
     * @return Vector of substrings
     */
    static std::vector<std::string> Split(const std::string &str, char delimiter, bool skip_empty = false);

    /**
     * @brief Split string by delimiter string
     * @param str String to split
     * @param delimiter Delimiter string
     * @param skip_empty Skip empty strings in result (default: false)
     * @return Vector of substrings
     */
    static std::vector<std::string> Split(const std::string &str, const std::string &delimiter,
                                          bool skip_empty = false);

    /**
     * @brief Join strings with separator
     * @param parts Vector of strings to join
     * @param separator Separator string
     * @return Joined string
     */
    static std::string Join(const std::vector<std::string> &parts, const std::string &separator);

    /**
     * @brief Trim whitespace from both ends
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string Trim(const std::string &str);

    /**
     * @brief Trim whitespace from left end
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string TrimLeft(const std::string &str);

    /**
     * @brief Trim whitespace from right end
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string TrimRight(const std::string &str);

    /**
     * @brief Convert string to lowercase
     * @param str String to convert
     * @return Lowercase string
     */
    static std::string ToLower(const std::string &str);

    /**
     * @brief Convert string to uppercase
     * @param str String to convert
     * @return Uppercase string
     */
    static std::string ToUpper(const std::string &str);

    /**
     * @brief Check if string starts with prefix
     * @param str String to check
     * @param prefix Prefix to search for
     * @return true if string starts with prefix
     */
    static bool StartsWith(const std::string &str, const std::string &prefix);

    /**
     * @brief Check if string ends with suffix
     * @param str String to check
     * @param suffix Suffix to search for
     * @return true if string ends with suffix
     */
    static bool EndsWith(const std::string &str, const std::string &suffix);

    /**
     * @brief Check if string contains substring
     * @param str String to search in
     * @param substr Substring to search for
     * @return true if string contains substring
     */
    static bool Contains(const std::string &str, const std::string &substr);

    /**
     * @brief Compare strings case-insensitively
     * @param str1 First string
     * @param str2 Second string
     * @return true if strings are equal (case-insensitive)
     */
    static bool EqualsIgnoreCase(const std::string &str1, const std::string &str2);

    /**
     * @brief Replace first occurrence of substring
     * @param str String to search in
     * @param from Substring to replace
     * @param to Replacement string
     * @return String with first occurrence replaced
     */
    static std::string Replace(const std::string &str, const std::string &from, const std::string &to);

    /**
     * @brief Replace all occurrences of substring
     * @param str String to search in
     * @param from Substring to replace
     * @param to Replacement string
     * @return String with all occurrences replaced
     */
    static std::string ReplaceAll(const std::string &str, const std::string &from, const std::string &to);

    /**
     * @brief Pad string to specified length with character
     * @param str String to pad
     * @param length Target length
     * @param pad_char Character to pad with (default: space)
     * @param left Pad on left side (default: false - right padding)
     * @return Padded string
     */
    static std::string Pad(const std::string &str, size_t length, char pad_char = ' ', bool left = false);

    /**
     * @brief Repeat string multiple times
     * @param str String to repeat
     * @param count Number of times to repeat
     * @return Repeated string
     */
    static std::string Repeat(const std::string &str, size_t count);
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_STRING_UTILS_H
