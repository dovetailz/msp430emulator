#ifndef memory_test_h
#define memory_test_h

#include <iostream>

#include "gtest/gtest.h"
#include "memory.h"

class MemoryTest : public ::testing::Test {
 public:
  MemoryTest(){};
  ~MemoryTest(){};

  void SetUp(){};
  void TearDown(){};

  Memory mem;
};

#endif