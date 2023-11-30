#include "processor.h"

#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <thread>

using namespace std::literals;
using clock_type = std::chrono::high_resolution_clock;

Processor::Processor() {
  PC = &R0;
  SP = &R1;
  SR = &R2;
  GC1 = &R2.val;
  GC2 = &R3;

  register_map.emplace(0, &R0);
  register_map.emplace(1, &R1);
  register_map.emplace(2, &R2.val);
  register_map.emplace(3, &R3);
  register_map.emplace(4, &R4);
  register_map.emplace(5, &R5);
  register_map.emplace(6, &R6);
  register_map.emplace(7, &R7);
  register_map.emplace(8, &R8);
  register_map.emplace(9, &R9);
  register_map.emplace(10, &R10);
  register_map.emplace(11, &R11);
  register_map.emplace(12, &R12);
  register_map.emplace(13, &R13);
  register_map.emplace(14, &R14);
  register_map.emplace(15, &R15);

  op_map.emplace("rrc", &Processor::op_rrc);
  op_map.emplace("swpb", &Processor::op_swpb);
  op_map.emplace("rra", &Processor::op_rra);
  op_map.emplace("sxt", &Processor::op_sxt);
  op_map.emplace("push", &Processor::op_push);
  op_map.emplace("call", &Processor::op_call);
  op_map.emplace("reti", &Processor::op_reti);
  op_map.emplace("jne", &Processor::op_jne_jnz);
  op_map.emplace("jeq", &Processor::op_jeq_jz);
  op_map.emplace("jnc", &Processor::op_jnc_jlo);
  op_map.emplace("jc", &Processor::op_jc_jhs);
  op_map.emplace("jn", &Processor::op_jn);
  op_map.emplace("jge", &Processor::op_jge);
  op_map.emplace("jle", &Processor::op_jle);
  op_map.emplace("jmp", &Processor::op_jmp);
  op_map.emplace("mov", &Processor::op_mov);
  op_map.emplace("add", &Processor::op_add);
  op_map.emplace("addc", &Processor::op_addc);
  op_map.emplace("subc", &Processor::op_subc);
  op_map.emplace("sub", &Processor::op_sub);
  op_map.emplace("cmp", &Processor::op_cmp);
  op_map.emplace("dadd", &Processor::op_dadd);
  op_map.emplace("bit", &Processor::op_bit);
  op_map.emplace("bic", &Processor::op_bic);
  op_map.emplace("bis", &Processor::op_bis);
  op_map.emplace("xor", &Processor::op_xor);
  op_map.emplace("and", &Processor::op_and);
}

void Processor::SetMemory(Memory* mem_ptr) {
  mem = mem_ptr;
  int_reset();
}

bool CheckBits(uint16_t a, uint16_t b, uint16_t bit) {
  auto v1 = (a >> bit) & 0xF;
  auto v2 = (b >> bit) & 0xF;
  return (v1 & v2);
}

