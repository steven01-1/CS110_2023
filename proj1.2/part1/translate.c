/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia
   to be part of the project in Feb 2023.
*/

#include "translate.h"  /* include a header */
#include "tables.h"  /* include a header */
#include "translate_utils.h"  /* include a header */
#include <stdio.h>  /* include a header */
#include <stdlib.h>  /* include a header */
#include <string.h>  /* include a header */
  
/* Writes data during the assembler's first pass to OUTPUT.
   The input Byte* buf should point to a buffer with a size larger than 8 Bytes.
   Then this function takes the first 4 bytes as a word and write to output
   FILE. And move the second 4 bytes to the head and set second 4 bytes to 0.
   Think about how to use this function in the output data.

   Notes: You can leave this function out, but you need to make sure that
   the form of the output data file remains the same.
 */
  
int write_static_data(FILE *output, Byte *buf) {  
  fprintf(output, "%08x\n", *((Word *)buf));  
  memcpy(buf, buf + 4, 4);  
  memset(buf + 4, 0, 4);  
  return 0;  /* The function returns 0 */
}  
  
/* Writes original code during the assembler's first pass to OUTPUT.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Returns the number of instructions written (so 0 if there were any errors).

   Notes: Think about what you should do with this return value.
 */
  
unsigned write_original_code(FILE *output, const char *name, char **args,  
                             int num_args) {  
  long imm;  
  unsigned num_basic_code = 0;  
  
  if (!output || !name || !args)  /* It judges the condition. */
    return 0;  /* The function returns 0 */
  
  /* if-else ladder for original code output */
  if (strcmp(name, "la") == 0) {  /* It judges the condition. */
    /* check count of arguments */
    if (num_args != 2)  /* It judges the condition. */
      return 0;  /* The function returns 0 */
  
    if (num_args == 2)  /* It judges the condition. */
      num_basic_code = 2;  /* The value of num_basic_code is updated by 2 */
   }  
  else if (strcmp(name, "lw") == 0) {  /* It judges the condition. */
    /* lw can takes two or three parameters */
    if (num_args != 2 && num_args != 3)  /* It judges the condition. */
      return 0;  /* The function returns 0 */
  
    /* lw x11, label */
    if (num_args == 2)  /* It judges the condition. */
      num_basic_code = 2;  /* The value of num_basic_code is updated by 2 */
   
    /* lw x11, 0(x12) */
    if (num_args == 3)  /* It judges the condition. */
      num_basic_code = 1;  /* The value of num_basic_code is updated by 1 */
   }  
  else if (strcmp(name, "li") == 0) {  /* It judges the condition. */
    /* check count of arguments */
    if (num_args != 2)  /* It judges the condition. */
      return 0;  /* The function returns 0 */
    /* translate and check input immediate */
    if (translate_num_32(&imm, args[1]) == -1)  /* It judges the condition. */
      return 0;  /* The function returns 0 */
  
    /* check how many lines are used */
    if (_INT12_MIN_ <= imm && imm <= _INT12_MAX_)  /* It judges the condition. */
      num_basic_code = 1;  /* The value of num_basic_code is updated by 1 */
     else  
      num_basic_code = 2;  /* The value of num_basic_code is updated by 2 */
   }  
  else {  
    num_basic_code = 1;  /* The value of num_basic_code is updated by 1 */
   }  
  
  /* perform write string to output here */
  write_inst_string(output, name, args, num_args);  
  if (num_basic_code > 1)  /* It judges the condition. */
    fprintf(output, "-\n");  
  
  return num_basic_code;  /* The function returns num_basic_code */
}  
  
/* Translate instructions perform a pseudoinstruction extpansion and call
   write_instruction() to write the corresponding instruction. The case for
   general instructions, beqz and la has already been completed, but you need to
   write code to translate the bnez, j, jr, li, lw and mv pseudoinstructions.
   Your pseudoinstruction expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in write_instruction().
   However, for pseudoinstructions, you must make sure that ARGS contains the
   correct number of arguments. You do NOT need to check whether the registers
   or label are valid, since that will be checked in write_instruction().

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addi instruction. Otherwise, expand it into
        a lui-addi pair.

   And for other pseudoinstruction:
    - your expansion should be as same as venus. Try them in venus and see what
        will happen.

   venus has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if venus behaves differently.

   If writing multiple instructions, make sure that each instruction has correct
   addrress.

   Returns the number of instructions written (so 0 if there were any errors).
 */
  
