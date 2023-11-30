#ifndef emulator_h
#define emulator_h

#include <iostream>

#include "debugger.h"
#include "memory.h"
#include "processor.h"

class Emulator {
 public:
  Emulator(){};
  Emulator(std::string filepath);
  ~Emulator(){};
  void Cycle();

 private:
  Debugger debug;
  std::string filepath;
};

#endif