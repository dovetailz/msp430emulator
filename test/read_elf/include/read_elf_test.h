#ifndef read_elf_test_h
#define read_elf_test_h

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "read_elf.h"

class ReadElfTest : public ::testing::Test {
 public:
  ReadElfTest() : reader(DOCUMENT_PATH){};
  ~ReadElfTest(){};

  void SetUp(){};
  void TearDown(){};

  ElfReader reader;

 protected:
  uint8_t magic[4]{0x7f, 0x45, 0x4c, 0x46};
};
#endif