#ifndef debugger_h
#define debugger_h

#include <iostream>

#include "memory.h"
#include "processor.h"

class Debugger {
 public:
  Debugger();
  ~Debugger();

  void LoadMem(std::string path);
  MemAddr GetPC();
  MemAddr GetResetAddress();
  MemAddr GetSP();
  uint8_t GetMemory(uint8_t addr);
  uint16_t GetMemory(uint16_t addr);
  uint16_t GetRegister(uint16_t reg);
  void DisplayRegisters();
  void DisplayInstruction(MemAddr addr);
  void Step();

  Processor proc;
  Memory mem;
};

#endif