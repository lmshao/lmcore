/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2024-2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_URL_H
#define LMSHAO_LMCORE_URL_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

namespace lmshao::lmcore {

/**
 * @brief Universal URL parser supporting HTTP/HTTPS/RTSP/RTSPS/FTP/WS/WSS
 *
 * Conforms to RFC 3986 URL standard.
 *
 * Supported URL format:
 *   scheme://[user:pass@]host[:port][/path][?query][#fragment]
 *
 * Supported schemes:
 * - HTTP/HTTPS (default port: 80/443)
 * - RTSP/RTSPS (default port: 554/322)
 * - FTP/FTPS (default port: 21/990)
 * - WS/WSS (default port: 80/443)
 * - FILE (local file system)
 *
 * Example usage:
 * @code
 *   // Parse RTSP URL
 *   auto url = URL::Parse("rtsp://admin:12345@192.168.1.100:554/stream?token=abc");
 *   if (url && url->IsValid()) {
 *       std::cout << "Host: " << url->Host() << std::endl;        // "192.168.1.100"
 *       std::cout << "Port: " << url->Port() << std::endl;        // 554
 *       std::cout << "Path: " << url->Path() << std::endl;        // "/stream"
 *       std::cout << "Username: " << url->Username() << std::endl;// "admin"
 *
 *       auto params = url->ParseQuery();
 *       std::cout << "Token: " << params["token"] << std::endl;   // "abc"
 *   }
 *
 *   // Parse HTTP URL
 *   auto http = URL::Parse("https://example.com/api?key=value#section");
 *   std::cout << "Scheme: " << http->Scheme() << std::endl;      // "https"
 *   std::cout << "Port: " << http->Port() << std::endl;          // 443 (default)
 *   std::cout << "Fragment: " << http->Fragment() << std::endl;  // "section"
 *
 *   // URL encoding/decoding
 *   std::string encoded = URL::Encode("Hello World!");  // "Hello%20World%21"
 *   std::string decoded = URL::Decode(encoded);         // "Hello World!"
 * @endcode
 */
class URL {
public:
    /**
     * @brief Parse URL string
     * @param url URL string to parse
     * @return Parsed URL object, or nullptr on failure
     */
    static std::shared_ptr<URL> Parse(const std::string &url);

    /**
     * @brief Get URL scheme (protocol)
     * @return Scheme string (e.g., "http", "rtsp")
     */
    const std::string &Scheme() const { return scheme_; }

    /**
     * @brief Get username from URL
     * @return Username (empty if not present)
     */
    const std::string &Username() const { return username_; }

    /**
     * @brief Get password from URL
     * @return Password (empty if not present)
     */
    const std::string &Password() const { return password_; }

    /**
     * @brief Get host (domain or IP address)
     * @return Host string
     */
    const std::string &Host() const { return host_; }

    /**
     * @brief Get port number
     * @return Port number (returns default port for scheme if not specified)
     */
    uint16_t Port() const;

    /**
     * @brief Get raw port number from URL
     * @return Port number (0 if not specified)
     */
    uint16_t RawPort() const { return port_; }

    /**
     * @brief Get path component
     * @return Path string (e.g., "/stream/live")
     */
    const std::string &Path() const { return path_; }

    /**
     * @brief Get query string (without '?')
     * @return Query string (e.g., "key1=val1&key2=val2")
     */
    const std::string &Query() const { return query_; }

    /**
     * @brief Get fragment (without '#')
     * @return Fragment string
     */
    const std::string &Fragment() const { return fragment_; }

    /**
     * @brief Parse query string into key-value map
     * @return Map of query parameters (URL-decoded)
     */
    std::map<std::string, std::string> ParseQuery() const;

    /**
     * @brief Get single query parameter value
     * @param key Parameter name
     * @param default_val Default value if not found
     * @return Parameter value (URL-decoded)
     */
    std::string GetQueryParam(const std::string &key, const std::string &default_val = "") const;

    /**
     * @brief Check if scheme is HTTP or HTTPS
     */
    bool IsHTTP() const { return scheme_ == "http" || scheme_ == "https"; }

    /**
     * @brief Check if scheme is RTSP or RTSPS
     */
    bool IsRTSP() const { return scheme_ == "rtsp" || scheme_ == "rtsps"; }

    /**
     * @brief Check if scheme is FTP or FTPS
     */
    bool IsFTP() const { return scheme_ == "ftp" || scheme_ == "ftps"; }

    /**
     * @brief Check if scheme is WebSocket (WS or WSS)
     */
    bool IsWebSocket() const { return scheme_ == "ws" || scheme_ == "wss"; }

    /**
     * @brief Check if scheme is FILE
     */
    bool IsFile() const { return scheme_ == "file"; }

    /**
     * @brief Check if URL uses secure protocol
     * @return true for https/rtsps/wss/ftps
     */
    bool IsSecure() const;

    /**
     * @brief Check if URL was parsed successfully
     */
    bool IsValid() const { return valid_; }

    /**
     * @brief Reconstruct URL string from components
     * @return Complete URL string
     */
    std::string ToString() const;

    /**
     * @brief URL-encode string (percent encoding per RFC 3986)
     * @param str String to encode
     * @return Encoded string (e.g., "a b" -> "a%20b")
     *
     * Encodes all characters except unreserved chars: A-Z a-z 0-9 - _ . ~
     */
    static std::string Encode(const std::string &str);

    /**
     * @brief URL-decode string
     * @param encoded Encoded string
     * @return Decoded string
     */
    static std::string Decode(const std::string &encoded);

private:
    URL() = default;
    bool ParseImpl(const std::string &url);
    void ParseAuthority(const std::string &authority);
    uint16_t GetDefaultPort() const;

    std::string scheme_;
    std::string username_;
    std::string password_;
    std::string host_;
    uint16_t port_ = 0;
    std::string path_;
    std::string query_;
    std::string fragment_;
    bool valid_ = false;

    static const std::map<std::string, uint16_t> DEFAULT_PORTS;
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_URL_H
