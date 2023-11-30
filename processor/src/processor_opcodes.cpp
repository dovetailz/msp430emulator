#include "processor.h"

void Processor::WriteToMemory(uint16_t mem_addr, uint16_t val, bool byte) {
  if (byte) {
    mem->SetUint8(mem_addr, val);
  } else {
    mem->SetUint16BSwap(mem_addr, val);
  }
}

void Processor::WriteToRegister(uint16_t reg, uint16_t val, bool byte) {
  if (byte) {
    *register_map[reg] = static_cast<uint8_t>(val);
  } else {
    *register_map[reg] = val;
  }
}

void Processor::SetFlagsXOR(uint16_t src, uint16_t dst, uint16_t val,
                            bool byte) {
  SR->zero = 0;
  SR->negative = 0;
  SR->overflow = 0;
  SR->carry = 0;

  if (byte) {
    uint8_t src_b = static_cast<uint8_t>(src);
    uint8_t dst_b = static_cast<uint8_t>(dst);
    uint8_t val_b = static_cast<uint8_t>(val);

    // Check for Zero Flag
    if (val_b == 0) {
      SR->zero = 1;
    }

    // Check For Negative Flag
    if (val_b & 0b10000000) {
      SR->negative = 1;
    }

    // Check For Overflow
    uint8_t src_bit = src_b >> 7;
    uint8_t dst_bit = dst_b >> 7;
    uint8_t val_bit = val_b >> 7;
    if ((src_bit == 1) && (dst_bit == 1)) {
      SR->overflow = 1;
    }

    // Check For Carry
    if (val_b != 0) {
      SR->carry = 1;
    }

  } else {
    // Check for Zero Flag
    if (val == 0) {
      SR->zero = 1;
    }

    // Check For Negative Flag
    if (val & 0x8000) {
      SR->negative = 1;
    }

    // Check For Overflow
    uint16_t src_bit = src >> 15;
    uint16_t dst_bit = dst >> 15;
    uint16_t val_bit = val >> 15;

    if ((src_bit == 1) && (dst_bit == 1)) {
      SR->overflow = 1;
    }
    // Check for Carry
    if (val != 0) {
      SR->carry = 1;
    }
  }
}

void Processor::SetFlags(uint16_t src, uint16_t dst, uint16_t val, bool byte) {
  uint32_t carry = src + dst;

  SR->zero = 0;
  SR->negative = 0;
  SR->overflow = 0;
  SR->carry = 0;

  if (byte) {
    uint8_t src_b = static_cast<uint8_t>(src);
    uint8_t dst_b = static_cast<uint8_t>(dst);
    uint8_t val_b = static_cast<uint8_t>(val);

    // Check for Zero Flag
    if (val_b == 0) {
      SR->zero = 1;
    }

    // Check For Negative Flag
    if (val_b & 0b10000000) {
      SR->negative = 1;
    }

    // Check For Overflow
    uint8_t src_bit = src_b >> 7;
    uint8_t dst_bit = dst_b >> 7;
    uint8_t val_bit = val_b >> 7;
    if ((src_bit == 1) && (dst_bit == 1) && (val_bit == 0)) {
      SR->overflow = 1;
    }
    if ((src_bit == 0) && (dst_bit == 0) && (val_bit == 1)) {
      SR->overflow = 1;
    }

    // Check For Carry
    if (carry & 0x100) {
      SR->carry = 1;
    }

  } else {
    // Check for Zero Flag
    if (val == 0) {
      SR->zero = 1;
    }

    // Check For Negative Flag
    if (val & 0x8000) {
      SR->negative = 1;
    }

    // Check For Overflow
    uint16_t src_bit = src >> 15;
    uint16_t dst_bit = dst >> 15;
    uint16_t val_bit = val >> 15;

    if ((src_bit == 1) && (dst_bit == 1) && (val_bit == 0)) {
      SR->overflow = 1;
    }
    if ((src_bit == 0) && (dst_bit == 0) && (val_bit == 1)) {
      SR->overflow = 1;
    }
    // Check for Carry
    if (carry & 0x10000) {
      SR->carry = 1;
    }

    if(DisplayVerbose()) {
      // printf("SR->zero = %i;SR->negative = %i;SR->overflow = %i;SR->carry = %i;",SR->zero, SR->negative,SR->overflow,SR->carry);
    }
  }
}

