#ifndef memory_h
#define memory_h

#include <cstdint>
#include <exception>
#include <iomanip>
#include <string>

typedef uint16_t MemAddr;

class Memory {
 public:
  constexpr static uint32_t MEM_SIZE = 0x10000;
  Memory();
  ~Memory();
  uint8_t GetUint8(MemAddr addr);
  uint16_t GetUint16(MemAddr addr);
  void SetUint8(MemAddr addr, uint8_t val);
  void SetUint16(MemAddr addr, uint16_t val);
  void SetUint16BSwap(MemAddr addr, uint16_t val);
  void LoadFile(std::string filepath);
  void DisplayMem();

 private:
  uint8_t mem[MEM_SIZE];
  void CheckBounds(MemAddr addr);
};

class MemoryException : public std::exception {
  std::string _msg;

 public:
  MemoryException(const std::string& msg) : _msg(msg) {}

  virtual const char* what() const noexcept override { return _msg.c_str(); }
};

#endif