int translate_inst(FILE *basic_code, FILE *machine_code, const char *name,  
                   char **args, size_t num_args, uint32_t addr,  
                   SymbolTable *symtbl) {  
  char *sub_args[3];  
  char buf[100];  
  long imm, head /*,uiSec, addSec*/;
  /* for get bits */
  int64_t fuck_c89;
  /* early error handling on invalid arguments */
  if (!basic_code || !machine_code || !name || !args)  /* It judges the condition. */
    return 0;  /* The function returns 0 */
  
  if (strcmp(name, "beqz") == 0) {  /* It judges the condition. */
    /* check count of arguments */
    if (num_args != 2)  /* It judges the condition. */
      return 0;  /* The function returns 0 */
  
    /* Setup parameters */
    sub_args[0] = args[0];  /* The value of sub_args[0] is updated by args[0] */
     sub_args[1] = "x0";  /* The value of sub_args[1] is updated by "x0" */
     sub_args[2] = args[1];  /* The value of sub_args[2] is updated by args[1] */
     if (write_instruction(basic_code, machine_code, "beq", sub_args, 3, addr,  /* It judges the condition. */
                          symtbl) == -1)  
      return 0;  /* The function returns 0 */
    /* error if not successful */
    return 1;  /* The function returns 1 */
  }  
  else if (strcmp(name, "bnez") == 0) {  /* It judges the condition. */
    /* YOUR CODE HERE */
    if (num_args != 2) {  /* It judges the condition. */
      return 0;  /* The function returns 0 */
    }  
    sub_args[0] = args[0];  /* The value of sub_args[0] is updated by args[0] */
     sub_args[1] = "x0";  /* The value of sub_args[1] is updated by "x0" */
     sub_args[2] = args[1];  /* The value of sub_args[2] is updated by args[1] */
     if (write_instruction(basic_code, machine_code, "bne", sub_args, 3, addr,  /* It judges the condition. */
                          symtbl) == -1) {  
      return 0;  /* The function returns 0 */
    }  
    return 1;  /* The function returns 1 */
  }  
  else if (strcmp(name, "j") == 0) {  /* It judges the condition. */
    /* YOUR CODE HERE */
    if (num_args != 1) {  /* It judges the condition. */
      return 0;  /* The function returns 0 */
    }  
    sub_args[0] = "x0";  /* The value of sub_args[0] is updated by "x0" */
    sub_args[1] = args[0];  /* The value of sub_args[1] is updated by args[0] */
     if (write_instruction(basic_code, machine_code, "jal", sub_args, 2, addr,  /* It judges the condition. */
                          symtbl) == -1) {
      return 0;  /* The function returns 0 */
    }  
    return 1;  /* The function returns 1 */
  }  
  else if (strcmp(name, "jr") == 0) {  /* It judges the condition. */
    /* YOUR CODE HERE */
    if (num_args != 1) {  /* It judges the condition. */
      return 0;  /* The function returns 0 */
    }  
    sub_args[0] = "x0";  /* The value of sub_args[0] is updated by "x0" */
     sub_args[1] = args[0];  /* The value of sub_args[1] is updated by args[0] */
     sub_args[2] = "0";  /* The value of sub_args[2] is updated by "0" */
     if (write_instruction(basic_code, machine_code, "jalr", sub_args, 3, addr,  /* It judges the condition. */
                          symtbl) == -1) {  
      return 0;  /* The function returns 0 */
    }  
    return 1;  /* The function returns 1 */
  } /* jal and jalr can be explicitly specified or use default ra */
  else if (strcmp(name, "jal") == 0) {  /* It judges the condition. */
    /* YOUR CODE HERE */
    if (num_args == 1) {  /* It judges the condition. */
      imm = get_addr_for_symbol(symtbl, args[0]);  /* It executes a "for" loop */
      sprintf(buf,"%d",STATIC_CAST(int, imm));  
      sub_args[0] = "x1";  /* The value of sub_args[0] is updated by "x1" */
      /* sub_args[1] = buf; */
      sub_args[1] = args[0];  /* The value of sub_args[1] is updated by args[0] */
       if (write_instruction(basic_code, machine_code, "jal", sub_args, 2, addr,  /* It judges the condition. */
                            symtbl) == -1) {  
        return 0;  /* The function returns 0 */
      }  
      return 1;  /* The function returns 1 */
    }  
    else if (num_args == 2) {  /* It judges the condition. */
      imm = get_addr_for_symbol(symtbl, args[1]);  /* It executes a "for" loop */
      sprintf(buf,"%d",STATIC_CAST(int, imm));  
      sub_args[0] = args[0];  /* The value of sub_args[0] is updated by args[0] */
      sub_args[1] = buf;
      if (write_instruction(basic_code, machine_code, "jal", args, 2, addr,  /* It judges the condition. */
                            symtbl) == -1) {  
        return 0;  /* The function returns 0 */
      }  
      return 1;  /* The function returns 1 */
    }  
    return 0;  /* The function returns 0 */
  }  
  else if (strcmp(name, "jalr") == 0) {  /* It judges the condition. */
    /* YOUR CODE HERE */
    if (num_args == 1) {  /* It judges the condition. */
      sub_args[0] = "x1";  /* The value of sub_args[0] is updated by "x1" */
       sub_args[1] = args[0];  /* The value of sub_args[1] is updated by args[0] */
       sub_args[2] = "0";  /* The value of sub_args[2] is updated by "0" */
       if (write_instruction(basic_code, machine_code, "jalr", sub_args, 3, addr,  /* It judges the condition. */
                            symtbl) == -1) {  
        return 0;  /* The function returns 0 */
      }  
      return 1;  /* The function returns 1 */
    }  
    else if (num_args == 3) {  /* It judges the condition. */
      if (write_instruction(basic_code, machine_code, "jalr", args, 3, addr,  /* It judges the condition. */
                            symtbl) == -1) {  
        return 0;  /* The function returns 0 */
      }  
      return 1;  /* The function returns 1 */
    }  
    return 0;  /* The function returns 0 */
  }  
  else if (strcmp(name, "la") == 0) {  /* It judges the condition. */
    /* check count of arguments */
    if (num_args != 2)  /* It judges the condition. */
      return 0;  /* The function returns 0 */
    /* Get the symbol address */
    imm = get_addr_for_symbol(symtbl, args[1]);  /* It executes a "for" loop */
    if (imm == -1)  /* It judges the condition. */
      return 0;  /* The function returns 0 */
    /* Get the symbol offset from current PC */
    imm = (imm - addr);  
    /* Setup parameters for auipc */
    head = (imm + 0x800) >> 12;  
    sprintf(buf, "%d", STATIC_CAST(int, head));  
    sub_args[0] = args[0];  /* The value of sub_args[0] is updated by args[0] */
     sub_args[1] = buf;  /* The value of sub_args[1] is updated by buf */
     /* auipc */
    if (write_instruction(basic_code, machine_code, "auipc", sub_args, 2, addr,  /* It judges the condition. */
                          symtbl) == -1)  
      return 0;  /* The function returns 0 */
    /* Setup parameters for addi */
    imm = imm - (head << 12);  
    sprintf(buf, "%d", STATIC_CAST(int, imm));  
    sub_args[0] = args[0];  /* The value of sub_args[0] is updated by args[0] */
     sub_args[1] = args[0];  /* The value of sub_args[1] is updated by args[0] */
     sub_args[2] = buf;  /* The value of sub_args[2] is updated by buf */
     /* addi */
    if (write_instruction(basic_code, machine_code, "addi", sub_args, 3,  /* It judges the condition. */
                          addr + 4, symtbl) == -1)  
      return 0;  /* The function returns 0 */
    /* error if not successful */
    return 2;  /* The function returns 2 */
  }  
  else if (strcmp(name, "lw") == 0) {  /* It judges the condition. */
    /* YOUR CODE HERE */
    if (num_args == 2) {  /* It judges the condition. */
      imm = get_addr_for_symbol(symtbl, args[1]);  /* It executes a "for" loop */
      if (imm == -1) return 0;  /* It judges the condition. */
      imm -= addr;
      head = (imm + 0x800) >> 12;
      sprintf(buf, "%d", STATIC_CAST(int, head));
      sub_args[0] =
      args[0]; /* The value of sub_args[0] is updated by args[0] */
      sub_args[1] = buf; /* The value of sub_args[1] is updated by buf */
      if (write_instruction(basic_code, machine_code, "auipc", sub_args,
                            2, /* It judges the condition. */
                            addr, symtbl) == -1) {  
        return 0;  /* The function returns 0 */
      }
      imm = imm - (head << 12);
      printf("here is lw %ld\n", imm);
      sprintf(buf, "%d", STATIC_CAST(int, imm));
      sub_args[0] =
      args[0]; /* The value of sub_args[0] is updated by args[0] */
      sub_args[1] = buf; /* The value of sub_args[1] is updated by "x0" */
      sub_args[2] = args[0]; /* The value of sub_args[2] is updated by buf */
      if (write_instruction(basic_code, machine_code, "lw", sub_args,
                            3, /* It judges the condition. */
                            addr + 4, symtbl) == -1) {
        return 0; /* The function returns 0 */
      }
      return 2; /* The function returns 2 */
    }  
    if (num_args == 3) {  /* It judges the condition. */
      if (write_instruction(basic_code, machine_code, "lw", args, 3, addr,  /* It judges the condition. */
                            symtbl) == -1) { 
        return 0;  /* The function returns 0 */
      }  
      return 1;  /* The function returns 1 */
    }  
    return 0;  /* The function returns 0 */
  }
  else if (strcmp(name, "li") == 0) { /* It judges the condition. */
    /* YOUR CODE HERE */
    if (num_args != 2) { /* It judges the condition. */
      return 0; /* The function returns 0 */
    }
    imm = strtol(args[1], NULL, 0); /*load the immediate*/
    /* printf("%ld\n", imm); */
    /* check the value of the immediate*/
    if (-2048 <= imm && imm <= 2047) {
      sub_args[0] = args[0];
      sub_args[1] = "x0";
      sprintf(buf, "%d", STATIC_CAST(int, imm));
      sub_args[2] = buf;
      /* run addi args[0] x0 imm*/
      if (write_instruction(basic_code, machine_code, "addi", sub_args, 3, addr,
                            symtbl) == -1) {
        return 0;
      }
      return 1;
    }
    else {
      /*set arguments for lui*/
      /* printf("%ld 1\n", imm); */
      imm = imm & 4294967295;
      head = (imm + 0x800) >> 12;
      /* fuck CA */
      fuck_c89 = head & 4294967295;
      fuck_c89++;
      /* printf("%ld %ld 2\n", head, imm); */
      sprintf(buf, "%d", STATIC_CAST(int, head));
      sub_args[0] = args[0];
      sub_args[1] = buf;
      /* lui args[0] head*/
      if (write_instruction(basic_code, machine_code, "lui", sub_args, 2, addr,
                            symtbl) == -1) {
        return 0;
      }
      /*set arguments for addi*/
      imm = imm - (head << 12);
      /* printf("%ld 3\n", imm); */
      sprintf(buf, "%d", STATIC_CAST(int, imm));
      sub_args[0] = args[0];
      sub_args[1] = args[0];
      sub_args[2] = buf;
      /* addi args[0] x0 the first 11 bits of imm*/
      if (write_instruction(basic_code, machine_code, "addi", sub_args, 3,
                            addr + 4, symtbl) == -1) {
        return 0;
      }
      return 2;
    }
  }
  else if (strcmp(name, "mv") == 0) {  /* It judges the condition. */
    /* YOUR CODE HERE */
    if (num_args != 2) {  /* It judges the condition. */
      return 0;  /* The function returns 0 */
    }  
    sub_args[0] = args[0];  /* The value of sub_args[0] is updated by args[0] */
     sub_args[1] = args[1];  /* The value of sub_args[1] is updated by args[1] */
     sub_args[2] = "0";  /* The value of sub_args[2] is updated by "x0" */
     if (write_instruction(basic_code, machine_code, "addi", sub_args, 3, addr,  /* It judges the condition. */
                          symtbl) == -1) {  
      return 0;  /* The function returns 0 */
    }  
    return 1;  /* The function returns 1 */
  }  
  else {  
    /* handle non-pseudo ones */
    if (write_instruction(basic_code, machine_code, name, args, num_args, addr,  /* It judges the condition. */
                          symtbl) == -1)  
      return 0;  /* The function returns 0 */
    /* error if not successful */
    return 1;  /* The function returns 1 */
  }  
  
  return 0;  /* The function returns 0 */
}  
  