Processor::OP Processor::GetOpCodeFunc() {
  auto opcode = static_cast<uint8_t>(current_instruction >> 12);
  if (opcode == 0xF) {
    return op_map["and"];
  }
  if (opcode == 0xE) {
    return op_map["xor"];
  }
  if (opcode == 0xD) {
    return op_map["bis"];
  }
  if (opcode == 0xC) {
    return op_map["bic"];
  }
  if (opcode == 0xB) {
    return op_map["bit"];
  }
  if (opcode == 0xA) {
    return op_map["dadd"];
  }
  if (opcode == 0x9) {
    return op_map["cmp"];
  }
  if (opcode == 0x8) {
    return op_map["sub"];
  }
  if (opcode == 0x7) {
    return op_map["subc"];
  }
  if (opcode == 0x6) {
    return op_map["addc"];
  }
  if (opcode == 0x5) {
    return op_map["add"];
  }
  if (opcode == 0x4) {
    return op_map["mov"];
  }
  if (opcode == 0x3) {
    auto opcode_sec = (current_instruction >> 8) & 0x0F;
    if ((opcode_sec & 0x8) && (opcode_sec & 0x4)) {
      return op_map["jmp"];
    }
    if (opcode_sec & 0x8) {
      return op_map["jle"];
    }
    if (opcode_sec & 0x4) {
      return op_map["jge"];
    }
    if (opcode_sec & 0x0) {
      return op_map["jn"];
    }
  }
  if (opcode == 0x2) {
    auto opcode_sec = (current_instruction >> 8) & 0x0F;
    if ((opcode_sec & 0x8) && (opcode_sec & 0x4)) {
      return op_map["jc"];
    }
    if (opcode_sec & 0x8) {
      return op_map["jnc"];
    }
    if (opcode_sec & 0x4) {
      return op_map["jeq"];
    }
    if (opcode_sec & 0x0) {
      return op_map["jne"];
    }
  }
  if (opcode == 0x1) {
    auto val = current_instruction & 0x0FFF;
    if (val >= 0x300) {
      return op_map["reti"];
    }
    if (val >= 0x280) {
      return op_map["call"];
    }
    if (val >= 0x200) {
      return op_map["push"];
    }
    if (val >= 0x180) {
      return op_map["sxt"];
    }
    if (val >= 0x100) {
      return op_map["rra"];
    }
    if (val >= 0x080) {
      return op_map["swpb"];
    }
    if (val >= 0x0) {
      return op_map["rrc"];
    }
  }
  if (opcode == 0x0) {
    std::cout << std::hex << +opcode << std::endl;
    throw(ProcessorException("Undefined Opcode"));
  }
  std::cout << std::hex << +opcode << std::endl;
  throw(ProcessorException("Undefined Opcode"));
}

void PrintTime() {
  std::chrono::time_point<std::chrono::system_clock> now =
      std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();

  typedef std::chrono::duration<
      int,
      std::ratio_multiply<std::chrono::hours::period, std::ratio<8> >::type>
      Days; /* UTC: +8:00 */

  Days days = std::chrono::duration_cast<Days>(duration);
  duration -= days;
  auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
  duration -= hours;
  auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
  duration -= minutes;
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
  duration -= seconds;
  auto milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration);
  duration -= milliseconds;
  auto microseconds =
      std::chrono::duration_cast<std::chrono::microseconds>(duration);
  duration -= microseconds;
  auto nanoseconds =
      std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

  std::cout << std::dec << hours.count() << ":" << minutes.count() << ":"
            << seconds.count() << ":" << milliseconds.count() << ":"
            << microseconds.count() << ":" << nanoseconds.count() << std::endl;
}

void Processor::Cycle() {
  std::cout << "CYCLE" << std::endl;
  Step();
}

void Processor::Step() {
  current_instruction = FetchInstruction(*PC);

  (this->*GetOpCodeFunc())();

  if (!no_increment) {
    *PC += 2;
  }

  no_increment = false;
}

uint16_t Processor::FetchInstruction(uint16_t PC) {
  if (PC <= PERIPH_MAX) {
    throw(ProcessorException(
        "Tried to fetch instruction from peripheral address space"));
  }
  return (mem->GetUint16(PC));
}

void Processor::int_reset() {
  // Configure RST/NMI pin
  // Switch IO pins to input mode
  // Initialize peripherals

  // Set status register
  SR->val = 0;

  // Reset Program Counter
  *PC = mem->GetUint16(RESET_VECTOR);
}

ADDRESSING_MODE Processor::GetAddressingMode(REG reg, uint8_t ax) {
  if (reg == REG::AS) {
    if (ax == 0b00) {
      return ADDRESSING_MODE::REGISTER;
    }
    if (ax == 0b01) {
      return ADDRESSING_MODE::INDEXED;
    }
    if (ax == 0b01) {
      return ADDRESSING_MODE::SYMBOLIC;
    }
    if (ax == 0b01) {
      return ADDRESSING_MODE::ABSOLUTE;
    }
    if (ax == 0b10) {
      return ADDRESSING_MODE::INDIRECT_REG;
    }
    if (ax == 0b11) {
      return ADDRESSING_MODE::INDIRECT_AUTO;
    }
    if (ax == 0b11) {
      return ADDRESSING_MODE::IMMEDIATE;
    }
  }

  if (reg == REG::AD) {
    if (ax == 0b0) {
      return ADDRESSING_MODE::REGISTER;
    }
    if (ax == 0b1) {
      return ADDRESSING_MODE::INDEXED;
    }
    if (ax == 0b1) {
      return ADDRESSING_MODE::SYMBOLIC;
    }
    if (ax == 0b1) {
      return ADDRESSING_MODE::ABSOLUTE;
    }
  }
  return ADDRESSING_MODE::NONE;
}

