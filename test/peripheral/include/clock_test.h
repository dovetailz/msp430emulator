#ifndef clock_test_h
#define clock_test_h

#include <iostream>

#include "clock.h"
#include "gtest/gtest.h"

class ClockTest : public ::testing::Test {
 public:
  ClockTest(){};
  ~ClockTest(){};

  void SetUp(){};
  void TearDown(){};

  Clock clock;
};

#endif
