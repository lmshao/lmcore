/*
 * Copyright © 2024 SHAO Liming <lmshao@163.com>. All rights reserved.
 */

#include <lmcore/url.h>

#include "../test_framework.h"

using namespace lmshao::lmcore;

// Basic HTTP URL parsing
TEST(URL, ParseHTTP)
{
    auto url = URL::Parse("http://example.com/path");
    EXPECT_TRUE(url != nullptr);
    EXPECT_TRUE(url->IsValid());
    EXPECT_EQ("http", url->Scheme());
    EXPECT_EQ("example.com", url->Host());
    EXPECT_EQ(80, url->Port());
    EXPECT_EQ("/path", url->Path());
    EXPECT_TRUE(url->IsHTTP());
    EXPECT_FALSE(url->IsSecure());
}

// HTTPS with explicit port
TEST(URL, ParseHTTPS)
{
    auto url = URL::Parse("https://example.com:8443/api/v1");
    EXPECT_TRUE(url != nullptr);
    EXPECT_TRUE(url->IsValid());
    EXPECT_EQ("https", url->Scheme());
    EXPECT_EQ("example.com", url->Host());
    EXPECT_EQ(8443, url->Port());
    EXPECT_EQ("/api/v1", url->Path());
    EXPECT_TRUE(url->IsHTTP());
    EXPECT_TRUE(url->IsSecure());
}

// RTSP URL with authentication
TEST(URL, ParseRTSP)
{
    auto url = URL::Parse("rtsp://admin:12345@192.168.1.100:554/stream");
    EXPECT_TRUE(url != nullptr);
    EXPECT_TRUE(url->IsValid());
    EXPECT_EQ("rtsp", url->Scheme());
    EXPECT_EQ("admin", url->Username());
    EXPECT_EQ("12345", url->Password());
    EXPECT_EQ("192.168.1.100", url->Host());
    EXPECT_EQ(554, url->Port());
    EXPECT_EQ("/stream", url->Path());
    EXPECT_TRUE(url->IsRTSP());
    EXPECT_FALSE(url->IsSecure());
}

// RTSPS (secure RTSP)
TEST(URL, ParseRTSPS)
{
    auto url = URL::Parse("rtsps://camera.local:322/live");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("rtsps", url->Scheme());
    EXPECT_EQ(322, url->Port());
    EXPECT_TRUE(url->IsRTSP());
    EXPECT_TRUE(url->IsSecure());
}

// Query parameters
TEST(URL, ParseQuery)
{
    auto url = URL::Parse("http://example.com/path?key1=value1&key2=value2&key3=");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("key1=value1&key2=value2&key3=", url->Query());

    auto params = url->ParseQuery();
    EXPECT_EQ(3, params.size());
    EXPECT_EQ("value1", params["key1"]);
    EXPECT_EQ("value2", params["key2"]);
    EXPECT_EQ("", params["key3"]);

    EXPECT_EQ("value1", url->GetQueryParam("key1"));
    EXPECT_EQ("default", url->GetQueryParam("nonexist", "default"));
}

// Query with URL encoding
TEST(URL, ParseQueryEncoded)
{
    auto url = URL::Parse("http://example.com/search?q=hello+world&name=%E4%B8%AD%E6%96%87");
    EXPECT_TRUE(url != nullptr);

    auto params = url->ParseQuery();
    EXPECT_EQ("hello world", params["q"]); // '+' becomes space
}

// Fragment
TEST(URL, ParseFragment)
{
    auto url = URL::Parse("https://example.com/page?id=123#section-2");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("/page", url->Path());
    EXPECT_EQ("id=123", url->Query());
    EXPECT_EQ("section-2", url->Fragment());
}

// FTP URL
TEST(URL, ParseFTP)
{
    auto url = URL::Parse("ftp://user:pass@ftp.example.com:2121/file.txt");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("ftp", url->Scheme());
    EXPECT_EQ("user", url->Username());
    EXPECT_EQ("pass", url->Password());
    EXPECT_EQ("ftp.example.com", url->Host());
    EXPECT_EQ(2121, url->Port());
    EXPECT_EQ("/file.txt", url->Path());
    EXPECT_TRUE(url->IsFTP());
    EXPECT_FALSE(url->IsSecure());
}

// WebSocket URL
TEST(URL, ParseWebSocket)
{
    auto url = URL::Parse("wss://socket.example.com/chat");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("wss", url->Scheme());
    EXPECT_EQ(443, url->Port());
    EXPECT_TRUE(url->IsWebSocket());
    EXPECT_TRUE(url->IsSecure());
}

// IPv6 address
TEST(URL, ParseIPv6)
{
    auto url = URL::Parse("http://[2001:db8::1]:8080/path");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("2001:db8::1", url->Host());
    EXPECT_EQ(8080, url->Port());
}

// IPv6 localhost
TEST(URL, ParseIPv6Localhost)
{
    auto url = URL::Parse("http://[::1]/api");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("::1", url->Host());
    EXPECT_EQ(80, url->Port());
    EXPECT_EQ("/api", url->Path());
}

// URL without port (use default)
TEST(URL, DefaultPort)
{
    auto http = URL::Parse("http://example.com/");
    EXPECT_EQ(80, http->Port());
    EXPECT_EQ(0, http->RawPort());

    auto https = URL::Parse("https://example.com/");
    EXPECT_EQ(443, https->Port());

    auto rtsp = URL::Parse("rtsp://example.com/");
    EXPECT_EQ(554, rtsp->Port());
}

// URL encoding
TEST(URL, EncodeBasic)
{
    EXPECT_EQ("Hello%20World", URL::Encode("Hello World"));
    EXPECT_EQ("a%2Bb%3Dc", URL::Encode("a+b=c"));
    EXPECT_EQ("100%25", URL::Encode("100%"));
}

