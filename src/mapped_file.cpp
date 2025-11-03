/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include "lmcore/mapped_file.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <iostream>

namespace lmshao::lmcore {

std::shared_ptr<MappedFile> MappedFile::Open(const std::string &path)
{
    auto file = std::shared_ptr<MappedFile>(new MappedFile());
    if (!file->OpenImpl(path)) {
        return nullptr;
    }
    return file;
}

bool MappedFile::OpenImpl(const std::string &path)
{
#ifdef _WIN32
    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(hFile, &file_size)) {
        std::cerr << "Failed to get file size: " << path << std::endl;
        CloseHandle(hFile);
        return false;
    }

    if (file_size.QuadPart == 0) {
        std::cerr << "File is empty: " << path << std::endl;
        CloseHandle(hFile);
        return false;
    }

    HANDLE hMapping = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);

    if (!hMapping) {
        std::cerr << "Failed to create file mapping: " << path << std::endl;
        CloseHandle(hFile);
        return false;
    }

    void *addr = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);

    if (!addr) {
        std::cerr << "Failed to map view of file: " << path << std::endl;
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return false;
    }

    file_handle_ = hFile;
    mapping_handle_ = hMapping;
    data_ = static_cast<uint8_t *>(addr);
    size_ = static_cast<size_t>(file_size.QuadPart);
    path_ = path;

#else
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    struct stat sb;
    if (fstat(fd, &sb) < 0) {
        std::cerr << "Failed to get file size: " << path << std::endl;
        close(fd);
        return false;
    }

    if (sb.st_size == 0) {
        std::cerr << "File is empty: " << path << std::endl;
        close(fd);
        return false;
    }

    void *addr = mmap(nullptr, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        std::cerr << "Failed to mmap file: " << path << std::endl;
        close(fd);
        return false;
    }

    madvise(addr, sb.st_size, MADV_SEQUENTIAL);

    fd_ = fd;
    data_ = static_cast<uint8_t *>(addr);
    size_ = static_cast<size_t>(sb.st_size);
    path_ = path;
#endif

    return true;
}

void MappedFile::Close()
{
    if (!data_) {
        return;
    }

#ifdef _WIN32
    UnmapViewOfFile(data_);
    if (mapping_handle_) {
        CloseHandle(static_cast<HANDLE>(mapping_handle_));
        mapping_handle_ = nullptr;
    }
    if (file_handle_) {
        CloseHandle(static_cast<HANDLE>(file_handle_));
        file_handle_ = nullptr;
    }
#else
    if (data_ != nullptr) {
        munmap(data_, size_);
    }
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
#endif

    data_ = nullptr;
    size_ = 0;
    path_.clear();
}

MappedFile::~MappedFile()
{
    Close();
}

} // namespace lmshao::lmcore
