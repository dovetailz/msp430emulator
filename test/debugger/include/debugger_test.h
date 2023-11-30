#ifndef debugger_test_h
#define debugger_test_h

#include <iostream>

#include "debugger.h"
#include "gtest/gtest.h"
#include "memory.h"
#include "processor.h"

class DebuggerTest : public ::testing::Test {
 public:
  DebuggerTest(){};
  ~DebuggerTest(){};

  void SetUp();
  void TearDown(){};

  Debugger debug;
};

#endif