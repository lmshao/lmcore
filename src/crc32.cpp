/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include "lmcore/crc32.h"

namespace lmshao::lmcore {

uint32_t CRC32::table_[256];
bool CRC32::table_initialized_ = false;

void CRC32::InitTable()
{
    if (table_initialized_) {
        return;
    }

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        table_[i] = crc;
    }
    table_initialized_ = true;
}

uint32_t CRC32::Calculate(const uint8_t *data, size_t len)
{
    InitTable();
    return UpdateInternal(0xFFFFFFFF, data, len) ^ 0xFFFFFFFF;
}

uint32_t CRC32::Calculate(const std::vector<uint8_t> &data)
{
    return Calculate(data.data(), data.size());
}

uint32_t CRC32::Calculate(const std::string &data)
{
    return Calculate(reinterpret_cast<const uint8_t *>(data.c_str()), data.length());
}

uint32_t CRC32::UpdateInternal(uint32_t crc, const uint8_t *data, size_t len)
{
    InitTable();

    for (size_t i = 0; i < len; i++) {
        crc = table_[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }

    return crc;
}

void CRC32::Context::Update(const uint8_t *data, size_t len)
{
    crc_ = CRC32::UpdateInternal(crc_, data, len);
}

void CRC32::Context::Update(const std::vector<uint8_t> &data)
{
    Update(data.data(), data.size());
}

void CRC32::Context::Update(const std::string &data)
{
    Update(reinterpret_cast<const uint8_t *>(data.c_str()), data.length());
}

uint32_t CRC32::Context::Final()
{
    return crc_ ^ 0xFFFFFFFF;
}

} // namespace lmshao::lmcore
