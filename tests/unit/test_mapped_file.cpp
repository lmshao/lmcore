/**
 * @author SHAO Liming <lmshao@163.com>
 * @copyright Copyright (c) 2025 SHAO Liming
 * @license MIT
 *
 * SPDX-License-Identifier: MIT
 */

#include <lmcore/data_buffer.h>
#include <lmcore/mapped_file.h>

#include <cstdio>
#include <cstring>
#include <fstream>

#include "../test_framework.h"

using namespace lmshao::lmcore;

// Helper function to create a test file
static std::string CreateTestFile(const std::string &filename, const std::string &content)
{
    std::ofstream file(filename, std::ios::binary);
    file.write(content.c_str(), content.size());
    file.close();
    return filename;
}

// Helper function to delete a test file
static void DeleteTestFile(const std::string &filename)
{
    std::remove(filename.c_str());
}

TEST(MappedFile, OpenValidFile)
{
    const std::string test_file = "test_mapped_file_valid.txt";
    const std::string test_content = "Hello, MappedFile!";

    CreateTestFile(test_file, test_content);

    auto file = MappedFile::Open(test_file);
    EXPECT_TRUE(file != nullptr);
    EXPECT_TRUE(file->IsValid());
    EXPECT_EQ(test_content.size(), file->Size());
    EXPECT_TRUE(file->Data() != nullptr);

    // Verify content
    EXPECT_EQ(0, std::memcmp(file->Data(), test_content.data(), test_content.size()));

    DeleteTestFile(test_file);
}

TEST(MappedFile, OpenNonExistentFile)
{
    auto file = MappedFile::Open("non_existent_file_12345.txt");
    EXPECT_TRUE(file == nullptr);
}

TEST(MappedFile, OpenEmptyFile)
{
    const std::string test_file = "test_mapped_file_empty.txt";
    CreateTestFile(test_file, "");

    auto file = MappedFile::Open(test_file);
    EXPECT_TRUE(file == nullptr);

    DeleteTestFile(test_file);
}

TEST(MappedFile, ReadLargeFile)
{
    const std::string test_file = "test_mapped_file_large.bin";
    const size_t file_size = 10 * 1024 * 1024; // 10 MB

    // Create a large test file with pattern
    {
        std::ofstream file(test_file, std::ios::binary);
        for (size_t i = 0; i < file_size; i++) {
            uint8_t byte = static_cast<uint8_t>(i % 256);
            file.write(reinterpret_cast<const char *>(&byte), 1);
        }
    }

    auto file = MappedFile::Open(test_file);
    EXPECT_TRUE(file != nullptr);
    EXPECT_TRUE(file->IsValid());
    EXPECT_EQ(file_size, file->Size());

    // Verify pattern at various positions
    const uint8_t *data = file->Data();
    EXPECT_EQ(0, data[0]);
    EXPECT_EQ(255, data[255]);
    EXPECT_EQ(0, data[256]);
    EXPECT_EQ(100, data[100]);
    EXPECT_EQ(static_cast<uint8_t>((file_size - 1) % 256), data[file_size - 1]);

    DeleteTestFile(test_file);
}

TEST(MappedFile, RandomAccess)
{
    const std::string test_file = "test_mapped_file_random.bin";
    const size_t file_size = 1024 * 1024; // 1 MB

    // Create test file with known pattern
    {
        std::ofstream file(test_file, std::ios::binary);
        for (size_t i = 0; i < file_size; i++) {
            uint8_t byte = static_cast<uint8_t>((i * 7) % 256);
            file.write(reinterpret_cast<const char *>(&byte), 1);
        }
    }

    auto file = MappedFile::Open(test_file);
    EXPECT_TRUE(file != nullptr);

    const uint8_t *data = file->Data();

    // Test random access at various positions
    EXPECT_EQ(static_cast<uint8_t>((0 * 7) % 256), data[0]);
    EXPECT_EQ(static_cast<uint8_t>((1000 * 7) % 256), data[1000]);
    EXPECT_EQ(static_cast<uint8_t>((50000 * 7) % 256), data[50000]);
    EXPECT_EQ(static_cast<uint8_t>((500000 * 7) % 256), data[500000]);
    EXPECT_EQ(static_cast<uint8_t>(((file_size - 1) * 7) % 256), data[file_size - 1]);

    DeleteTestFile(test_file);
}

