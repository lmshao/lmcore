/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/url.h>

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <regex>
#include <sstream>

namespace lmshao::lmcore {

const std::map<std::string, uint16_t> URL::DEFAULT_PORTS = {
    {"http", 80},  {"https", 443}, {"rtsp", 554}, {"rtsps", 322},  {"ftp", 21},
    {"ftps", 990}, {"ws", 80},     {"wss", 443},  {"ssh", 22},     {"telnet", 23},
    {"smtp", 25},  {"pop3", 110},  {"imap", 143}, {"mysql", 3306}, {"redis", 6379}};

std::shared_ptr<URL> URL::Parse(const std::string &url)
{
    auto parsed = std::shared_ptr<URL>(new URL());
    if (parsed->ParseImpl(url)) {
        return parsed;
    }
    return nullptr;
}

bool URL::ParseImpl(const std::string &url)
{
    if (url.empty()) {
        return false;
    }

    // RFC 3986 URL regex pattern
    // ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?
    static const std::regex URL_REGEX(R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)",
                                      std::regex::extended);

    std::smatch matches;
    if (!std::regex_match(url, matches, URL_REGEX)) {
        return false;
    }

    // Extract components
    scheme_ = matches[2].str();
    std::string authority = matches[4].str();
    path_ = matches[5].str();
    query_ = matches[7].str();
    fragment_ = matches[9].str();

    // Convert scheme to lowercase
    std::transform(scheme_.begin(), scheme_.end(), scheme_.begin(), [](unsigned char c) { return std::tolower(c); });

    // Parse authority: [userinfo@]host[:port]
    if (!authority.empty()) {
        ParseAuthority(authority);
    }

    // Validate: must have scheme and host (except for file:// which may not have host)
    valid_ = !scheme_.empty() && (!host_.empty() || scheme_ == "file");
    return valid_;
}

void URL::ParseAuthority(const std::string &authority)
{
    // Format: [userinfo@]host[:port]
    // userinfo format: username[:password]

    std::string remaining = authority;

    // Extract userinfo if present
    size_t at_pos = remaining.find('@');
    if (at_pos != std::string::npos) {
        std::string userinfo = remaining.substr(0, at_pos);
        remaining = remaining.substr(at_pos + 1);

        // Parse username:password
        size_t colon_pos = userinfo.find(':');
        if (colon_pos != std::string::npos) {
            username_ = Decode(userinfo.substr(0, colon_pos));
            password_ = Decode(userinfo.substr(colon_pos + 1));
        } else {
            username_ = Decode(userinfo);
        }
    }

    // Parse host[:port]
    // Handle IPv6 addresses: [::1] or [2001:db8::1]
    if (remaining[0] == '[') {
        size_t bracket_end = remaining.find(']');
        if (bracket_end != std::string::npos) {
            host_ = remaining.substr(1, bracket_end - 1);
            remaining = remaining.substr(bracket_end + 1);

            // Check for port after ]
            if (!remaining.empty() && remaining[0] == ':') {
                try {
                    port_ = static_cast<uint16_t>(std::stoi(remaining.substr(1)));
                } catch (...) {
                    port_ = 0;
                }
            }
        } else {
            host_ = remaining; // Malformed, but keep it
        }
    } else {
        // IPv4 or hostname
        size_t colon_pos = remaining.rfind(':');
        if (colon_pos != std::string::npos) {
            host_ = remaining.substr(0, colon_pos);
            try {
                port_ = static_cast<uint16_t>(std::stoi(remaining.substr(colon_pos + 1)));
            } catch (...) {
                // Invalid port, treat whole string as host
                host_ = remaining;
                port_ = 0;
            }
        } else {
            host_ = remaining;
        }
    }
}

uint16_t URL::Port() const
{
    if (port_ > 0) {
        return port_;
    }
    return GetDefaultPort();
}

uint16_t URL::GetDefaultPort() const
{
    auto it = DEFAULT_PORTS.find(scheme_);
    return it != DEFAULT_PORTS.end() ? it->second : 0;
}

bool URL::IsSecure() const
{
    return scheme_ == "https" || scheme_ == "rtsps" || scheme_ == "wss" || scheme_ == "ftps";
}

std::map<std::string, std::string> URL::ParseQuery() const
{
    std::map<std::string, std::string> params;

    if (query_.empty()) {
        return params;
    }

    // Split by '&' or ';'
    std::istringstream stream(query_);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        // Handle ';' as separator too
        size_t semicolon_pos = 0;
        while ((semicolon_pos = pair.find(';')) != std::string::npos) {
            pair[semicolon_pos] = '&';
        }

        // Split by '='
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = Decode(pair.substr(0, eq_pos));
            std::string value = Decode(pair.substr(eq_pos + 1));
            params[key] = value;
        } else if (!pair.empty()) {
            // Key without value
            params[Decode(pair)] = "";
        }
    }

    return params;
}

std::string URL::GetQueryParam(const std::string &key, const std::string &default_val) const
{
    auto params = ParseQuery();
    auto it = params.find(key);
    return it != params.end() ? it->second : default_val;
}

std::string URL::ToString() const
{
    if (!valid_) {
        return "";
    }

    std::ostringstream oss;

    // Scheme
    oss << scheme_ << "://";

    // Userinfo
    if (!username_.empty()) {
        oss << Encode(username_);
        if (!password_.empty()) {
            oss << ":" << Encode(password_);
        }
        oss << "@";
    }

    // Host (IPv6 needs brackets)
    if (host_.find(':') != std::string::npos) {
        oss << "[" << host_ << "]";
    } else {
        oss << host_;
    }

    // Port (omit if default)
    if (port_ > 0 && port_ != GetDefaultPort()) {
        oss << ":" << port_;
    }

    // Path
    oss << path_;

    // Query
    if (!query_.empty()) {
        oss << "?" << query_;
    }

    // Fragment
    if (!fragment_.empty()) {
        oss << "#" << fragment_;
    }

    return oss.str();
}

std::string URL::Encode(const std::string &str)
{
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex << std::uppercase;

    for (unsigned char c : str) {
        // Unreserved characters (RFC 3986): A-Z a-z 0-9 - _ . ~
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            // Percent-encode
            encoded << '%' << std::setw(2) << static_cast<int>(c);
        }
    }

    return encoded.str();
}

std::string URL::Decode(const std::string &encoded)
{
    std::ostringstream decoded;

    for (size_t i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '%') {
            // Decode %XX
            if (i + 2 < encoded.length()) {
                std::string hex = encoded.substr(i + 1, 2);
                try {
                    int value = std::stoi(hex, nullptr, 16);
                    decoded << static_cast<char>(value);
                    i += 2;
                } catch (...) {
                    // Invalid hex, keep as-is
                    decoded << '%';
                }
            } else {
                decoded << '%';
            }
        } else if (encoded[i] == '+') {
            // '+' in query string represents space
            decoded << ' ';
        } else {
            decoded << encoded[i];
        }
    }

    return decoded.str();
}

} // namespace lmshao::lmcore
