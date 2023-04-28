#include <stdlib.h>  /* include a header */
#include <string.h>  /* include a header */
#include <stdio.h>  /* include a header */
#include "mem.h"  /* include a header */


/*******************************
 * Helper Functions
 *******************************/

/* Call this function in the following cases:
        1. Attempts to access a memory location that it is not allowed to access.
        2. Attempts to access a memory location in a way that is not allowed.
 */
static void raise_segmentation_fault(Address addr) {
   write_to_log("Error - segmentation fault at address %08x\n", addr);
}

/* Call this function if a data access is performed to an address 
   that is not aligned for the size of the access.
 */
static void raise_alignment_fault(Address addr) {
    write_to_log("Error - alignment fault at address %08x\n", addr);
}

/* This function is helpful to your alignment check.
 */
int check_alignment(Address address, Alignment alignment) {
    if (alignment == LENGTH_BYTE)  /* It judges the condition. */
    {
        return 1;  /* The function returns 1 */
    }
    /* HALF_WORD: The last bit is not used. */
    else if (alignment == LENGTH_HALF_WORD)  /* It judges the condition. */
    {
        return address % 2 == 0;
    }
    /* WORD: The last two bits are not used. */
    else if (alignment == LENGTH_WORD)  /* It judges the condition. */
    {
        return address % 4 == 0;
    }
    else {
        return 0;  /* The function returns 0 */
    }
}

/* To store the data in the corresponding address in memory, 
   please note the following points:
        1. When check_align==1, check if the memory address is aligned
         with the data type. You need to use raise_alignment_fault().
        2. Think about when you need to use raise_segmentation_fault().
        3. Please perform an address conversion before actually store to memory.
 */

void store(Byte *memory, Address address, Alignment alignment, Word value,
           int check_align)
{
    /* YOUR CODE HERE */
    unsigned int i = 0; /* The iterator */
    int mem_offset = 0; /* The offset from the start of the memory*/
    /* Check for whether virtual memory can be transferred to physical memory*/
    /* stack real: 0x80000 ~ 0x10 0000 , length: 0x80000
    *  stack vertual: 0x7fff fffc ~ 0x7FF7 FFFC
     */
    /* the '=' is not sure !!! */
    if ((address >= (unsigned)0x10040000 && address <= (unsigned)0x7FF7FFFC) || address < (unsigned)0x10000000   /* It judges the condition. */
        || address > (unsigned) STACK_BASE) {
        raise_segmentation_fault(address);
        return;
    }
    /* the following address are in the range. */
    /* Data */
    if (address >= (unsigned)DATA_BASE && address < (unsigned)0x10040000 ) {  /* It judges the condition. */
      /* Check if the memroy address is aligned with the data type */
      if (check_align == 1) {  /* It judges the condition. */
        if (check_alignment(address, alignment) == 0) {  /* It judges the condition. */
          raise_alignment_fault(address + (unsigned)0x40000);
          return;
        }
      }
      /* set the mem offset */
        mem_offset = address - DATA_BASE;
        i = 0;  /* The value of i is updated by 0 */
         /* updata the memory */
        while (i < alignment) {  /* It executes a "while" loop */
            memory[mem_offset + i + 0x40000] = value & 0xff;
            value >>= 8;
            i++;
        }
    }
    /* STACK */
    if (address >= (unsigned)0x7FF7FFFC) {  /* It judges the condition. */
      /* Check if the memroy address is aligned with the data type */
      if (check_align == 1) {  /* It judges the condition. */
        if (check_alignment(address, alignment) == 0) {  /* It judges the condition. */
          raise_alignment_fault((unsigned)0x100000 - (STACK_BASE - address));
          return;
        }
      }
        mem_offset = STACK_BASE - address;
        i = 0;  /* The value of i is updated by 0 */
         while (i < alignment) {  /* It executes a "while" loop */
            memory[(unsigned)0x100000 - mem_offset + i] =  value & 0xff;
            value >>= 8;
            i++;
        }
    }
    return;
}

/* To load the data in the corresponding address in memory, 
   please note the following points:
        1. When check_align==1, check if the memory address is aligned
         with the data type. You need to use raise_alignment_fault().
        2. Think about when you need to use raise_segmentation_fault().
        3. Please perform an address conversion before actually load from memory.
 */
Word load(Byte *memory, Address address, Alignment alignment, int check_align)
{
    /* YOUR CODE HERE */
    unsigned int i = 0; /* The iterator */
    int mem_offset = 0; /* The offset from the start of the memory*/
    Word ret = 0; /* The value loaded from the memory */

    /* Check for whether virtual memory can be transferred to physical memory*/
    /* stack */
    if (address > STACK_BASE) {  /* It judges the condition. */
        raise_segmentation_fault(address);
        return 0;  /* The function returns 0 */
    }
    /* data */
    if (address >= (unsigned)0x10040000 && address < (unsigned)0x7FF7FFFC ) {  /* It judges the condition. */
        raise_segmentation_fault(address);
        return 0;  /* The function returns 0 */
    }
    if (address >= (unsigned) 0x40000 && address < (unsigned)0x10000000) {  /* It judges the condition. */
        raise_segmentation_fault(address);
        return 0;  /* The function returns 0 */
    }

    /* the following address are in the range. */
    /* Data */
    if (address >= DATA_BASE && address <= (unsigned)0x10040000) {  /* It judges the condition. */
    /* Check if the memroy address is aligned with the data type */
    if (check_align == 1) {  /* It judges the condition. */
        if (check_alignment(address, alignment) == 0) {  /* It judges the condition. */
            raise_alignment_fault(address - DATA_BASE + (unsigned)0x40000);
            return 0;  /* The function returns 0 */
        }
    }
        mem_offset = address - DATA_BASE;
        i = 0;  /* The value of i is updated by 0 */
         while (i < alignment) {  /* It executes a "while" loop */
            ret += memory[mem_offset + 0x40000 + i] << 8 * i;
            i++;
        }
    }
    /* Text */
    else if ( address < DATA_BASE) {  /* It judges the condition. */
    /* Check if the memroy address is aligned with the data type */
    if (check_align == 1) {   /* It judges the condition. */
        if (check_alignment(address, alignment) == 0) {  /* It judges the condition. */
            raise_alignment_fault(address);
            return 0;  /* The function returns 0 */
        }
    }
        mem_offset = address - TEXT_BASE;
        i = 0;  /* The value of i is updated by 0 */
         while (i < alignment) {  /* It executes a "while" loop */
            ret += memory[mem_offset + i] << 8*i ;
            i++;
        }
    }
    /* Stack */
    else if (address >= (unsigned)0x7FF7FFFC && address <= (unsigned)0x7FFFFFFC ) {   /* It judges the condition. */
    /* Check if the memroy address is aligned with the data type */
    if (check_align == 1) {   /* It judges the condition. */
        if (check_alignment(address, alignment) == 0) {  /* It judges the condition. */
            raise_alignment_fault((unsigned)0x100000 - (STACK_BASE - address));
            return 0;  /* The function returns 0 */
        }
    }        
        mem_offset = STACK_BASE - address;
        i = 0;  /* The value of i is updated by 0 */
         while (i < alignment) {  /* It executes a "while" loop */
            ret += memory[(unsigned)0x100000 - mem_offset + i] << 8 * i;
            i++;
        }
    }
    return ret;  /* The function returns ret */
}