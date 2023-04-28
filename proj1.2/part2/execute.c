#include "execute.h"  /* include a header */
#include "execute_utils.h"  /* include a header */
#include "mem.h"  /* include a header */
#include <stdio.h>  /* include a header */
#include <assert.h>  /* include a header */

/*******************************
 * Helper Functions
 *******************************/

/* Function handle with error machine code. You should skip the error
   machine code after call this funcion
*/
void raise_machine_code_error(Instruction instruction) {
  write_to_log("Invalid Machine Code: 0x%08x\n", instruction.bits);
}

/* Function handle with undefined ecall parameter.
 */
void raise_undefined_ecall_error(Register a0) {
  write_to_log("Undefined Ecall Number: %d\n", a0);
}

/* Function to exit current program */
void exit_program(Processor *p) {
  if (p->stat == 1) {  /* It judges the condition. */
    /* set cpu state unwork */
    p->stat = 0;  /* The value of p->stat is updated by 0 */
     write_to_log("Exited with error code 0.\n");
  }
  else {
    write_to_log("Error: program already exit.\n");
  }
}

/*
    In this section you need to complete execute instruction. This should modify
    the appropriate registers, make any necessary calls to memory, and updatge
   the program counter to refer You will find the functions in execute_utils.h
   very useful. So it's better to finish execute_utils.c first.
   The following provides you with a general framework. You can follow this
   framework or add your own functions, but please don't change declaration
   or feature of execute().
   Case execute() is the entry point to the execution phase. The
   execute_*() is specific implementations for each instruction type. You had
   better read types.h carefully. Think about the usage of the union defined in
   types.h
 */

/*
 *This function does not check all the validation of the parameters!
 *The function only guarantees the validation of the opcode.
 */
void execute(Instruction inst, Processor *p, Byte *memory) {
  switch (inst.opcode.opcode) {
  /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
  /* R-type normal. 0x33 = 51 */
  case (unsigned)51:
    execute_rtype(inst, p);
    p->PC += 4;
    break;
  /* I-type load. 0x03 = 3  */
  case (unsigned)3:
    execute_load(inst, p, memory);
    p->PC += 4;
    break;
  /* I-type arithmetic 0x13 = 19 */
  case (unsigned)19:
    execute_itype_except_load(inst, p);
    p->PC += 4;
    break;
  /* I-type jalr 0x67 = 103 */
  case (unsigned)103:
    execute_jalr(inst, p);
    break;
  /* I-type ecall 0x73 = 115*/
  case (unsigned)115:
    execute_ecall(p, memory);
    p->PC += 4;
    break;
  /* S-type 0x23 = 35 */
  case (unsigned)35:
    execute_store(inst, p, memory);
    p->PC += 4;
    break;
  /* SB-type 0x63 = 99 */
  case (unsigned)99:
    execute_branch(inst, p);
    break;
  /* U-type 0x17 = 23, 0x37 = 55  */
  case (unsigned)23:
    /* auipc */
    execute_utype(inst, p);
    p->PC += 4;
    break;
  case (unsigned)55:
    /* lui */
    execute_utype(inst, p);
    p->PC += 4;
    break;
  /* UJ-type  0x6f = 111 */
  case (unsigned)111:
    execute_jal(inst, p);
    break;
  default: /* undefined opcode */
    raise_machine_code_error(inst);
    p->PC += 4;
    break;
  }
}

/* the opcode is validated
 *  Do not modify PC!!
 */
