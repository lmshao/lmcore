/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include "lmcore/md5.h"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace lmshao::lmcore {

static const uint32_t S11 = 7;
static const uint32_t S12 = 12;
static const uint32_t S13 = 17;
static const uint32_t S14 = 22;
static const uint32_t S21 = 5;
static const uint32_t S22 = 9;
static const uint32_t S23 = 14;
static const uint32_t S24 = 20;
static const uint32_t S31 = 4;
static const uint32_t S32 = 11;
static const uint32_t S33 = 16;
static const uint32_t S34 = 23;
static const uint32_t S41 = 6;
static const uint32_t S42 = 10;
static const uint32_t S43 = 15;
static const uint32_t S44 = 21;

static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) | (~x & z);
}

static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & z) | (y & ~z);
}

static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z)
{
    return x ^ y ^ z;
}

static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z)
{
    return y ^ (x | ~z);
}

static inline uint32_t RotateLeft(uint32_t x, uint32_t n)
{
    return (x << n) | (x >> (32 - n));
}

static inline void FF(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
    a += F(b, c, d) + x + ac;
    a = RotateLeft(a, s);
    a += b;
}

static inline void GG(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
    a += G(b, c, d) + x + ac;
    a = RotateLeft(a, s);
    a += b;
}

static inline void HH(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
    a += H(b, c, d) + x + ac;
    a = RotateLeft(a, s);
    a += b;
}

static inline void II(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
    a += I(b, c, d) + x + ac;
    a = RotateLeft(a, s);
    a += b;
}

static void Encode(uint8_t *output, const uint32_t *input, size_t len)
{
    for (size_t i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = input[i] & 0xff;
        output[j + 1] = (input[i] >> 8) & 0xff;
        output[j + 2] = (input[i] >> 16) & 0xff;
        output[j + 3] = (input[i] >> 24) & 0xff;
    }
}

static void Decode(uint32_t *output, const uint8_t *input, size_t len)
{
    for (size_t i = 0, j = 0; j < len; i++, j += 4) {
        output[i] = static_cast<uint32_t>(input[j]) | (static_cast<uint32_t>(input[j + 1]) << 8) |
                    (static_cast<uint32_t>(input[j + 2]) << 16) | (static_cast<uint32_t>(input[j + 3]) << 24);
    }
}

void MD5::Init(Context &ctx)
{
    ctx.count[0] = ctx.count[1] = 0;
    ctx.state[0] = 0x67452301;
    ctx.state[1] = 0xefcdab89;
    ctx.state[2] = 0x98badcfe;
    ctx.state[3] = 0x10325476;
}

void MD5::Transform(uint32_t state[4], const uint8_t block[64])
{
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    Decode(x, block, 64);

    FF(a, b, c, d, x[0], S11, 0xd76aa478);
    FF(d, a, b, c, x[1], S12, 0xe8c7b756);
    FF(c, d, a, b, x[2], S13, 0x242070db);
    FF(b, c, d, a, x[3], S14, 0xc1bdceee);
    FF(a, b, c, d, x[4], S11, 0xf57c0faf);
    FF(d, a, b, c, x[5], S12, 0x4787c62a);
    FF(c, d, a, b, x[6], S13, 0xa8304613);
    FF(b, c, d, a, x[7], S14, 0xfd469501);
    FF(a, b, c, d, x[8], S11, 0x698098d8);
    FF(d, a, b, c, x[9], S12, 0x8b44f7af);
    FF(c, d, a, b, x[10], S13, 0xffff5bb1);
    FF(b, c, d, a, x[11], S14, 0x895cd7be);
    FF(a, b, c, d, x[12], S11, 0x6b901122);
    FF(d, a, b, c, x[13], S12, 0xfd987193);
    FF(c, d, a, b, x[14], S13, 0xa679438e);
    FF(b, c, d, a, x[15], S14, 0x49b40821);

    GG(a, b, c, d, x[1], S21, 0xf61e2562);
    GG(d, a, b, c, x[6], S22, 0xc040b340);
    GG(c, d, a, b, x[11], S23, 0x265e5a51);
    GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
    GG(a, b, c, d, x[5], S21, 0xd62f105d);
    GG(d, a, b, c, x[10], S22, 0x2441453);
    GG(c, d, a, b, x[15], S23, 0xd8a1e681);
    GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
    GG(a, b, c, d, x[9], S21, 0x21e1cde6);
    GG(d, a, b, c, x[14], S22, 0xc33707d6);
    GG(c, d, a, b, x[3], S23, 0xf4d50d87);
    GG(b, c, d, a, x[8], S24, 0x455a14ed);
    GG(a, b, c, d, x[13], S21, 0xa9e3e905);
    GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
    GG(c, d, a, b, x[7], S23, 0x676f02d9);
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);

    HH(a, b, c, d, x[5], S31, 0xfffa3942);
    HH(d, a, b, c, x[8], S32, 0x8771f681);
    HH(c, d, a, b, x[11], S33, 0x6d9d6122);
    HH(b, c, d, a, x[14], S34, 0xfde5380c);
    HH(a, b, c, d, x[1], S31, 0xa4beea44);
    HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
    HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
    HH(b, c, d, a, x[10], S34, 0xbebfbc70);
    HH(a, b, c, d, x[13], S31, 0x289b7ec6);
    HH(d, a, b, c, x[0], S32, 0xeaa127fa);
    HH(c, d, a, b, x[3], S33, 0xd4ef3085);
    HH(b, c, d, a, x[6], S34, 0x4881d05);
    HH(a, b, c, d, x[9], S31, 0xd9d4d039);
    HH(d, a, b, c, x[12], S32, 0xe6db99e5);
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
    HH(b, c, d, a, x[2], S34, 0xc4ac5665);

    II(a, b, c, d, x[0], S41, 0xf4292244);
    II(d, a, b, c, x[7], S42, 0x432aff97);
    II(c, d, a, b, x[14], S43, 0xab9423a7);
    II(b, c, d, a, x[5], S44, 0xfc93a039);
    II(a, b, c, d, x[12], S41, 0x655b59c3);
    II(d, a, b, c, x[3], S42, 0x8f0ccc92);
    II(c, d, a, b, x[10], S43, 0xffeff47d);
    II(b, c, d, a, x[1], S44, 0x85845dd1);
    II(a, b, c, d, x[8], S41, 0x6fa87e4f);
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
    II(c, d, a, b, x[6], S43, 0xa3014314);
    II(b, c, d, a, x[13], S44, 0x4e0811a1);
    II(a, b, c, d, x[4], S41, 0xf7537e82);
    II(d, a, b, c, x[11], S42, 0xbd3af235);
    II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
    II(b, c, d, a, x[9], S44, 0xeb86d391);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