/* Writes the instruction in basic and hexadecimal format to OUTPUT during
   pass #2.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   The address (addr) is given for current instruction PC address.

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step.

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write
   anything to OUTPUT but simply return -1. venus may be a useful resource for
   this step.

   Note the use of helper functions. Consider writing your own! If the function
   definition comes afterwards, you must declare it first (see translate.h).

   Returns 0 on success and -1 on error.
 */
int write_instruction(FILE *basic_code, FILE *machine_code, const char *name,  
                      char **args, size_t num_args, uint32_t addr,  
                      SymbolTable *symtbl) {  
  /** R-format */
  /* add */
  if (strcmp(name, "add") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x00, 0x00, args,  
                       num_args);  
  /* mul */
  else if (strcmp(name, "mul") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x00, 0x01, args,  
                       num_args);  
  /* sub */
  else if (strcmp(name, "sub") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x00, 0x20, args,  
                       num_args);  
  /* sll */
  else if (strcmp(name, "sll") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x01, 0x00, args,  
                       num_args);  
  /* mulh */
  else if (strcmp(name, "mulh") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x01, 0x01, args,  
                       num_args);  
  /* slt */
  else if (strcmp(name, "slt") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x02, 0x00, args,  
                       num_args);  
  /* sltu */
  else if (strcmp(name, "sltu") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x03, 0x00, args,  
                       num_args);  
  /* xor */
  else if (strcmp(name, "xor") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x04, 0x00, args,  
                       num_args);  
  /* div */
  else if (strcmp(name, "div") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x04, 0x01, args,  
                       num_args);  
  /* srl */
  else if (strcmp(name, "srl") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x05, 0x00, args,  
                       num_args);  
  /* sra */
  else if (strcmp(name, "sra") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x05, 0x20, args,  
                       num_args);  
  /* or */
  else if (strcmp(name, "or") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x06, 0x00, args,  
                       num_args);  
  /* rem */
  else if (strcmp(name, "rem") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x06, 0x01, args,  
                       num_args);  
  /* and */
  else if (strcmp(name, "and") == 0)  /* It judges the condition. */
    return write_rtype(basic_code, machine_code, name, 0x33, 0x07, 0x00, args,  
                       num_args);  
  /* YOUR CODE HERE */
  /* I-type */
  else if (strcmp(name, "lb")  == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x03, 0x0, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "lh") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x03, 0x01, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "lw") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x03, 0x02, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "lbu") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x03, 0x04, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "lhu") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x03, 0x05, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "addi") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x0, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "slli") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x1, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "slti") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x2, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "sltiu") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x3, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "xori") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x4, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "srli") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x5, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "srai") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x5, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "ori") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x6, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "andi") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x13, 0x7, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "jalr") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x67, 0x0, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "ecall") == 0) {  /* It judges the condition. */
    return write_itype(basic_code, machine_code, name, 0x73, 0x0, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "sb") == 0) {  /* It judges the condition. */
    return write_stype(basic_code, machine_code, name, 0x23, 0x0, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "sh") == 0) {  /* It judges the condition. */
    return write_stype(basic_code, machine_code, name, 0x23, 0x1, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "sw") == 0) {  /* It judges the condition. */
    return write_stype(basic_code, machine_code, name, 0x23, 0x2, args, num_args, symtbl);  
  }  
  /* SB-type */
  else if (strcmp(name, "beq") == 0) {  /* It judges the condition. */
    return write_sbtype(basic_code, machine_code, name, 0x63, 0x0, args, num_args, addr, symtbl);  
  }  
  else if (strcmp(name, "bne") == 0) {  /* It judges the condition. */
    return write_sbtype(basic_code, machine_code, name, 0x63, 0x1, args, num_args, addr, symtbl);  
  }  
  else if (strcmp(name, "blt") == 0) {  /* It judges the condition. */
    return write_sbtype(basic_code, machine_code, name, 0x63, 0x4, args, num_args, addr, symtbl);  
  }  
  else if (strcmp(name, "bge") == 0) {  /* It judges the condition. */
    return write_sbtype(basic_code, machine_code, name, 0x63, 0x5, args, num_args, addr, symtbl);  
  }  
  else if (strcmp(name, "bltu") == 0) {  /* It judges the condition. */
    return write_sbtype(basic_code, machine_code, name, 0x63, 0x6, args, num_args, addr, symtbl);  
  }  
  else if (strcmp(name, "bgeu") == 0) {  /* It judges the condition. */
    return write_sbtype(basic_code, machine_code, name, 0x63, 0x7, args, num_args, addr, symtbl);  
  }  
  /* U-type */
  else if (strcmp(name, "auipc") == 0) {  /* It judges the condition. */
    return write_utype(basic_code, machine_code, name, 0x17, args, num_args, symtbl);  
  }  
  else if (strcmp(name, "lui") == 0) {  /* It judges the condition. */
    return write_utype(basic_code, machine_code, name, 0x37, args, num_args, symtbl);  
  }  
  /* UJ-type */
  else if (strcmp(name, "jal") == 0) {  /* It judges the condition. */
    return write_ujtype(basic_code, machine_code, name, 0x6f, args, num_args, addr, symtbl);  
  }  
  else  
    return -1;  /* The function returns -1 */
}  
  