std::string Processor::GetOperandString(std::optional<uint16_t> source_mem,
                                        uint16_t src) {
  std::stringstream operand;
  if (source_mem) {
    operand << "M[0x" << std::hex << source_mem.value() << "]";
  } else {
    operand << "R" << std::dec << src;
  }

  return operand.str();
}

/**
 * @brief ADD OP Code
 *
 */
void Processor::op_add() {
  current_opcode = OPCODES::ADD;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  uint16_t val;
  std::optional<uint16_t> source_mem;
  std::optional<uint16_t> destination_mem;

  auto byte = (instruction.byte_word == 1) ? true : false;

  auto next = GetAsAd(instruction.s_reg, instruction.d_reg, source_mem,
                      destination_mem, byte);

  src = &next.first;
  dst = &next.second;
  val = *src + *dst;

  if (DisplayVerbose()) {
    printf("ADD R%i=0x%04x, R%i=0x%04x, As: %i, Ad: %i, ", instruction.s_reg,
           *register_map[instruction.s_reg], instruction.d_reg,
           *register_map[instruction.d_reg], instruction.as, instruction.ad);
    if (byte) {
      std::cout << "BYTE";
    } else {
      std::cout << "WORD";
    }
    if (const_generator_used) {
      printf(", CGVAL: 0x%04x", const_generator_val);
    }
    std::cout << std::endl;
    std::string source = GetOperandString(source_mem, instruction.s_reg);
    std::string destination =
        GetOperandString(destination_mem, instruction.d_reg);
    printf("ADD %s(0x%04x) + %s(0x%04x) = 0x%04x -> %s", source.c_str(), *src,
           destination.c_str(), *dst, val, destination.c_str());
    std::cout << std::endl;
  }

  // Write value to register or memory
  if (destination_mem) {
    WriteToMemory(destination_mem.value(), val, byte);
  } else {
    WriteToRegister(instruction.d_reg, val, byte);
  }

  // Set appropriate status flags
  SetFlags(*src, *dst, val, byte);
};

void Processor::op_addc() { throw(ProcessorException("ADDC Undefined")); };
void Processor::op_and() { throw(ProcessorException("AND Undefined")); };
void Processor::op_bic() { throw(ProcessorException("BIC Undefined")); };

void Processor::op_bis() {
  current_opcode = OPCODES::BIS;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  uint16_t val;
  std::optional<uint16_t> source_mem;
  std::optional<uint16_t> destination_mem;

  auto byte = (instruction.byte_word == 1) ? true : false;

  auto next = GetAsAd(instruction.s_reg, instruction.d_reg, source_mem,
                      destination_mem, byte);

  src = &next.first;
  dst = &next.second;

  val = *src | *dst;

  if (DisplayVerbose()) {
    printf("BIS R%i=0x%04x, R%i=0x%04x, As: %i, Ad: %i, ", instruction.s_reg,
           *register_map[instruction.s_reg], instruction.d_reg,
           *register_map[instruction.d_reg], instruction.as, instruction.ad);
    if (byte) {
      std::cout << "BYTE";
    } else {
      std::cout << "WORD";
    }
    if (const_generator_used) {
      printf(", CGVAL: 0x%04x", const_generator_val);
    }
    std::cout << std::endl;
    std::string source = GetOperandString(source_mem, instruction.s_reg);
    std::string destination =
        GetOperandString(destination_mem, instruction.d_reg);

    printf("BIS %s(0x%04x) OR %s(0x%04x) = 0x%04x -> %s", source.c_str(), *src,
           destination.c_str(), *dst, val, destination.c_str());
    std::cout << std::endl;
  }

  // Write value to register or memory
  if (destination_mem) {
    WriteToMemory(destination_mem.value(), val, byte);
  } else {
    WriteToRegister(instruction.d_reg, val, byte);
  }
};