// URL encoding - unreserved characters
TEST(URL, EncodeUnreserved)
{
    // Unreserved: A-Z a-z 0-9 - _ . ~
    EXPECT_EQ("ABC-xyz_123.test~", URL::Encode("ABC-xyz_123.test~"));
}

// URL encoding - special characters
TEST(URL, EncodeSpecial)
{
    EXPECT_EQ("%21%40%23%24", URL::Encode("!@#$"));
    EXPECT_EQ("%2F%3F%3D%26", URL::Encode("/?=&"));
}

// URL decoding
TEST(URL, DecodeBasic)
{
    EXPECT_EQ("Hello World", URL::Decode("Hello%20World"));
    EXPECT_EQ("Hello World", URL::Decode("Hello+World")); // '+' -> space
    EXPECT_EQ("a+b=c", URL::Decode("a%2Bb%3Dc"));
}

// URL decoding - invalid percent encoding
TEST(URL, DecodeInvalid)
{
    EXPECT_EQ("%", URL::Decode("%"));     // Incomplete
    EXPECT_EQ("%1", URL::Decode("%1"));   // Incomplete
    EXPECT_EQ("%ZZ", URL::Decode("%ZZ")); // Invalid hex
}

// Round trip encoding
TEST(URL, EncodeDecodeRoundTrip)
{
    std::string original = "Hello World! Test@123 #$%";
    std::string encoded = URL::Encode(original);
    std::string decoded = URL::Decode(encoded);
    EXPECT_EQ(original, decoded);
}

// ToString - basic URL
TEST(URL, ToStringBasic)
{
    auto url = URL::Parse("http://example.com/path");
    EXPECT_EQ("http://example.com/path", url->ToString());
}

// ToString - full URL with all components
TEST(URL, ToStringFull)
{
    auto url = URL::Parse("https://user:pass@example.com:8443/path?key=val#frag");
    std::string result = url->ToString();
    // Password should be encoded in output
    EXPECT_TRUE(result.find("https://") != std::string::npos);
    EXPECT_TRUE(result.find("user") != std::string::npos);
    EXPECT_TRUE(result.find("pass") != std::string::npos);
    EXPECT_TRUE(result.find("example.com:8443") != std::string::npos);
}

// ToString - IPv6
TEST(URL, ToStringIPv6)
{
    auto url = URL::Parse("http://[::1]:8080/api");
    EXPECT_EQ("http://[::1]:8080/api", url->ToString());
}

// Invalid URL - empty string
TEST(URL, InvalidEmpty)
{
    auto url = URL::Parse("");
    EXPECT_TRUE(url == nullptr);
}

// Invalid URL - no scheme
TEST(URL, InvalidNoScheme)
{
    auto url = URL::Parse("example.com/path");
    EXPECT_TRUE(url == nullptr);
}

// Invalid URL - no host
TEST(URL, InvalidNoHost)
{
    auto url = URL::Parse("http:///path");
    EXPECT_TRUE(url == nullptr); // Should fail to parse
}

// URL with username only (no password)
TEST(URL, UsernameOnly)
{
    auto url = URL::Parse("rtsp://admin@camera.local/stream");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("admin", url->Username());
    EXPECT_EQ("", url->Password());
}

// URL with empty path
TEST(URL, EmptyPath)
{
    auto url = URL::Parse("http://example.com");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("", url->Path());
}

// URL with complex query (multiple delimiters)
TEST(URL, ComplexQuery)
{
    auto url = URL::Parse("http://example.com/?a=1&b=2&c=3");
    EXPECT_TRUE(url != nullptr);
    auto params = url->ParseQuery();
    EXPECT_EQ(3, params.size());
    EXPECT_EQ("1", params["a"]);
    EXPECT_EQ("2", params["b"]);
    EXPECT_EQ("3", params["c"]);
}

// Case insensitivity of scheme
TEST(URL, SchemeCase)
{
    auto url1 = URL::Parse("HTTP://EXAMPLE.COM/PATH");
    auto url2 = URL::Parse("http://EXAMPLE.COM/PATH");
    EXPECT_TRUE(url1 != nullptr);
    EXPECT_TRUE(url2 != nullptr);
    EXPECT_EQ("http", url1->Scheme()); // Should be lowercase
    EXPECT_EQ("http", url2->Scheme());
}

// File URL
TEST(URL, ParseFile)
{
    auto url = URL::Parse("file:///home/user/document.txt");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("file", url->Scheme());
    EXPECT_EQ("/home/user/document.txt", url->Path());
    EXPECT_TRUE(url->IsFile());
}

// URL with special characters in password
TEST(URL, SpecialCharsInPassword)
{
    auto url = URL::Parse("rtsp://user:p%40ss%23word@host/stream");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("user", url->Username());
    EXPECT_EQ("p@ss#word", url->Password()); // Should be decoded
}

// Real-world RTSP URL example
TEST(URL, RealWorldRTSP)
{
    auto url = URL::Parse("rtsp://admin:Admin123@192.168.1.64:554/Streaming/Channels/101?transportmode=unicast");
    EXPECT_TRUE(url != nullptr);
    EXPECT_EQ("rtsp", url->Scheme());
    EXPECT_EQ("admin", url->Username());
    EXPECT_EQ("Admin123", url->Password());
    EXPECT_EQ("192.168.1.64", url->Host());
    EXPECT_EQ(554, url->Port());
    EXPECT_EQ("/Streaming/Channels/101", url->Path());
    EXPECT_EQ("unicast", url->GetQueryParam("transportmode"));
}

RUN_ALL_TESTS()