void execute_rtype(Instruction inst, Processor *p) {
  /* YOUR CODE HERE */
  /* local variable */
  int rd = inst.rtype.rd, rs1 = inst.rtype.rs1, rs2 = inst.rtype.rs2,
      funct3 = inst.rtype.funct3, funct7 = inst.rtype.funct7;
  Register *reg = p->reg;
  /* for mulh */
  sDouble tmp64;
  /* check the validation of registers
   * register: x0 ~ x31  */
  if (rd < 0 || rd > 31 || rs1 < 0 || rs1 > 31 || rs2 < 0 || rs2 > 31) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    return;
  }
  /* add */
  if (funct3 == 0 && funct7 == 0) {  /* It judges the condition. */
    reg[rd] = reg[rs1] + reg[rs2];
  }
  /* mul */
  else if (funct3 == 0 && funct7 == 1) {  /* It judges the condition. */
    reg[rd] = reg[rs1] * reg[rs2];
  }
  /* sub */
  else if (funct3 == 0 && funct7 == 32) {  /* It judges the condition. */
    reg[rd] = reg[rs1] - reg[rs2];
  }
  /* sll */
  else if (funct3 == 1 && funct7 == 0) {  /* It judges the condition. */
    /* check validation of bit shifting */
    if (reg[rs2] > 31) {  /* It judges the condition. */
      reg[rd] = 0;  /* The value of reg[rd] is updated by 0 */
     }
    else
      reg[rd] = reg[rs1] << reg[rs2];
  }
  /* mulh */
  else if (funct3 == 1 && funct7 == 1) {  /* It judges the condition. */
    tmp64 = ((sDouble)(int)reg[rs1]) * ((sDouble)(int)reg[rs2]);
    reg[rd] = tmp64 >> 32;
  }
  /* slt */
  else if (funct3 == 2 && funct7 == 0) {  /* It judges the condition. */
    reg[rd] = (int)reg[rs1] < (int)reg[rs2];
  }
  /* sltu */
  else if (funct3 == 3 && funct7 == 0) {  /* It judges the condition. */
    reg[rd] = reg[rs1] < reg[rs2];
  }
  /* xor */
  else if (funct3 == 4 && funct7 == 0) {  /* It judges the condition. */
    reg[rd] = reg[rs1] ^ reg[rs2];
  }
  /* div */
  else if (funct3 == 4 && funct7 == 1) {  /* It judges the condition. */
    reg[rd] = (int)reg[rs1] / (int)reg[rs2];
  }
  /* srl */
  else if (funct3 == 5 && funct7 == 0) {  /* It judges the condition. */
    /* check validation of bit shifting */
    if (reg[rs2] > 31) {  /* It judges the condition. */
      reg[rd] = 0;  /* The value of reg[rd] is updated by 0 */
     }
    else
      reg[rd] = reg[rs1] >> reg[rs2];
  }
  /* sra */
  else if (funct3 == 5 && funct7 == 32) {  /* It judges the condition. */
    /* check validation of bit shifting */
    if (reg[rs2] > 31) {  /* It judges the condition. */
      reg[rd] = 0;  /* The value of reg[rd] is updated by 0 */
     }
    else
      reg[rd] = (int)reg[rs1] >> reg[rs2];
  }
  /* or */
  else if (funct3 == 6 && funct7 == 0) {  /* It judges the condition. */
    reg[rd] = reg[rs1] | reg[rs2];
  }
  /* rem */
  else if (funct3 == 6 && funct7 == 1) {  /* It judges the condition. */
    reg[rd] = (int)reg[rs1] % (int)reg[rs2];
  }
  /* and */
  else if (funct3 == 7 && funct7 == 0) {  /* It judges the condition. */
    reg[rd] = reg[rs1] & reg[rs2];
  }
  /* wrong instruction for funct3 or funct7 */
  else {
    raise_machine_code_error(inst);
  }
  /* modify x0 */
  if (rd == 0) {
    reg[0] = 0;
  }
}

/* the opcode is validated
 *  Do not modify PC!! 
 slli and srli don't have to check funct7, because it affect the imm 
 */
void execute_itype_except_load(Instruction inst, Processor *p) {
  /* YOUR CODE HERE */
  /* load parameters */
  int rd = inst.itype.rd, rs1 = inst.itype.rs1, funct3 = inst.itype.funct3;
  int imm = get_imm_operand(inst);
  Register *reg = p->reg;
  /* check validation of the register */
  if (rd < 0 || rd > 31 || rs1 < 0 || rs1 > 31) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    return;
  }
  /* addi */
  if (funct3 == 0) {  /* It judges the condition. */
    reg[rd] = (int)reg[rs1] + imm;
  }
  /* slli */
  else if (funct3 == 1) {  /* It judges the condition. */
    /* check validation of bit shifting */
    if (imm < 0 || imm > 31) {  /* It judges the condition. */
      raise_machine_code_error(inst);
      return;
    }
    else
      reg[rd] = reg[rs1] << imm;
  }
  /* slti */
  else if (funct3 == 2) {  /* It judges the condition. */
    reg[rd] = (int)reg[rs1] < imm;
  }
  /* sltiu */
  else if (funct3 == 3) {  /* It judges the condition. */
    reg[rd] = reg[rs1] < (unsigned int)imm;
  }
  /* xori */
  else if (funct3 == 4) {  /* It judges the condition. */
    reg[rd] = reg[rs1] ^ imm;
  }
  /* srli */
  else if (funct3 == 5 && (inst.itype.imm >> 10 & 1) == 0) {  /* It judges the condition. */
    /* check validation of bit shifting */
    if (imm < 0 || imm > 31) {  /* It judges the condition. */
      raise_machine_code_error(inst);
      return;
    }
    else
      reg[rd] = reg[rs1] >> imm;
  }
  /* srai */
  else if (funct3 == 5 && (inst.itype.imm >> 10 & 1) == 1) {  /* It judges the condition. */
    /* check validation of bit shifting */
    if (imm < 0 || imm > 31) {  /* It judges the condition. */
      raise_machine_code_error(inst);
      return;
    }
    else
      reg[rd] = (int)reg[rs1] >> imm;
  }
  /* ori */
  else if (funct3 == 6) {  /* It judges the condition. */
    reg[rd] = reg[rs1] | imm;
  }
  /* andi */
  else if (funct3 == 7) {  /* It judges the condition. */
    reg[rd] = reg[rs1] & imm;
  }
  /* wrong funct3 */
  else {
    raise_machine_code_error(inst);
  }
  /* modify x0 */
  if (rd == 0) {
    reg[0] = 0;
  }
}

