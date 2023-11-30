#include "memory_test.h"

TEST_F(MemoryTest, GetUint8) {
  for (uint32_t addr = 0; addr < Memory::MEM_SIZE; addr++) {
    ASSERT_EQ(mem.GetUint8(addr), 0x00)
        << +mem.GetUint8(addr) << "not equal to 0x00" << std::endl;
  }
}

TEST_F(MemoryTest, GetUint16) {
  for (uint32_t addr = 0; addr < Memory::MEM_SIZE; addr += 2) {
    ASSERT_EQ(mem.GetUint16(addr), 0x0000)
        << "Addr: 0x" << std::hex << addr << std::endl;
  }
}

TEST_F(MemoryTest, SetUint8) {
  MemAddr addr = 0x1234;
  uint8_t val = 0xDE;

  mem.SetUint8(addr, val);
  ASSERT_EQ(mem.GetUint8(addr), val)
      << "SetUint8 did not set properly" << std::endl;
}

TEST_F(MemoryTest, SetUint16) {
  MemAddr addr = 0x4320;
  uint16_t val = 0xDEAD;

  mem.SetUint16(addr, __bswap_16(val));
  ASSERT_EQ(mem.GetUint16(addr), val)
      << "SetUint16 did not set properly" << std::endl;
}

TEST_F(MemoryTest, SetUint16_NonAligned) {
  MemAddr addr = 0x4321;
  uint16_t val = 0xDEAD;

  EXPECT_THROW(mem.SetUint16(addr, val), MemoryException);
}

TEST_F(MemoryTest, ElfReader) {
  mem.LoadFile(DOCUMENT_PATH);
  auto val = mem.GetUint16(0xfffe);
  EXPECT_EQ(val, 0xf842) << "Memory not loaded properly";
}