/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_rtype(),
   write_inst_stype(), write_inst_sbtype(), write_inst_utype(),
   write_inst_ecall(), write_inst_hex() to write to basic_code or machine_code.
   Both are defined in translate_utils.h.

   Function write_rtype() is complete for reference. Complete other functions
   implementation below. You will find bitwise operations to be the cleanest
   way to complete this function.

   Returns 0 on success and -1 on failure
 */
int write_rtype(FILE *basic_code, FILE *machine_code, const char *name,  
                uint8_t opcode, uint8_t funct3, uint8_t funct7, char **args,  
                size_t num_args) {  
  /*  Perhaps perform some error checking? */
  int rd, rs, rt;  
  uint32_t instruction;  
  /* R-format requires rd rs rt */
  if (num_args != 3) {  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  }  
  
  /* destination register */
  rd = translate_reg(args[0]);  /* The value of rd is updated by translate_reg(args[0]) */
  /* printf("%d %s\n", rd, args[0]); */
  /* source register 1 */
  rs = translate_reg(args[1]);  /* The value of rs is updated by translate_reg(args[1]) */
   /* source register 2 */
  rt = translate_reg(args[2]);  /* The value of rt is updated by translate_reg(args[2]) */
   
  /* error checking for register ids */
  if (rd == -1 || rs == -1 || rt == -1) {  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  }  
  
  /* write basic code */
  write_inst_rtype(basic_code, name, rd, rs, rt);  
  
  /* generate instruction */
  instruction = opcode + (rd << 7) + (funct3 << 12) + (rs << 15) + (rt << 20) +  
                (funct7 << 25);  
  
  /* write machine code */
  write_inst_hex(machine_code, instruction);  
  return 0;  /* The function returns 0 */
}  
  
