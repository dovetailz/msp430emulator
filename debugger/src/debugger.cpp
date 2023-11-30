#include "debugger.h"

Debugger::Debugger() {}

Debugger::~Debugger() {}

void Debugger::LoadMem(std::string path) {
  mem.LoadFile(path);
  proc.SetMemory(&mem);
}

MemAddr Debugger::GetPC() { return *proc.PC; }

MemAddr Debugger::GetSP() { return *proc.SP; }

MemAddr Debugger::GetResetAddress() {
  return mem.GetUint16(Processor::RESET_VECTOR);
}

void Debugger::DisplayRegisters() {
  for (auto reg : proc.register_map) {
    static int counter = 0;
    std::cout << "R" << std::setw(2) << std::left << counter << ": ";
    printf("0x%04x", *reg.second);
    if (reg.first == 0) {
      std::cout << " (PC)";
    } else if (reg.first == 1) {
      std::cout << " (SP)";
    } else if (reg.first == 2) {
      std::cout << " (SR/CG1)";
    } else if (reg.first == 3) {
      std::cout << " (CG2)";
    }
    std::cout << std::endl;
    counter++;
  }
}

void Debugger::DisplayInstruction(MemAddr addr) {
  proc.DisplayInstruction(addr);
}

uint8_t Debugger::GetMemory(uint8_t addr) { return proc.mem->GetUint8(addr); }

uint16_t Debugger::GetMemory(uint16_t addr) {
  return proc.mem->GetUint16(addr);
}

uint16_t Debugger::GetRegister(uint16_t reg) { return *proc.register_map[reg]; }

void Debugger::Step() {
  proc.step = true;
  printf("PC: 0x%04x\n", GetPC());
  proc.Step();
  std::cout<<std::endl;
  std::cin.get();
  std::cout<<std::endl;
  proc.step = false;
}
