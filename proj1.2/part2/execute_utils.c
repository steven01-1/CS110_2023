#include <stdio.h>  /* include a header */
#include <stdlib.h>  /* include a header */
#include "execute_utils.h"  /* include a header */


/* You may find implementing this function helpful */

/* Signed extension of a number to 32 bits. 
   size is the number of valid bits of field.
   e.g. field=0x8a, size=8, return 0xFFFFFF8a.
        field=0x7a, size=8, return 0x0000007a.
 */
int bitSigner(unsigned int field, unsigned int size)
{
    /* YOUR CODE HERE */
    int ans = 0;
    if (field & (1 << (size - 1))) { /* Check whether field is negative */
        ans = (field & ((1 << (size - 1)) - 1)) - (1 << (size - 1));
        return ans;  /* The function returns ans */
    }
    else {
        return field;  /* The function returns field */
    }
}

/* Return the imm from instruction, for details, please refer to I type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_imm_operand(Instruction instruction)
{
    /* YOUR CODE HERE */
    /* for srai */
    if (instruction.itype.funct3 == 5 && (instruction.itype.imm >> 10 & 1) == 1) {  /* It judges the condition. */
      /* 0b0001 1111 = 31 */
      return instruction.itype.imm & 31;
    }  
    
    return bitSigner(instruction.itype.imm, 12);  /* The function returns instruction.itype.imm */
}
/* Return the offset from instruction, for details, please refer to SB type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_branch_offset(Instruction instruction)
{
    /* YOUR CODE HERE */
    unsigned int ans = 0;
    ans += instruction.sbtype.imm5 & 0x1e; /* 1 to 4 bits */
    ans += ((instruction.sbtype.imm7 & 0x3f) << 5); /* 5 to 10 bits */
    ans += ((instruction.sbtype.imm5 & 1) << 11); /* The 11th bit */
    ans += ((instruction.sbtype.imm7 & 0x40) << 6); /* The 12th bit */
    return bitSigner(ans, 13); /* Set the signer */
}

/* Return the offset from instruction, for details, please refer to UJ type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_jump_offset(Instruction instruction)
{
    /* YOUR CODE HERE */
    unsigned int ans = 0;
    ans += ((instruction.ujtype.imm & 0x7fe00) >> 8); /* 1 to 10 bits*/
    ans += ((instruction.ujtype.imm & 0x100) << 3); /* The 11th bit */
    ans += ((instruction.ujtype.imm & 0xff) << 12); /* 12 to 19 bits */
    ans += ((instruction.ujtype.imm & 0x80000) << 1); /*The 20 th bit */
    return bitSigner(ans, 21);
}

/* Return the offset from instruction, for details, please refer to S type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_store_offset(Instruction instruction)
{
    /* YOUR CODE HERE */
    unsigned int ans = 0;
    ans += instruction.stype.imm5;
    ans += (instruction.stype.imm7 << 5);
    return bitSigner(ans, 12);
}