int write_itype(FILE *basic_code, FILE *machine_code, const char *name,  
                uint8_t opcode, uint8_t funct3, char **args, size_t num_args,  
                SymbolTable *symbol) {  
  /* YOUR CODE HERE */
  /*  Perhaps perform some error checking? */
  int rd, rs, imm;  
  uint32_t instruction;  
  /* check if strtol is successful */
  char *tmp = NULL;
  /* judge whether it has '()' */
  int parenthesis = 0;  
  if (symbol == NULL)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* deal with ecall */
  if (strcmp(name, "ecall") == 0) {  /* It judges the condition. */
    write_inst_ecall(basic_code, name);
    instruction = 0x00000073;  /* The value of instruction is updated by 0x00000073 */
     write_inst_hex(machine_code, instruction);  
    return 0;  /* The function returns 0 */
  }
  /* I-format requires rd rs imm */
  if (num_args != 3) {  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  }  
  /* destination register */
  rd = translate_reg(args[0]);  /* The value of rd is updated by translate_reg(args[0]) */
   if (strcmp(name, "lb") == 0 || strcmp(name, "lh") == 0 ||  /* It judges the condition. */
      strcmp(name, "lw") == 0 || strcmp(name, "lbu") == 0 ||  
      strcmp(name, "lhu") == 0)
    parenthesis = 1;  /* The value of parenthesis is updated by 1 */
  /* source register 1 */
  if (parenthesis) {  /* It judges the condition. */
    rs = translate_reg(args[2]);  /* The value of rs is updated by translate_reg(args[2]) */
     /* judge if it is a label */
    if (is_valid_label(args[1])) {  /* It judges the condition. */
      imm = get_addr_for_symbol(symbol, args[1]);  /* It executes a "for" loop */
      if (imm == -1)  /* It judges the condition. */
        return -1;  /* The function returns -1 */
    }
    else {
      tmp = NULL;
      imm = strtol(args[1], &tmp, 0);
      if (args[1] == tmp )  /* It judges the condition. */
        return -1;  /* The function returns -1 */
    }  
  }  
  else {  
    rs = translate_reg(args[1]);  /* The value of rs is updated by translate_reg(args[1]) */
     /* imm = strtol(args[2], NULL, 0); */
    if (is_valid_label(args[2])) {  /* It judges the condition. */
      imm = get_addr_for_symbol(symbol, args[2]);  /* It executes a "for" loop */
      if (imm == -1)  /* It judges the condition. */
        return -1;  /* The function returns -1 */
    }  
    else {
      tmp = NULL;
      imm = strtol(args[2], &tmp, 0);
      if (tmp == args[2])  /* It judges the condition. */
        return -1;  /* The function returns -1 */
    }  
  }  
  /* judge validation */
  if (rd == -1 || rs == -1)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* even number address???  2^11 = 2048  */
  if (strcmp(name, "jalr") != 0 && (imm > 2047 || imm < -2048))  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* 2^6 = 4096 */
  if (strcmp(name, "jalr") == 0 && (imm < -4096 || imm > 4095)) {
    return -1;
  }
  /* the imm for logical shifting cannot be too big */
  if (strcmp(name, "slli") == 0 || strcmp(name, "srli") == 0 || strcmp(name, "srai")
   == 0) {
    if (imm >= 32 || imm < 0) {
      return -1;
    }
   }
  /*write_inst_ity  */
  if (parenthesis)  /* It judges the condition. */
    fprintf(basic_code, "%s x%d %d(x%d)\n", name, rd, imm, rs);  
  else
    fprintf(basic_code, "%s x%d x%d %d\n", name, rd, rs, imm);  
  /* write machine code */
  instruction = opcode + (rd << 7) + (funct3 << 12) + (rs << 15) + (imm << 20);  
  /* for srai */
  if (strcmp(name, "srai") == 0) {
    instruction = opcode + (rd << 7) + (funct3 << 12) + (0x20 << 25) + (rs << 15) + (imm << 20);
   }
  /* for jalr */
  if (strcmp(name, "jalr") == 0) {
    printf("imm is : %d\n", imm);
    instruction =
    opcode + (rd << 7) + (funct3 << 12) + +(rs << 15) + (imm  << 20);
  }
  /* generate instruction */
  write_inst_hex(machine_code, instruction);  
  return 0;  /* The function returns 0 */
}  
  
