#include "memory.h"

#include <byteswap.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include<chrono>
#include<thread>


#include "read_elf.h"

Memory::Memory() {
  // for (uint32_t addr = 0; addr < Memory::MEM_SIZE; addr++) {
  //   mem[addr] = 0xFF;
  // }
}

Memory::~Memory() {}

uint8_t Memory::GetUint8(MemAddr addr) { return mem[addr]; }

uint16_t Memory::GetUint16(MemAddr addr) {
  uint16_t val = static_cast<uint16_t>(mem[addr]) << 8;
  val = val | static_cast<uint16_t>(mem[addr + 1]);
  return __bswap_16(val);
}

void Memory::SetUint8(MemAddr addr, uint8_t val) {
  mem[addr] = val;
  if (addr == 0x21) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "P1OUT: 0x" << std::hex << +val << std::endl;
  }
}

void Memory::SetUint16(MemAddr addr, uint16_t val) {
  CheckBounds(addr);
  auto msb = val >> 8;
  auto lsb = val & 0x00FF;
  mem[addr] = msb;
  mem[addr + 1] = lsb;
}

void Memory::SetUint16BSwap(MemAddr addr, uint16_t val) {
  val = __bswap_16(val);
  CheckBounds(addr);
  auto msb = val >> 8;
  auto lsb = val & 0x00FF;
  mem[addr] = msb;
  mem[addr + 1] = lsb;
}

void Memory::CheckBounds(MemAddr addr) {
  if (addr % 2 == 0) {
    return;
  }
  std::string error = std::to_string(addr);
  error += " is not word aligned";
  throw MemoryException(error);
}

void Memory::LoadFile(std::string filepath) {
  ElfReader elf_reader(filepath);
  auto segments = elf_reader.GetLoadableSegments();
  if (!segments.has_value()) {
    throw(ElfReaderException("No loadable segments found in file"));
  }
  for (auto segment : segments.value()) {
    // std::cout << "Loading segment into memory" << std::endl;

    std::ifstream elf_file(filepath, std::ios::binary | std::ios::ate);
    std::streamsize size = elf_file.tellg();
    auto mem_addr = segment.p_offset;
    auto mem_size = segment.p_memsz;
    elf_file.seekg(mem_addr, std::ios::beg);
    char* buffer = new char[mem_size];
    elf_file.read(buffer, mem_size);
    for (int x = 0; x < mem_size; x++) {
      mem[x + segment.p_paddr] = buffer[x];
    }
  }
  // DisplayMem();
}

void Memory::DisplayMem() {
  for (int x = 0; x < MEM_SIZE; x += 16) {
    std::cout << std::hex << std::setfill('0') << std::setw(8) << std::right
              << x << "  ";
    std::cout << std::hex << std::setfill('0') << std::setw(2) << std::right
              << +mem[x];
    for (int z = 1; z < 16; z++) {
      if (z == 8) {
        std::cout << " ";
      }
      std::cout << " " << std::hex << std::setfill('0') << std::setw(2)
                << std::right << +mem[x + z];
    }

    std::cout << "  |";
    for (int z = 0; z < 16; z++) {
      if (isprint(mem[x + z])) {
        std::cout << static_cast<char>(mem[x + z]);
      } else {
        std::cout << ".";
      }
    }
    std::cout << "|";
    std::cout << std::endl;
    ;
  }
}