void Processor::op_bit() { throw(ProcessorException("BIT Undefined")); };

void Processor::op_call() {
  current_opcode = OPCODES::CALL;
  current_format = FORMAT::FORMAT2;
  auto instruction = Format2();
  instruction.val = current_instruction;
  std::optional<uint16_t> destination_mem;

  auto byte = instruction.byte_word == 1 ? true : false;

  auto next = GetAsAdFormat2(instruction.ds_reg, destination_mem, byte);

  auto dst = next;
  *SP = *SP - 2;
  mem->SetUint16BSwap(*SP, *PC + 2);
  *PC = dst;

  if (DisplayVerbose()) {
    printf("CALL R%i=0x%04x, Ad: %i, ", instruction.ds_reg,
           *register_map[instruction.ds_reg], instruction.ad);
    if (byte) {
      std::cout << "BYTE";
    } else {
      std::cout << "WORD";
    }
    if (const_generator_used) {
      printf(", CGVAL: 0x%04x", const_generator_val);
    }
    std::cout << std::endl;
    printf("CALL 0x%04x", dst);
    std::cout << std::endl;
  }

  no_increment = true;
};

void Processor::op_cmp() {
  current_opcode = OPCODES::CMP;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  uint16_t val;
  std::optional<uint16_t> source_mem;
  std::optional<uint16_t> destination_mem;

  auto byte = (instruction.byte_word == 1) ? true : false;

  auto next = GetAsAd(instruction.s_reg, instruction.d_reg, source_mem,
                      destination_mem, byte);

  src = &next.first;
  dst = &next.second;

  auto inverse = ~*src + 1;

  // std::cout<<"SRC: " <<std::hex<<*src<<std::endl;
  // std::cout<<"DST: " <<std::hex<<*dst<<std::endl;
  // std::cout<<"INV: " <<std::hex<<inverse<<std::endl;

  val = *dst + inverse;

  if (DisplayVerbose()) {
    printf("CMP R%i=0x%04x, R%i=0x%04x, As: %i, Ad: %i, ", instruction.s_reg,
           *register_map[instruction.s_reg], instruction.d_reg,
           *register_map[instruction.d_reg], instruction.as, instruction.ad);
    if (byte) {
      std::cout << "BYTE";
    } else {
      std::cout << "WORD";
    }
    if (const_generator_used) {
      printf(", CGVAL: 0x%04x", const_generator_val);
    }

    std::cout << std::endl;
    std::string source = GetOperandString(source_mem, instruction.s_reg);
    std::string destination =
        GetOperandString(destination_mem, instruction.d_reg);
    printf("CMP %s(0x%04x) - %s(0x%04x) = 0x%04x -> %s", destination.c_str(),
           *dst, source.c_str(), *src, val, destination.c_str());
    std::cout << std::endl;
  }

  // Write value to register or memory
  // if (destination_mem) {
  //   WriteToMemory(destination_mem.value(), val, byte);
  // } else {
  //   WriteToRegister(instruction.d_reg, val, byte);
  // }

  // // Set appropriate status flags
  SetFlags(*dst, inverse, val, byte);

  // Write value to register or memory
  // if (destination_mem) {
  //   WriteToMemory(destination_mem.value(), val, byte);
  // } else {
  //   WriteToRegister(instruction.d_reg, val, byte);
  // }

  // if (instruction.d_reg == 0) {
  //   no_increment = true;
  // }
  // throw(ProcessorException("CMP Undefined"));
};