bool Processor::CheckConstantGenerator(uint16_t reg, uint16_t as,
                                       uint16_t* val) {
  if ((reg == 2) && (as == 0b00)) {
    return false;
  }
  if ((reg == 2) && (as == 0b01)) {
    return false;
  }
  if ((reg == 2) && (as == 0b10)) {
    *val = 4;
    return true;
  }
  if ((reg == 2) && (as == 0b11)) {
    *val = 8;
    return true;
  }
  if ((reg == 3) && (as == 0b00)) {
    *val = 0;
    return true;
  }
  if ((reg == 3) && (as == 0b01)) {
    *val = 1;
    return true;
  }
  if ((reg == 3) && (as == 0b10)) {
    *val = 2;
    return true;
  }
  if ((reg == 3) && (as == 0b11)) {
    *val = 0xFFFF;
    return true;
  }
  return false;
}

uint16_t Processor::GetAsAdFormat2(uint16_t reg,
                                   std::optional<uint16_t>& destination_mem,
                                   bool byte) {
  uint8_t ad{};
  uint16_t ad_val{};

  bool constant_generator = false;
  destination_mem = {};
  uint16_t constant;

  if (current_format == FORMAT::FORMAT2) {
    auto instruction = Format2();
    instruction.val = current_instruction;
    ad = instruction.ad;

    if (instruction.ds_reg == 2) {
      constant_generator = CheckConstantGenerator(2, ad, &constant);
    } else if (instruction.ds_reg == 3) {
      constant_generator = CheckConstantGenerator(3, ad, &constant);
    }
    const_generator_used = constant_generator;
  } else {
    return {};
  }

  auto ad_mode = GetAddressingMode(REG::AS, ad);

  if (constant_generator) {
    ad_val = constant;
    const_generator_val = constant;
  } else if (ad_mode == ADDRESSING_MODE::REGISTER) {
    ad_val = *register_map[reg];
  } else if ((ad_mode == ADDRESSING_MODE::INDEXED) ||
             (ad_mode == ADDRESSING_MODE::SYMBOLIC) ||
             (ad_mode == ADDRESSING_MODE::ABSOLUTE)) {
    *PC += 2;
    ad_val = mem->GetUint16(*PC);
    ad_val = mem->GetUint16(ad_val + *register_map[reg]);
  } else if (ad_mode == ADDRESSING_MODE::INDIRECT_REG) {
    ad_val = mem->GetUint16(*register_map[reg]);
  } else if ((ad_mode == ADDRESSING_MODE::INDIRECT_AUTO) ||
             (ad_mode == ADDRESSING_MODE::IMMEDIATE)) {
    if (reg == 0) {
      *PC += 2;
      ad_val = mem->GetUint16(*PC);
    } else {
      ad_val = mem->GetUint16(*register_map[reg]);
      if (byte) {
        *register_map[reg] += 1;
      } else {
      }
    }
  }
  return ad_val;
}