void MD5::Update(Context &ctx, const uint8_t *data, size_t len)
{
    // Use size_t for loop index to match input length type
    size_t i;
    uint32_t index, partLen;

    index = (ctx.count[0] >> 3) & 0x3F;

    // Explicitly narrow bit length to 32-bit words to avoid C4267 warnings
    const uint32_t bitlen_low = static_cast<uint32_t>(len << 3);
    const uint32_t bitlen_high = static_cast<uint32_t>(len >> 29);

    if ((ctx.count[0] += bitlen_low) < bitlen_low) {
        ctx.count[1]++;
    }
    ctx.count[1] += bitlen_high;

    partLen = 64 - index;

    if (len >= partLen) {
        std::memcpy(&ctx.buffer[index], data, partLen);
        Transform(ctx.state, ctx.buffer);

        for (i = partLen; i + 63 < len; i += 64) {
            Transform(ctx.state, &data[i]);
        }

        index = 0;
    } else {
        i = 0;
    }

    std::memcpy(&ctx.buffer[index], &data[i], len - i);
}

void MD5::Final(Context &ctx, uint8_t digest[16])
{
    uint8_t bits[8];
    uint32_t index, padLen;

    Encode(bits, ctx.count, 8);

    index = (ctx.count[0] >> 3) & 0x3f;
    padLen = (index < 56) ? (56 - index) : (120 - index);

    uint8_t padding[64] = {0x80};
    Update(ctx, padding, padLen);
    Update(ctx, bits, 8);

    Encode(digest, ctx.state, 16);
}

std::string MD5::DigestToHex(const uint8_t digest[16])
{
    std::ostringstream oss;
    for (int i = 0; i < 16; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    return oss.str();
}

std::string MD5::Calculate(const uint8_t *data, size_t len)
{
    Context ctx;
    uint8_t digest[16];

    Init(ctx);
    Update(ctx, data, len);
    Final(ctx, digest);

    return DigestToHex(digest);
}

std::string MD5::Calculate(const std::vector<uint8_t> &data)
{
    return Calculate(data.data(), data.size());
}

std::string MD5::Calculate(const std::string &data)
{
    return Calculate(reinterpret_cast<const uint8_t *>(data.c_str()), data.length());
}

std::string MD5::CalculateFile(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    Context ctx;
    uint8_t digest[16];
    Init(ctx);

    constexpr size_t BUFFER_SIZE = 8192;
    uint8_t buffer[BUFFER_SIZE];

    while (file.read(reinterpret_cast<char *>(buffer), BUFFER_SIZE)) {
        Update(ctx, buffer, file.gcount());
    }
    if (file.gcount() > 0) {
        Update(ctx, buffer, file.gcount());
    }

    Final(ctx, digest);
    return DigestToHex(digest);
}

} // namespace lmshao::lmcore