/* You may need to use exit_program() */
void execute_ecall(Processor *p, Byte *memory) {
  /* YOUR CODE HERE */
  /* load parameters */
  Register *reg = p->reg;
  /* tmp_srt for write_to_log() */
  char tmp_str[100];
  /* int i = 0; */
  int address = p->reg[11];
  /* for ecall */
  Byte tmp;
  /* check the syscall number. x10 = a0 */
  switch (reg[10]) {
  /* print integer */
  case 1: {
    sprintf(tmp_str, "%d", reg[11]);
    write_to_log(tmp_str);
    break;
  }
  /* print string */
  case 4: {
    /* It executes a "while" loop */
    while ((tmp = load(memory, address, LENGTH_BYTE, 0)) != 0) {
      sprintf(tmp_str, "%c", tmp);
      write_to_log(tmp_str);
      address++;
    }
    break;
  }
  /* exit */
  case 10: {
    exit_program(p);
    break;
  }
  /* print character */
  case 11: {
    sprintf(tmp_str, "%c", reg[11]);
    write_to_log(tmp_str);
    break;
  }
  /* wrong syscall number */
  default: {
    raise_undefined_ecall_error(reg[10]);
  }
  }
}

/* need to modify PC */
void execute_branch(Instruction inst, Processor *p) {
  /* YOUR CODE HERE */
  /* load parameters */
  int rs1 = inst.sbtype.rs1, rs2 = inst.sbtype.rs2, funct3 = inst.sbtype.funct3;
  Register *reg = p->reg;
  int address = (int)p->PC + get_branch_offset(inst);
  /* check validation of the register */
  if (rs1 < 0 || rs1 > 31 || rs2 < 0 || rs2 > 31) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    p->PC += 4;
    return;
  }
  if (address % 4 != 0) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    p->PC += 4;
    return;
  }
  /* jump out of range! */
   /* raise_machine_code_error(inst); */
  /* if (address > 0x00040000 || address < 0) {
    p->PC += 4;
    return;
  } */
  switch (funct3) {
  /* beq */
  case 0:
    if ((int)reg[rs1] == (int)reg[rs2]) {  /* It judges the condition. */
      p->PC = address;  /* The value of p->PC is updated by address */
     }
     else
      p->PC += 4;
    break;
  /* bne */
  case 1:
    if ((int)reg[rs1] != (int)reg[rs2]) {  /* It judges the condition. */
      p->PC = address;  /* The value of p->PC is updated by address */
     }
     else
      p->PC += 4;     
    break;
  /* blt */
  case 4:
    if ((int)reg[rs1] < (int)reg[rs2]) {  /* It judges the condition. */
      p->PC = address;  /* The value of p->PC is updated by address */
     }
     else
      p->PC += 4;     
    break;
  /* bge */
  case 5:
    if ((int)reg[rs1] >= (int)reg[rs2]) {  /* It judges the condition. */
      p->PC = address;  /* The value of p->PC is updated by address */
     }
     else
      p->PC += 4;     
    break;
  /* bltu */
  case 6:
    if (reg[rs1] < reg[rs2]) {  /* It judges the condition. */
      p->PC = address;  /* The value of p->PC is updated by address */
     }
     else
      p->PC += 4;     
    break;
  /* bgeu */
  case 7:
    if (reg[rs1] >= reg[rs2]) {  /* It judges the condition. */
      p->PC = address;  /* The value of p->PC is updated by address */
     }
     else
      p->PC += 4;     
    break;
  /* not match */
  default:
    raise_machine_code_error(inst);
    p->PC += 4;
    break;
  }
}

/* the opcode is validated
 *  Do not modify PC!! */
