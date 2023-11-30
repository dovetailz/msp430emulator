#include "processor_test.h"

/**
 * @brief Load MSP430 Binary Into Memory
 *
 */
void ProcessorTest::SetUp() {
  mem.LoadFile(DOCUMENT_PATH);
  proc.SetMemory(&mem);
}

/**
 * @brief Check if PC is initialized correctly
 *
 */
TEST_F(ProcessorTest, Init_PC) {
  EXPECT_EQ(*proc.PC, 0xf842) << "PC not initialized correctly";
  EXPECT_EQ(*proc.PC, mem.GetUint16(proc.RESET_VECTOR))
      << "PC not initialized correctly";
}

/**
 * @brief Check if status register is initialized correctly
 *
 */
TEST_F(ProcessorTest, Init_SR) {
  EXPECT_EQ(proc.SR->val, 0x0000) << "Status register not initialized properly";
}

TEST_F(ProcessorTest, SetRegisters) {
  uint16_t val = 0xFFFF;

  for (auto reg : proc.register_map) {
    *reg.second = val;
  }

  EXPECT_EQ(*proc.PC, val);
  EXPECT_EQ(*proc.SP, val);
  EXPECT_EQ(proc.SR->val, val);
  EXPECT_EQ(proc.R3, val);
  EXPECT_EQ(proc.R4, val);
  EXPECT_EQ(proc.R5, val);
  EXPECT_EQ(proc.R6, val);
  EXPECT_EQ(proc.R7, val);
  EXPECT_EQ(proc.R8, val);
  EXPECT_EQ(proc.R9, val);
  EXPECT_EQ(proc.R10, val);
  EXPECT_EQ(proc.R11, val);
  EXPECT_EQ(proc.R12, val);
  EXPECT_EQ(proc.R13, val);
  EXPECT_EQ(proc.R14, val);
  EXPECT_EQ(proc.R15, val);
}

/*
    As/Ad   Addressing Mode
    00/0    Register
    01/1    Indexed
    01/1    Absolute
    10/-    Indirect Register
    11/-    Indirect Autoincrement
    11/-    Immediate
*/

/**
 * @brief Set instruction with correct endianess swap
 *
 * @param instruction
 */
void ProcessorTest::SetInstruction(uint16_t instruction) {
  proc.mem->SetUint16(*proc.PC, __bswap_16(instruction));
}

/**
 * @brief Ensure constant generator works under all six conditions
 *
 */
TEST_F(ProcessorTest, ConstantGenerator) {
  auto instruction = Processor::Format1();
  instruction.op_code = 5;    // Add Op-Cpde
  instruction.s_reg = 2;      // R2 Source Register
  instruction.byte_word = 0;  // Word Operation
  instruction.d_reg = 4;      // R4 Destination Register
  instruction.ad = 0;
  proc.R4 = 0;

  // No Constant
  instruction.as = 0b00;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, false)
      << "No Constant Failed" << std::endl;

  // 0x4 Constant
  instruction.as = 0b01;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, false)
      << "No Constant Failed" << std::endl;

  // 0x4 Constant
  instruction.as = 0b10;
  SetInstruction(instruction.val);
  proc.R4 = 0;
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, true)
      << "0x4 Constant Not Generated" << std::endl;
  EXPECT_EQ(proc.R4, 4) << "0x4 Constant Failed" << std::endl;

  // 0x8 Constant
  instruction.as = 0b11;
  proc.R4 = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, true)
      << "0x8 Constant Not Generated" << std::endl;
  EXPECT_EQ(proc.R4, 8) << "0x8 Constant Failed" << std::endl;

  // 0x0 Constant
  instruction.s_reg = 3;  // R3 Source Register
  instruction.as = 0b00;
  proc.R4 = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, true)
      << "0x0 Constant Not Generated" << std::endl;
  EXPECT_EQ(proc.R4, 0) << "0x0 Constant Failed" << std::endl;

  // 0x1 Constant
  instruction.as = 0b01;
  proc.R4 = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, true)
      << "0x1 Constant Not Generated" << std::endl;
  EXPECT_EQ(proc.R4, 1) << "0x1 Constant Failed" << std::endl;

  // 0x2 Constant
  instruction.as = 0b10;
  proc.R4 = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, true)
      << "0x2 Constant Not Generated" << std::endl;
  EXPECT_EQ(proc.R4, 2) << "0x2 Constant Failed" << std::endl;

  // 0xFFFF Constant
  instruction.as = 0b11;
  proc.R4 = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, true)
      << "0xFFFF Constant Not Generated" << std::endl;
  EXPECT_EQ(proc.R4, 0xffff) << "0xFFFF Constant Failed" << std::endl;
}