int write_stype(FILE *basic_code, FILE *machine_code, const char *name,  
                uint8_t opcode, uint8_t funct3, char **args, size_t num_args,  
                SymbolTable *symbol) {  
  /* YOUR CODE HERE */
  /*  Perhaps perform some error checking? */
  int rs1, rs2;
  int imm;  
  uint32_t instruction;
  char *tmp = NULL;
  if (num_args != 3) { /* It judges the condition. */
    return -1;  /* The function returns -1 */
  }
  /* the parameters should be passed from left to right rs2 imm rs1 */
  /* check if the second parameter is a label */
  if (is_valid_label(args[1])) {  /* It judges the condition. */
    imm = get_addr_for_symbol(symbol, args[1]);  /* It executes a "for" loop */
    if (imm == -1)  /* It judges the condition. */
      return -1;  /* The function returns -1 */
  }  
  else {
    tmp = NULL;
    imm = strtol(args[1], &tmp, 0);
    if (args[1] == tmp)  /* It judges the condition. */
      return -1;  /* The function returns -1 */
  }  
  /* source register 2 */
  rs2 = translate_reg(args[0]);  /* The value of rs2 is updated by translate_reg(args[0]) */
   /* source register 1 */
  rs1 = translate_reg(args[2]);  /* The value of rs1 is updated by translate_reg(args[2]) */
   /* check validation */
  if (rs1 == -1 || rs2 == -1)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  if (imm < -2048 || imm > 2047)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* write basic code */
  write_inst_stype(basic_code, name, rs2, rs1, imm);  
  /* generate instruction */
  instruction = opcode + ((imm & 31) << 7) + (funct3 << 12) + (rs1 << 15) +  
                (rs2 << 20) + ((imm >> 5) << 25);  
  /* write machine code */
  write_inst_hex(machine_code, instruction);  
  return 0;  /* The function returns 0 */
}  
  