std::pair<uint16_t, uint16_t> Processor::GetAsAd(
    uint16_t src_reg, uint16_t dst_reg, std::optional<uint16_t>& source_mem,
    std::optional<uint16_t>& destination_mem, bool byte) {
  uint8_t as{};
  uint8_t ad{};

  uint16_t as_val{};
  uint16_t ad_val{};

  bool constant_generator = false;
  destination_mem = {};
  source_mem = {};
  uint16_t constant;

  if (current_format == FORMAT::FORMAT1) {
    auto instruction = Format1();
    instruction.val = current_instruction;
    as = instruction.as;
    ad = instruction.ad;

    if (instruction.s_reg == 2) {
      constant_generator = CheckConstantGenerator(2, as, &constant);
    } else if (instruction.s_reg == 3) {
      constant_generator = CheckConstantGenerator(3, as, &constant);
    }

    const_generator_used = constant_generator;

  } else if (current_format == FORMAT::FORMAT2) {
    auto instruction = Format2();
    instruction.val = current_instruction;
    as = 0;
    ad = instruction.ad;
  } else {
    return {};
  }
  auto as_mode = GetAddressingMode(REG::AS, as);
  auto ad_mode = GetAddressingMode(REG::AD, ad);

  current_ad_mode = ad_mode;
  current_as_mode = as_mode;

  if (constant_generator) {
    as_val = constant;
    const_generator_val = constant;
  } else if (as_mode == ADDRESSING_MODE::REGISTER) {
    as_val = *register_map[src_reg];
  } else if ((as_mode == ADDRESSING_MODE::INDEXED) ||
             (as_mode == ADDRESSING_MODE::SYMBOLIC) ||
             (as_mode == ADDRESSING_MODE::ABSOLUTE)) {
    uint16_t offset;
    if ((src_reg == 2) && (as == 1)) {
      offset = 0;
    } else {
      offset = *register_map[src_reg];
    }

    *PC += 2;
    as_val = mem->GetUint16(*PC);
    source_mem = as_val + offset;
    as_val = mem->GetUint16(as_val + offset);

  } else if (as_mode == ADDRESSING_MODE::INDIRECT_REG) {
    as_val = mem->GetUint16(*register_map[src_reg]);
  } else if ((as_mode == ADDRESSING_MODE::INDIRECT_AUTO) ||
             (as_mode == ADDRESSING_MODE::IMMEDIATE)) {
    if (src_reg == 0) {
      *PC += 2;
      as_val = mem->GetUint16(*PC);
    } else {
      as_val = mem->GetUint16(*register_map[src_reg]);
      if (byte) {
        *register_map[src_reg] += 1;
      } else {
        *register_map[src_reg] += 2;
      }
    }
  }

  if ((ad_mode == ADDRESSING_MODE::INDEXED) ||
      (ad_mode == ADDRESSING_MODE::SYMBOLIC) ||
      (ad_mode == ADDRESSING_MODE::ABSOLUTE) ||
      (ad_mode == ADDRESSING_MODE::IMMEDIATE) ||
      (ad_mode == ADDRESSING_MODE::INDIRECT_AUTO)) {
    *PC += 2;
    ad_val = mem->GetUint16(*PC);
    uint16_t offset;
    if ((dst_reg == 2) && (ad == 1)) {
      offset = 0;
    } else {
      offset = *register_map[dst_reg];
    }
    destination_mem = ad_val + offset;
    ad_val = mem->GetUint16(ad_val + offset);

  } else {
    ad_val = *register_map[dst_reg];
  }
  return std::pair<uint16_t, uint16_t>(as_val, ad_val);
}

std::string Processor::GetModeString(ADDRESSING_MODE addr) {
  if (addr == ADDRESSING_MODE::ABSOLUTE) {
    return "ABSOLUTE";
  }
  if (addr == ADDRESSING_MODE::IMMEDIATE) {
    return "IMMEDIATE";
  }
  if (addr == ADDRESSING_MODE::INDEXED) {
    return "INDEXED";
  }
  if (addr == ADDRESSING_MODE::INDIRECT_AUTO) {
    return "INDIRECT_AUTO";
  }
  if (addr == ADDRESSING_MODE::INDIRECT_REG) {
    return "INDIRECT_REG";
  }
  if (addr == ADDRESSING_MODE::REGISTER) {
    return "REGISTER";
  }
  if (addr == ADDRESSING_MODE::SYMBOLIC) {
    return "SYMBOLIc";
  }

  return "NONE";
}

void Processor::DisplayInstruction(MemAddr addr) {
  display_instruction = true;
  current_instruction = FetchInstruction(addr);
  (this->*GetOpCodeFunc())();
  display_instruction = false;
}

void Processor::PrintStatusRegister() {
  std::cout << "Overflow: " << +SR->overflow << " Carry: " << +SR->carry
            << " Negative: " << +SR->negative << " Zero: " << +SR->zero
            << std::endl;
}

bool Processor::DisplayVerbose() { return step || display_instruction; }
// bool Processor::DisplayVerbose() { return false;}

Processor::~Processor() {}