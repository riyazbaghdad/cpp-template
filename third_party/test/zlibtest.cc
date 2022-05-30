#include <zlib.h>

#include <iostream>
#include <string>

#include "gtest/gtest.h"

struct ZLibData {
  char* buffer;
  uLongf size;
  size_t max_size;
};

int CompressString(std::string& input, ZLibData& compressData) {
  size_t inputSize = input.size();

  // Ask zlib for the max destination size
  compressData.max_size = compressBound(inputSize);

  compressData.buffer = new char[compressData.max_size];

  return compress(
      reinterpret_cast<Bytef*>(compressData.buffer), &compressData.size,
      reinterpret_cast<Bytef*>((unsigned char*)input.c_str()), inputSize);
}

std::string UnCompressString(const ZLibData& compressData) {
  char* uncompress_buffer = new char[compressData.max_size];
  uLongf uncompress_size = compressData.max_size;

  uncompress(reinterpret_cast<Bytef*>(uncompress_buffer), &uncompress_size,
             reinterpret_cast<Bytef*>(compressData.buffer), compressData.size);

  std::cout << "uncompress_buffer: " << uncompress_buffer << std::endl;

  return std::string(uncompress_buffer, uncompress_size);
}

TEST(ZLibTest, CompressAndUncompress1) {
  std::string input =
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
  ZLibData compress_data;
  CompressString(input, compress_data);
  EXPECT_EQ(compress_data.size, 14);

  auto actual_uncompressed = UnCompressString(compress_data);
  EXPECT_EQ(actual_uncompressed, input);
}

TEST(ZLibTest, CompressAndUncompress2) {
  std::string input = "This is a test. This is a test. This is a test.";
  ZLibData compress_data;
  CompressString(input, compress_data);
  EXPECT_EQ(compress_data.size, 25);

  auto actual_uncompressed = UnCompressString(compress_data);
  EXPECT_EQ(actual_uncompressed, input);
}

TEST(ZLibTest, CompressAndUncompress3) {
  std::string input =
      "test test test test test test test test test test test test test test "
      "test.";
  ZLibData compress_data;
  CompressString(input, compress_data);
  EXPECT_EQ(compress_data.size, 17);

  auto actual_uncompressed = UnCompressString(compress_data);
  EXPECT_EQ(actual_uncompressed, input);
}
