/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/string_utils.h>

#include <algorithm>
#include <cctype>
#include <sstream>

namespace lmshao::lmcore {

std::vector<std::string> StringUtils::Split(const std::string &str, char delimiter, bool skip_empty)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        if (!skip_empty || !item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

std::vector<std::string> StringUtils::Split(const std::string &str, const std::string &delimiter, bool skip_empty)
{
    std::vector<std::string> result;

    if (str.empty() || delimiter.empty()) {
        if (!str.empty()) {
            result.push_back(str);
        }
        return result;
    }

    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!skip_empty || !token.empty()) {
            result.push_back(token);
        }
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    std::string token = str.substr(start);
    if (!skip_empty || !token.empty()) {
        result.push_back(token);
    }

    return result;
}

std::string StringUtils::Join(const std::vector<std::string> &parts, const std::string &separator)
{
    if (parts.empty()) {
        return "";
    }

    std::string result;
    size_t total_size = 0;

    // Calculate total size for efficient allocation
    for (const auto &part : parts) {
        total_size += part.size();
    }
    if (parts.size() > 1) {
        total_size += separator.size() * (parts.size() - 1);
    }
    result.reserve(total_size);

    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            result += separator;
        }
        result += parts[i];
    }

    return result;
}

std::string StringUtils::Trim(const std::string &str)
{
    return TrimLeft(TrimRight(str));
}

std::string StringUtils::TrimLeft(const std::string &str)
{
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    return str.substr(start);
}

std::string StringUtils::TrimRight(const std::string &str)
{
    size_t end = str.size();
    while (end > 0 && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }
    return str.substr(0, end);
}

std::string StringUtils::ToLower(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string StringUtils::ToUpper(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool StringUtils::StartsWith(const std::string &str, const std::string &prefix)
{
    if (prefix.size() > str.size()) {
        return false;
    }
    return str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::EndsWith(const std::string &str, const std::string &suffix)
{
    if (suffix.size() > str.size()) {
        return false;
    }
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool StringUtils::Contains(const std::string &str, const std::string &substr)
{
    return str.find(substr) != std::string::npos;
}

bool StringUtils::EqualsIgnoreCase(const std::string &str1, const std::string &str2)
{
    if (str1.size() != str2.size()) {
        return false;
    }

    const char *p1 = str1.c_str();
    const char *p2 = str2.c_str();
    size_t len = str1.size();

    for (size_t i = 0; i < len; ++i) {
        if (std::tolower(static_cast<unsigned char>(p1[i])) != std::tolower(static_cast<unsigned char>(p2[i]))) {
            return false;
        }
    }

    return true;
}

std::string StringUtils::Replace(const std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty()) {
        return str;
    }

    size_t pos = str.find(from);
    if (pos == std::string::npos) {
        return str;
    }

    std::string result = str;
    result.replace(pos, from.length(), to);
    return result;
}

std::string StringUtils::ReplaceAll(const std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty()) {
        return str;
    }

    std::string result = str;
    size_t pos = 0;

    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }

    return result;
}

std::string StringUtils::Pad(const std::string &str, size_t length, char pad_char, bool left)
{
    if (str.size() >= length) {
        return str;
    }

    size_t pad_count = length - str.size();
    std::string padding(pad_count, pad_char);

    return left ? (padding + str) : (str + padding);
}

std::string StringUtils::Repeat(const std::string &str, size_t count)
{
    if (count == 0 || str.empty()) {
        return "";
    }

    std::string result;
    result.reserve(str.size() * count);

    for (size_t i = 0; i < count; ++i) {
        result += str;
    }

    return result;
}

} // namespace lmshao::lmcore
