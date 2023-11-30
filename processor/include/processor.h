#ifndef processor_h
#define processor_h

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>

#include "memory.h"

enum class FORMAT { FORMAT1, FORMAT2, JUMP, NONE };

enum class REG { AS, AD, NONE };

enum class ADDRESSING_MODE {
  REGISTER,
  INDEXED,
  SYMBOLIC,
  ABSOLUTE,
  INDIRECT_REG,
  INDIRECT_AUTO,
  IMMEDIATE,
  NONE
};

enum class OPCODES {
  ADD,
  ADDC,
  AND,
  BIC,
  BIS,
  BIT,
  CALL,
  CMP,
  DADD,
  JC,
  JL,
  JN,
  JEQ,
  JGE,
  JMP,
  JNC,
  JNE,
  MOV,
  SUB,
  SUBC,
  XOR
};

class Processor {
  typedef void (Processor::*OP)();

 public:
  Processor();
  ~Processor();

  void SetMemory(Memory* mem);
  void Step();
  uint16_t FetchInstruction(uint16_t PC);
  ADDRESSING_MODE GetAddressingMode(REG reg, uint8_t ax);

  uint16_t GetAsAdFormat2(uint16_t reg,
                          std::optional<uint16_t>& destination_mem, bool byte);

  std::pair<uint16_t, uint16_t> GetAsAd(
      uint16_t src_reg, uint16_t dst_reg, std::optional<uint16_t>& source_mem,
      std::optional<uint16_t>& destination_mem, bool byte);

  OP GetOpCodeFunc();
  OP GetOp();
  std::string GetModeString(ADDRESSING_MODE addr);
  bool CheckConstantGenerator(uint16_t reg_num, uint16_t as, uint16_t* val);
  void Cycle();

  Memory* mem;
  FORMAT current_format{FORMAT::NONE};

  uint16_t R0{};
  uint16_t R1{};
  union StatusRegister {
    struct {
      uint8_t carry : 1;
      uint8_t zero : 1;
      uint8_t negative : 1;
      uint8_t general_int_en : 1;
      uint8_t cpu_off : 1;
      uint8_t osc_off : 1;
      uint8_t generator_0 : 1;
      uint8_t generator_1 : 1;
      uint8_t overflow : 1;
      uint8_t : 7;
    };
    uint16_t val;
  };
  StatusRegister R2{};
  uint16_t R3{};
  uint16_t R4{};
  uint16_t R5{};
  uint16_t R6{};
  uint16_t R7{};
  uint16_t R8{};
  uint16_t R9{};
  uint16_t R10{};
  uint16_t R11{};
  uint16_t R12{};
  uint16_t R13{};
  uint16_t R14{};
  uint16_t R15{};

  uint16_t* PC;
  uint16_t* SP;
  StatusRegister* SR;
  uint16_t* GC1;
  uint16_t* GC2;
  uint16_t current_instruction{};

  void SetFlags(uint16_t src, uint16_t dst, uint16_t val, bool byte);
  void SetFlagsXOR(uint16_t src, uint16_t dst, uint16_t val, bool byte);
  void WriteToMemory(uint16_t mem_addr, uint16_t val, bool byte);
  void WriteToRegister(uint16_t reg, uint16_t val, bool byte);
  void PrintStatusRegister();
  void DisplayInstruction(MemAddr addr);
  bool DisplayVerbose();
  std::string GetOperandString(std::optional<uint16_t> source_mem,
                               uint16_t src);

  // op codes
  void op_add();
  void op_addc();
  void op_and();
  void op_bic();
  void op_bis();
  void op_bit();
  void op_call();
  void op_cmp();
  void op_dadd();
  void op_jc_jhs();
  void op_jeq_jz();
  void op_jge();
  void op_jle();
  void op_jmp();
  void op_jn();
  void op_jnc_jlo();
  void op_jne_jnz();
  void op_mov();
  void op_push();
  void op_push_b();
  void op_reti();
  void op_rra();
  void op_rra_b();
  void op_rrc();
  void op_rrc_b();
  void op_sub();
  void op_subc();
  void op_swpb();
  void op_sxt();
  void op_xor();

  // interrupts
  void int_reset();

  union Format1 {
    struct {
      uint8_t d_reg : 4;
      uint8_t as : 2;
      uint8_t byte_word : 1;
      uint8_t ad : 1;
      uint8_t s_reg : 4;
      uint8_t op_code : 4;
    };
    uint16_t val;
  };

  union Format2 {
    struct {
      uint8_t ds_reg : 4;
      uint8_t ad : 2;
      uint8_t byte_word : 1;
      uint16_t op_code : 9;
    };
    uint16_t val;
  };

  union Jump {
    struct {
      int16_t offset : 10;
      uint8_t c : 3;
      uint8_t op_code : 3;
    };
    uint16_t val;
  };

  std::map<std::string, OP> op_map;
  std::map<uint16_t, uint16_t*> register_map{};

  static constexpr uint16_t RESET_VECTOR = 0xFFFE;
  static constexpr uint16_t PERIPH_MAX = 0x01FF;

  ADDRESSING_MODE current_as_mode{};
  ADDRESSING_MODE current_ad_mode{};
  OPCODES current_opcode{};
  bool const_generator_used{false};
  uint16_t const_generator_val{};
  bool display_instruction{false};
  bool step{false};
  bool no_increment{false};
};

class ProcessorException : public std::exception {
  std::string _msg;

 public:
  ProcessorException(const std::string& msg) : _msg(msg) {}

  virtual const char* what() const noexcept override { return _msg.c_str(); }
};

#endif