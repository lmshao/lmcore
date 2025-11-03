/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LMSHAO_LMCORE_MAPPED_FILE_H
#define LMSHAO_LMCORE_MAPPED_FILE_H

#include <cstdint>
#include <memory>
#include <string>

#include "lmcore/noncopyable.h"

namespace lmshao::lmcore {

/**
 * @brief Memory-mapped file for efficient read-only file access
 *
 * Cross-platform implementation:
 * - Linux/macOS/Unix: mmap() with MADV_SEQUENTIAL optimization
 * - Windows: CreateFileMapping() + MapViewOfFile()
 *
 * Features:
 * - Zero-copy access (OS lazy-loads pages on demand)
 * - Efficient for large files (no full load into physical memory)
 * - Fast random access
 * - Shared page cache across processes
 *
 * Usage:
 * @code
 *   auto file = MappedFile::Open("video.h264");
 *   const uint8_t* data = file->Data();
 *   size_t size = file->Size();
 *   // Direct access: data[offset], data + offset, etc.
 * @endcode
 */
class MappedFile : public NonCopyable {
public:
    ~MappedFile();

    static std::shared_ptr<MappedFile> Open(const std::string &path);

    const uint8_t *Data() const { return data_; }
    size_t Size() const { return size_; }
    bool IsValid() const { return data_ != nullptr && size_ > 0; }
    const std::string &Path() const { return path_; }

private:
    MappedFile() = default;
    bool OpenImpl(const std::string &path);
    void Close();

    uint8_t *data_ = nullptr;
    size_t size_ = 0;
    std::string path_;

#ifdef _WIN32
    void *file_handle_ = nullptr;    // HANDLE
    void *mapping_handle_ = nullptr; // HANDLE
#else
    int fd_ = -1;
#endif
};

} // namespace lmshao::lmcore

#endif // LMSHAO_LMCORE_MAPPED_FILE_H