void Processor::op_dadd() { throw(ProcessorException("DADD Undefined")); };
void Processor::op_jc_jhs() {
  current_opcode = OPCODES::JC;
  current_format = FORMAT::JUMP;
  FORMAT::JUMP;
  auto instruction = Jump();
  instruction.val = current_instruction;

  auto offset = instruction.offset;

  if (DisplayVerbose()) {
    printf("JC_JHS OPCODE=0x%04x, C=0x%04x, OFFSET=%i, CARRY=0x%04x",
           instruction.op_code, instruction.c, offset, SR->carry);
    std::cout << std::endl;
  }

  throw(ProcessorException("JC_JHS Undefined"));
};
void Processor::op_jeq_jz() { 
  current_opcode = OPCODES::JEQ;
  current_format = FORMAT::JUMP;
  FORMAT::JUMP;
  auto instruction = Jump();
  instruction.val = current_instruction;

  auto offset = instruction.offset;
  auto new_pc = *PC + (2* offset);

  if (DisplayVerbose()) {
      printf("JEQ_JZ OPCODE=0x%04x, Z=0x%04x, OFFSET=%i, NEW_PC=0x%04x",
            instruction.op_code, SR->zero, offset, new_pc);
      std::cout << std::endl;
    }

  if(SR->zero == 1) {
    *PC = new_pc;
  }
};
void Processor::op_jge() { throw(ProcessorException("JGE Undefined")); 
};
void Processor::op_jle() { throw(ProcessorException("JLE Undefined")); };
void Processor::op_jmp() {
  current_opcode = OPCODES::JC;
  current_format = FORMAT::JUMP;
  FORMAT::JUMP;
  auto instruction = Jump();
  instruction.val = current_instruction;

  auto offset = instruction.offset;
  auto new_pc = *PC + (2* offset);

  if (DisplayVerbose()) {
    printf("JMP OPCODE=0x%04x, C=0x%04x, OFFSET=%i, NEW_PC=0x%04x",
           instruction.op_code, instruction.c, offset, new_pc);
    std::cout << std::endl;
  }

  *PC = new_pc;

  // throw(ProcessorException("JC_JHS Undefined"));
  // throw(ProcessorException("JMP Undefined"));
};
void Processor::op_jn() { throw(ProcessorException("JN Undefined")); };
void Processor::op_jnc_jlo() {
  throw(ProcessorException("JNC_JLO Undefined"));
};
void Processor::op_jne_jnz() {
  throw(ProcessorException("JNE_JNZ Undefined"));
};
void Processor::op_mov() {
  current_opcode = OPCODES::MOV;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  uint16_t val;
  std::optional<uint16_t> source_mem;
  std::optional<uint16_t> destination_mem;

  auto byte = (instruction.byte_word == 1) ? true : false;

  auto next = GetAsAd(instruction.s_reg, instruction.d_reg, source_mem,
                      destination_mem, byte);

  src = &next.first;
  dst = &next.second;
  val = *src;

  if (DisplayVerbose()) {
    printf("MOV R%i=0x%04x, R%i=0x%04x, As: %i, Ad: %i, ", instruction.s_reg,
           *register_map[instruction.s_reg], instruction.d_reg,
           *register_map[instruction.d_reg], instruction.as, instruction.ad);
    if (byte) {
      std::cout << "BYTE";
    } else {
      std::cout << "WORD";
    }
    if (const_generator_used) {
      printf(", CGVAL: 0x%04x", const_generator_val);
    }
    std::cout << std::endl;
    std::string source = GetOperandString(source_mem, instruction.s_reg);
    std::string destination =
        GetOperandString(destination_mem, instruction.d_reg);

    printf("MOV %s(0x%04x) to %s", source.c_str(), val, destination.c_str());
    std::cout << std::endl;
  }

  // Write value to register or memory
  if (destination_mem) {
    WriteToMemory(destination_mem.value(), val, byte);
  } else {
    WriteToRegister(instruction.d_reg, val, byte);
  }

  if (instruction.d_reg == 0) {
    no_increment = true;
  }
};

void Processor::op_push() { throw(ProcessorException("PUSH Undefined")); };
void Processor::op_push_b() { throw(ProcessorException("PUSH_B Undefined")); };
void Processor::op_reti() { throw(ProcessorException("RETI Undefined")); };
void Processor::op_rra() { throw(ProcessorException("RRA Undefined")); };
void Processor::op_rra_b() { throw(ProcessorException("RRA_B Undefined")); };
void Processor::op_rrc() { throw(ProcessorException("RRC Undefined")); };
void Processor::op_rrc_b() { throw(ProcessorException("RRC_B Undefined")); };