/* Hint: the way for branch to calculate relative address.
   e.g. bne bne rs rt label
   assume the byte_offset(addr) of label is L,
   current instruction byte_offset(addr) is A
   the relative address I for label satisfy:
     L = A + I
   so the relative addres is
     I = L - A              */
int write_sbtype(FILE *basic_code, FILE *machine_code, const char *name,  
                 uint8_t opcode, uint8_t funct3, char **args, size_t num_args,  
                 uint32_t addr, SymbolTable *symbol) {  
  /* YOUR CODE HERE */
  /*  Perhaps perform some error checking? */
  int rs1, rs2;
  int offset;
  uint32_t instruction;
  /* SB-format requires rd rs rt */
  if (num_args != 3) {  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  }  
  /* source register 1 */
  rs1 = translate_reg(args[0]);  /* The value of rs1 is updated by translate_reg(args[0]) */
   /* source register 2 */
  rs2 = translate_reg(args[1]);  /* The value of rs2 is updated by translate_reg(args[1]) */
   offset = get_addr_for_symbol(symbol, args[2]);  /* It executes a "for" loop */
  /* check validation */
  if (offset == -1)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  offset -= addr;  
  if (rs1 == -1 || rs2 == -1) {  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  }  
  /* 2^12 = 4096 */
  if (offset < -4096 || offset > 4095)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* write basic code */
  write_inst_sbtype(basic_code, name, rs1, rs2, offset);  
  /* generate instruction */
  /* offset >>= 1;  */ 
  /* label ends with '0' */ 
  instruction = opcode + ((offset >> 11 & 1) << 7) + ((offset & 30) << 7) +  
                (funct3 << 12) + (rs1 << 15) + (rs2 << 20) +  
                ((offset >> 5 & 63) << 25) + ((offset >> 12 & 1) << 31);  
  
  /* write machine code */
  write_inst_hex(machine_code, instruction);  
  return 0;  /* The function returns 0 */
}  
  
  
int write_utype(FILE *basic_code, FILE *machine_code, const char *name,  
                uint8_t opcode, char **args, size_t num_args ,SymbolTable *symbol)   
                {  
  /* YOUR CODE HERE */
  /*  Perhaps perform some error checking? */
  int imm;
  int rd;  
  uint32_t instruction;
  /* to check strtol()  */
  char *tmp = NULL;
  /* U-format requires rd rs rt */
  if (num_args != 2)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* dest register */
  rd = translate_reg(args[0]);  /* The value of rd is updated by translate_reg(args[0]) */
   if (rd == -1)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* check if a parameter is a label */
  if (is_valid_label(args[1])) {  /* It judges the condition. */
    imm = get_addr_for_symbol(symbol, args[1]);  /* It executes a "for" loop */
    if (imm == -1)  /* It judges the condition. */
      return -1;  /* The function returns -1 */
    /* absolute address */
   /*  imm += TEXT_BASE;  */
  }  
  else {
    tmp = NULL;
    imm = strtol(args[1], &tmp, 0);
    if (args[1] == tmp)  /* It judges the condition. */
      return -1;  /* The function returns -1 */
  }  
  /* check validation */
  if (imm < 0 || imm > 1048575)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  write_inst_utype(basic_code, name, rd, imm);  
  instruction = (opcode) + (rd << 7) + (imm << 12);  
  write_inst_hex(machine_code, instruction);  
  return 0;  /* The function returns 0 */
}  
  
