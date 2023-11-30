#include "debugger_test.h"

/**
 * @brief Load MSP430 Binary Into Memory
 *
 */
void DebuggerTest::SetUp() { debug.LoadMem(DOCUMENT_PATH); }

/**
 * @brief Check if PC is initialized correctly
 *
 */
TEST_F(DebuggerTest, Init_PC) {
  auto PC = debug.GetPC();
  auto RESET = debug.GetResetAddress();
  EXPECT_EQ(PC, 0xf842) << "PC not initialized correctly";
  EXPECT_EQ(PC, RESET) << "PC does not equal RESET";
}

TEST_F(DebuggerTest, DisplayRegisters) { debug.DisplayRegisters(); }

TEST_F(DebuggerTest, DisplayInstruction) { debug.DisplayInstruction(0xf834); }

TEST_F(DebuggerTest, Step) {
  // Initialize Stack Pointer
  debug.Step();
  EXPECT_EQ(debug.GetSP(), 0x280);

  // Call #system.pre.init
  auto return_val = debug.GetPC() + 4;
  debug.Step();
  EXPECT_EQ(debug.GetPC(), 0xf864);
  EXPECT_EQ(debug.GetSP(), 0x280 - 2);
  EXPECT_EQ(debug.GetMemory(debug.GetSP()), return_val);

  // MOV 1, R12
  debug.Step();
  EXPECT_EQ(debug.GetRegister(12), 1);

  // RET
  debug.Step();
  EXPECT_EQ(debug.GetSP(), 0x280);
  EXPECT_EQ(debug.GetPC(), 0xf84a);

  // MOV 0, R12
  debug.Step();
  EXPECT_EQ(debug.GetRegister(12), 0);

  // Call Main
  debug.Step();
  EXPECT_EQ(debug.GetPC(), 0xf800);
  EXPECT_EQ(debug.GetSP(), 0x280 - 2);
  EXPECT_EQ(debug.GetMemory(debug.GetSP()), 0xf850);

  // DECD SP
  debug.Step();
  EXPECT_EQ(debug.GetSP(), 0x27C);

  // Stop Watchdog Timer
  debug.Step();
  EXPECT_EQ(debug.GetMemory(static_cast<uint16_t>(0x120)), 0x5a80);

  // Set System Clock BCSCTL1
  debug.Step();

  // Set System Clock DCOCTL
  debug.Step();

  // BIS.B #1, M[0x22]
  debug.Step();
  EXPECT_EQ(debug.GetMemory(static_cast<uint8_t>(0x22)), 1);

  // XOR P1OUT
  debug.Step();
  EXPECT_EQ(debug.GetMemory(static_cast<uint8_t>(0x21)), 1);

  // Clear SP
  debug.Step();
  EXPECT_EQ(debug.GetMemory(debug.GetSP()), 0x0);

  //
  // debug.Step();
}