void Processor::op_sub() {
  current_opcode = OPCODES::SUB;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  uint16_t val;
  std::optional<uint16_t> source_mem;
  std::optional<uint16_t> destination_mem;

  auto byte = (instruction.byte_word == 1) ? true : false;

  auto next = GetAsAd(instruction.s_reg, instruction.d_reg, source_mem,
                      destination_mem, byte);

  src = &next.first;
  dst = &next.second;

  auto inverse = ~*src + 1;

  // std::cout<<"SRC: " <<std::hex<<*src<<std::endl;
  // std::cout<<"DST: " <<std::hex<<*dst<<std::endl;
  // std::cout<<"INV: " <<std::hex<<inverse<<std::endl;

  val = *dst + inverse;

  if (DisplayVerbose()) {
    printf("SUB R%i=0x%04x, R%i=0x%04x, As: %i, Ad: %i, ", instruction.s_reg,
           *register_map[instruction.s_reg], instruction.d_reg,
           *register_map[instruction.d_reg], instruction.as, instruction.ad);
    if (byte) {
      std::cout << "BYTE";
    } else {
      std::cout << "WORD";
    }
    if (const_generator_used) {
      printf(", CGVAL: 0x%04x", const_generator_val);
    }

    std::cout << std::endl;
    std::string source = GetOperandString(source_mem, instruction.s_reg);
    std::string destination =
        GetOperandString(destination_mem, instruction.d_reg);
    printf("SUB %s(0x%04x) - %s(0x%04x) = 0x%04x -> %s", destination.c_str(),
           *dst, source.c_str(), *src, val, destination.c_str());
    std::cout << std::endl;
  }

  // Write value to register or memory
  if (destination_mem) {
    WriteToMemory(destination_mem.value(), val, byte);
  } else {
    WriteToRegister(instruction.d_reg, val, byte);
  }

  // // Set appropriate status flags
  SetFlags(*dst, inverse, val, byte);
};

void Processor::op_subc() { throw(ProcessorException("SUBC Undefined")); };
void Processor::op_swpb() { throw(ProcessorException("SWPB Undefined")); };
void Processor::op_sxt() { throw(ProcessorException("SXT Undefined")); };

void Processor::op_xor() {
  current_opcode = OPCODES::XOR;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  uint16_t val;
  std::optional<uint16_t> source_mem;
  std::optional<uint16_t> destination_mem;

  auto byte = (instruction.byte_word == 1) ? true : false;

  auto next = GetAsAd(instruction.s_reg, instruction.d_reg, source_mem,
                      destination_mem, byte);

  src = &next.first;
  dst = &next.second;
  val = *src ^ *dst;

  if (DisplayVerbose()) {
    printf("XOR R%i=0x%04x, R%i=0x%04x, As: %i, Ad: %i, ", instruction.s_reg,
           *register_map[instruction.s_reg], instruction.d_reg,
           *register_map[instruction.d_reg], instruction.as, instruction.ad);
    if (byte) {
      std::cout << "BYTE";
    } else {
      std::cout << "WORD";
    }
    if (const_generator_used) {
      printf(", CGVAL: 0x%04x", const_generator_val);
    }
    std::cout << std::endl;
    std::string source = GetOperandString(source_mem, instruction.s_reg);
    std::string destination =
        GetOperandString(destination_mem, instruction.d_reg);

    printf("XOR %s(0x%04x) XOR %s(0x%04x) = 0x%04x -> %s", source.c_str(), *src,
           destination.c_str(), *dst, val, destination.c_str());
    std::cout << std::endl;
  }

  // Write value to register or memory
  if (destination_mem) {
    WriteToMemory(destination_mem.value(), val, byte);
  } else {
    WriteToRegister(instruction.d_reg, val, byte);
  }
};