/**
 * @brief Make sure operands are under correct register mode ad/as
 *
 */
TEST_F(ProcessorTest, OpCode_RegMode) {
  auto instruction = Processor::Format1();
  instruction.op_code = 5;    // Add Op-Cpde
  instruction.s_reg = 3;      // R3 Source Register
  instruction.byte_word = 0;  // Word Operation
  instruction.d_reg = 4;      // R4 Destination Register
  instruction.as = 0;

  // Check AD Register Mode
  instruction.ad = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_ad_mode, ADDRESSING_MODE::REGISTER)
      << "AD was not REGISTER mode";

  // Check AD Indexed/Symbolic/Absolute Mode
  instruction.ad = 1;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_ad_mode, ADDRESSING_MODE::INDEXED)
      << "AD was not INDEXED mode";

  // Check AS Register Mode
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::REGISTER)
      << "AS was not REGISTER mode";

  // Check AS Indexed/Symbolic/Absolute Mode
  instruction.as = 0b01;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::INDEXED)
      << "AS was not INDEXED mode";

  // Check AS Indirect Register Mode
  instruction.as = 0b10;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::INDIRECT_REG)
      << "AS was not INDIRECT REGISTER mode";

  // Check AS Indirect Auto Mode
  instruction.as = 0b11;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::INDIRECT_AUTO)
      << "AS was not INDIRECT AUTO mode";
}

/**
 * @brief Test ADD OpCpde
 *
 */
TEST_F(ProcessorTest, OpAdd) {
  auto instruction = Processor::Format1();
  instruction.op_code = 5;    // Add Op-Cpde
  instruction.s_reg = 4;      // R3 Source Register
  instruction.byte_word = 0;  // Word Operation
  instruction.d_reg = 5;      // R4 Destination Register
  proc.R4 = 5;
  proc.R5 = 4;

  // Check AS/AD Register Mode
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_opcode, OPCODES::ADD);
  EXPECT_EQ(proc.current_ad_mode, ADDRESSING_MODE::REGISTER);
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::REGISTER);
  EXPECT_EQ(proc.R5, 9) << "ADD Register Mode Failed";

  // Check AS/AD Indexed Mode
  instruction.ad = 1;
  instruction.as = 0b01;
  proc.R4 = 0x1000;
  proc.R5 = 0x2000;
  auto src_offset = 0x10;
  auto dst_offset = 0x20;
  proc.mem->SetUint16(proc.R4 + src_offset, __bswap_16(0x50));
  proc.mem->SetUint16(proc.R5 + dst_offset, __bswap_16(0x100));
  SetInstruction(instruction.val);
  proc.mem->SetUint16(*proc.PC + 2, __bswap_16(src_offset));
  proc.mem->SetUint16(*proc.PC + 4, __bswap_16(dst_offset));
  proc.Step();
  EXPECT_EQ(proc.current_opcode, OPCODES::ADD);
  EXPECT_EQ(proc.current_ad_mode, ADDRESSING_MODE::INDEXED);
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::INDEXED);
  EXPECT_EQ(proc.R4, 0x1000);
  EXPECT_EQ(proc.R5, 0x2000);
  EXPECT_EQ(proc.mem->GetUint16(0x2020), 0x150) << "ADD Indexed Mode Failed";

  // Check AS/AD Indirect Register Mode
  instruction.ad = 0;
  instruction.as = 0b10;
  proc.R4 = 0x1000;
  proc.R5 = 0x200;
  proc.mem->SetUint16BSwap(proc.R4, 0x50);
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_opcode, OPCODES::ADD);
  EXPECT_EQ(proc.current_ad_mode, ADDRESSING_MODE::REGISTER);
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::INDIRECT_REG);
  EXPECT_EQ(proc.R4, 0x1000);
  EXPECT_EQ(proc.R5, 0x250);
  EXPECT_EQ(proc.mem->GetUint16(0x1000), 0x50)
      << "ADD Indirect Register Mode Failed";

  // Check AS/AD Auto Increment
  instruction.ad = 0;
  instruction.as = 0b11;
  proc.R4 = 0x1000;
  proc.R5 = 0x200;
  proc.mem->SetUint16BSwap(proc.R4, 0x50);
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_opcode, OPCODES::ADD);
  EXPECT_EQ(proc.current_ad_mode, ADDRESSING_MODE::REGISTER);
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::INDIRECT_AUTO);
  EXPECT_EQ(proc.R4, 0x1002) << "ADD Indirect Autoincrement Mode Failed";
  EXPECT_EQ(proc.R5, 0x250);

  // Check AS/AD Auto Increment
  instruction.ad = 0;
  instruction.as = 0b11;
  instruction.s_reg = 0;  // PC Source Register
  proc.R5 = 0x200;
  proc.mem->SetUint16(proc.R0 + 2, __bswap_16(0x100));
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.current_opcode, OPCODES::ADD);
  EXPECT_EQ(proc.current_ad_mode, ADDRESSING_MODE::REGISTER);
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::INDIRECT_AUTO);
  EXPECT_EQ(proc.R5, 0x300) << "ADD Immediate Mode Failed";

  // Check ADD with constant generator
  instruction.s_reg = 2;      // R2 Source Register
  instruction.byte_word = 0;  // Word Operation
  instruction.d_reg = 4;      // R4 Destination Register
  proc.R3 = 10;
  proc.R4 = 5;
  instruction.ad = 0;
  instruction.as = 0b11;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.const_generator_used, true);
  EXPECT_EQ(proc.current_opcode, OPCODES::ADD);
  EXPECT_EQ(proc.current_ad_mode, ADDRESSING_MODE::REGISTER);
  EXPECT_EQ(proc.current_as_mode, ADDRESSING_MODE::INDIRECT_AUTO);
  EXPECT_EQ(proc.R4, 13) << "Constant Generator Failed";
}