TEST(MappedFile, GetPath)
{
    const std::string test_file = "test_mapped_file_path.txt";
    const std::string test_content = "Path test";

    CreateTestFile(test_file, test_content);

    auto file = MappedFile::Open(test_file);
    EXPECT_TRUE(file != nullptr);
    EXPECT_EQ(test_file, file->Path());

    DeleteTestFile(test_file);
}

TEST(MappedFile, DirectAccessZeroCopy)
{
    const std::string test_file = "test_mapped_file_direct_access.txt";
    const std::string test_content = "Zero-copy direct access test";

    CreateTestFile(test_file, test_content);

    auto file = MappedFile::Open(test_file);
    EXPECT_TRUE(file != nullptr);

    // Direct access - true zero-copy (recommended)
    const uint8_t *data = file->Data();
    size_t size = file->Size();

    EXPECT_TRUE(data != nullptr);
    EXPECT_EQ(test_content.size(), size);
    EXPECT_EQ(0, std::memcmp(data, test_content.data(), test_content.size()));

    // Access with offset (zero-copy)
    const uint8_t *offset_data = data + 5;
    EXPECT_EQ(0, std::memcmp(offset_data, "copy direct", 11));

    DeleteTestFile(test_file);
}

TEST(MappedFile, MultipleInstances)
{
    const std::string test_file = "test_mapped_file_multiple.txt";
    const std::string test_content = "Multiple instances test";

    CreateTestFile(test_file, test_content);

    // Open same file multiple times
    auto file1 = MappedFile::Open(test_file);
    auto file2 = MappedFile::Open(test_file);
    auto file3 = MappedFile::Open(test_file);

    EXPECT_TRUE(file1 != nullptr);
    EXPECT_TRUE(file2 != nullptr);
    EXPECT_TRUE(file3 != nullptr);

    EXPECT_EQ(test_content.size(), file1->Size());
    EXPECT_EQ(test_content.size(), file2->Size());
    EXPECT_EQ(test_content.size(), file3->Size());

    // All should have same content
    EXPECT_EQ(0, std::memcmp(file1->Data(), test_content.data(), test_content.size()));
    EXPECT_EQ(0, std::memcmp(file2->Data(), test_content.data(), test_content.size()));
    EXPECT_EQ(0, std::memcmp(file3->Data(), test_content.data(), test_content.size()));

    DeleteTestFile(test_file);
}

TEST(MappedFile, BinaryData)
{
    const std::string test_file = "test_mapped_file_binary.bin";

    // Create binary file with all byte values
    {
        std::ofstream file(test_file, std::ios::binary);
        for (int i = 0; i < 256; i++) {
            uint8_t byte = static_cast<uint8_t>(i);
            file.write(reinterpret_cast<const char *>(&byte), 1);
        }
    }

    auto file = MappedFile::Open(test_file);
    EXPECT_TRUE(file != nullptr);
    EXPECT_EQ(256, file->Size());

    const uint8_t *data = file->Data();
    for (int i = 0; i < 256; i++) {
        EXPECT_EQ(static_cast<uint8_t>(i), data[i]);
    }

    DeleteTestFile(test_file);
}

TEST(MappedFile, H264SimulatedData)
{
    const std::string test_file = "test_mapped_file_h264.bin";

    // Simulate H.264 NAL units with start codes
    std::vector<uint8_t> h264_data = {// Start code + SPS (NAL type 7)
                                      0x00, 0x00, 0x00, 0x01, 0x67, 0x42, 0x00, 0x1E,
                                      // Start code + PPS (NAL type 8)
                                      0x00, 0x00, 0x00, 0x01, 0x68, 0xCE, 0x3C, 0x80,
                                      // Start code + IDR frame (NAL type 5)
                                      0x00, 0x00, 0x00, 0x01, 0x65, 0x88, 0x84, 0x00};

    {
        std::ofstream file(test_file, std::ios::binary);
        file.write(reinterpret_cast<const char *>(h264_data.data()), h264_data.size());
    }

    auto file = MappedFile::Open(test_file);
    EXPECT_TRUE(file != nullptr);
    EXPECT_EQ(h264_data.size(), file->Size());

    const uint8_t *data = file->Data();

    // Verify start codes
    EXPECT_EQ(0x00, data[0]);
    EXPECT_EQ(0x00, data[1]);
    EXPECT_EQ(0x00, data[2]);
    EXPECT_EQ(0x01, data[3]);

    // Verify NAL unit types
    EXPECT_EQ(0x67, data[4]);  // SPS
    EXPECT_EQ(0x68, data[12]); // PPS
    EXPECT_EQ(0x65, data[20]); // IDR

    DeleteTestFile(test_file);
}

RUN_ALL_TESTS()