/* In this project there is no need to relocate labels,
   you may think about the reasons. */
int write_ujtype(FILE *basic_code, FILE *machine_code, const char *name,  
                 uint8_t opcode, char **args, size_t num_args, uint32_t addr,  
                 SymbolTable *symbol) {  
  /* YOUR CODE HERE */
  /*  Perhaps perform some error checking? */
  uint32_t instruction;
  int imm;
  int rd;
  int label_offset;
  char *tmp;
  if (num_args != 2 || strcmp(name, "jal") != 0) /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* load paremeters */
  rd = translate_reg(args[0]);  /* The value of rd is updated by translate_reg(args[0]) */
   /* check valication */
  if (rd == -1)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* deal with invalid label */
  if (!is_valid_label(args[1]))  /* It judges the condition. */ {
    tmp = NULL;
    label_offset = strtol(args[1], &tmp, 0);
    if (args[1] == tmp) {
      printf("fail\n");
      return -1;
    }
  }
  else {
    label_offset =
    get_addr_for_symbol(symbol, args[1]); /* It executes a "for" loop */
  }
  if (label_offset == -1 ) {
    return -1;
  }
  /* 2^20 = 1048576 */
  imm = label_offset - addr;
  if (imm % 4 != 0)
    return -1;
  printf("here %d %u %d\n", label_offset, addr, imm);
  if (imm < -1048576 || imm > 1048575) { /* judge validation */
    return -1;
  }
  printf("offset: %d\n", imm);
  fprintf(basic_code, "jal x%d %d\n", rd, imm); /* write to file */
  instruction = opcode + (rd << 7) + ((imm >> 12 & 255) << 12) +
                ((imm >> 11 & 1) << 20) + ((imm >> 1 & 1023) << 21) +
                ((imm >> 20 & 1) << 31); /* creat instruciton */
  write_inst_hex(machine_code, instruction);
  return 0;  /* The function returns 0 */
}  