TEST_F(ProcessorTest, OpAdd_Flags_Word) {
  auto instruction = Processor::Format1();
  instruction.op_code = 5;    // Add Op-Cpde
  instruction.s_reg = 4;      // R3 Source Register
  instruction.byte_word = 0;  // Word Operation
  instruction.d_reg = 5;      // R4 Destination Register
  proc.R4 = 0;
  proc.R5 = 0;

  // Check Zero Flag
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->zero, 1);

  // Check Negative Flag
  proc.R4 = 0x8000;
  proc.R5 = 0;
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->negative, 1);
  EXPECT_EQ(proc.SR->zero, 0);

  // Check Overflow Flag
  proc.R4 = 0x7000;
  proc.R5 = 0x1000;
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->overflow, 1);
  EXPECT_EQ(proc.SR->negative, 1);
  EXPECT_EQ(proc.SR->zero, 0);

  // Check Overflow Flag
  proc.R4 = 0xF000;
  proc.R5 = 0x8000;
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->overflow, 1);
  EXPECT_EQ(proc.SR->zero, 0);

  // Check Carry Flag
  proc.R4 = 0xF000;
  proc.R5 = 0x1000;
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->overflow, 0);
  EXPECT_EQ(proc.SR->carry, 1);
}

TEST_F(ProcessorTest, OpAdd_Flags_Byte) {
  auto instruction = Processor::Format1();
  instruction.op_code = 5;    // Add Op-Cpde
  instruction.s_reg = 4;      // R3 Source Register
  instruction.byte_word = 1;  // Word Operation
  instruction.d_reg = 5;      // R4 Destination Register
  proc.R4 = 0;
  proc.R5 = 0;

  // Check Zero Flag
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->zero, 1);

  // Check Negative Flag
  proc.R4 = 0x80;
  proc.R5 = 0;
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->negative, 1);
  EXPECT_EQ(proc.SR->zero, 0);

  // Check Overflow Flag
  proc.R4 = 0x70;
  proc.R5 = 0x10;
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->overflow, 1);
  EXPECT_EQ(proc.SR->negative, 1);
  EXPECT_EQ(proc.SR->zero, 0);

  // Check Overflow Flag
  proc.R4 = 0xF0;
  proc.R5 = 0x80;
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->overflow, 1);
  EXPECT_EQ(proc.SR->zero, 0);

  // Check Carry Flag
  proc.R4 = 0xF0;
  proc.R5 = 0x10;
  instruction.ad = 0;
  instruction.as = 0;
  SetInstruction(instruction.val);
  proc.Step();
  EXPECT_EQ(proc.SR->overflow, 0);
  EXPECT_EQ(proc.SR->carry, 1);
}

TEST_F(ProcessorTest, Step) { proc.Step(); }

TEST_F(ProcessorTest, Cycle) { proc.Cycle(); }