void execute_load(Instruction inst, Processor *p, Byte *memory) {
  /* YOUR CODE HERE */
  /* load parameters */
  int rd = inst.itype.rd, rs1 = inst.itype.rs1, funct3 = inst.itype.funct3;
  Register *reg = p->reg;
  /* get the address */
  int address = (int)reg[rs1] + get_imm_operand(inst);
  /* check validation of the register */
  if (rd < 0 || rd > 31 || rs1 < 0 || rs1 > 31) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    return;
  }
  /* check the validation of the conversion */
  /* assert(get_imm_operand(inst) <= 2047 && get_imm_operand(inst) >= -2048); */
  /* lb */
  if (funct3 == 0) {  /* It judges the condition. */
    /* sign extension */
    reg[rd] = bitSigner(load(memory, address, LENGTH_BYTE, 1), 8);
  }
  /* lh */
  else if (funct3 == 1) {  /* It judges the condition. */
    /* sign extension */
    reg[rd] = bitSigner(load(memory, address, LENGTH_HALF_WORD, 1), 16);
  }
  /* lw */
  else if (funct3 == 2) {  /* It judges the condition. */
    reg[rd] =
    load(memory, address, LENGTH_WORD, 1);
  }
  /* lbu */
  else if (funct3 == 4) {  /* It judges the condition. */
    reg[rd] = load(memory, address, LENGTH_BYTE, 1);
  }
  /* lhu */
  else if (funct3 == 5) {  /* It judges the condition. */
    reg[rd] = load(memory, address, LENGTH_HALF_WORD, 1);
  }
  /* wrong funct3 */
  else {
    raise_machine_code_error(inst);
  }
  /* modify x0 */
  if (rd == 0) {
    reg[0] = 0;
  }
}

void execute_store(Instruction inst, Processor *p, Byte *memory) {
  /* YOUR CODE HERE */
  /* load parameters */
  int rs1 = inst.stype.rs1, rs2 = inst.stype.rs2, funct3 = inst.stype.funct3;
  Register *reg = p->reg;
  /* get the address */
  int address = (int)reg[rs1] + get_store_offset(inst);
  /* check validation of the register */
  if (rs1 < 0 || rs1 > 31 || rs2 < 0 || rs2 > 31) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    return;
  }
  switch (funct3) {
  /* sb */
  case 0:
    store(memory, address, LENGTH_BYTE, reg[rs2], 1);
    break;
  /* sh */
  case 1:
    store(memory, address, LENGTH_HALF_WORD, reg[rs2], 1);
    break;
  /* sw */
  case 2:
    store(memory, address, LENGTH_WORD, reg[rs2], 1);
    break;
  /* not match */
  default:
    raise_machine_code_error(inst);
    break;
  }
}

/* need to modify PC */
void execute_jal(Instruction inst, Processor *p) {
  /* YOUR CODE HERE */
  /* load parameters */
  int rd = inst.ujtype.rd;
  int address = p->PC + get_jump_offset(inst);
  Register *reg = p->reg;
  /* check validation of the register */
  if (rd < 0 || rd > 31) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    p->PC += 4;
    return;
  }
  /* check validation */
  if (address % 4 != 0) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    p->PC += 4;
    return;
  }
  /* store the PC */
  reg[rd] = p->PC + 4;
  /* jump */
  p->PC = address;  /* The value of p->PC is updated by address */
 }

/* need to modify PC !!!!!*/
void execute_jalr(Instruction inst, Processor *p) {
  /* YOUR CODE HERE */
  /* load parameters */
  int rd = inst.itype.rd, rs1 = inst.itype.rs1, funct3 = inst.itype.funct3;
  Register *reg = p->reg;
  /* get the address */
  int address = (int)reg[rs1] + get_imm_operand(inst);
  /* check validation of the register and funct3 */
  if (rd < 0 || rd > 31 || rs1 < 0 || rs1 > 31) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    p->PC += 4;
    return;
  }
  if (funct3 != 0) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    p->PC += 4;
    return;
  }
  /* check the validation of the conversion */
  if (address % 2 != 0) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    p->PC += 4;
    return;
  }
  reg[rd] = p->PC + 4;
  /* modify x0 */
  if (rd == 0) {
    reg[0] = 0;
  }
  /*!!!!!!!!!! have fog!!!!!!!!!!! */
  p->PC = address;  /* The value of p->PC is updated by address */
 }

void execute_utype(Instruction inst, Processor *p) {
  /* YOUR CODE HERE */
  /* load parameters */
  int rd = inst.utype.rd;
  Register *reg = p->reg;
  int offset = inst.utype.imm;
  /* check validation of the register */
  if (rd < 0 || rd > 31) {  /* It judges the condition. */
    raise_machine_code_error(inst);
    return;
  }
  /* auipc */
  if (inst.utype.opcode == 0x17) {  /* It judges the condition. */
    reg[rd] = p->PC + (unsigned)(offset << 12);
  }
  else if (inst.utype.opcode == 0x37) {  /* It judges the condition. */
    reg[rd] = offset << 12;
  }
  /* not matched */
  else {
    raise_machine_code_error(inst);
  }
  /* modify x0 */
  if (rd == 0) {
    reg[0] = 0;
  }
}