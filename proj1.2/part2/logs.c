#include <stdlib.h>  /* include a header */
#include <string.h>  /* include a header */
#include "logs.h"  /* include a header */
#include "execute_utils.h"  /* include a header */


/*******************************
 * Helper Functions
 *******************************/

/* Creates a new empty Logs and returns a pointer to that Logs. 
   If memory allocation fails, you should call allocation_failed().
 */
Logs *create_logs() {
    /* YOUR CODE HERE */
    Logs* log_pointer = malloc(sizeof(Logs));
    Stack* stack_pointer;
    if (log_pointer == NULL) {  /* It judges the condition. */
        allocation_failed();
        return NULL;
    }
    stack_pointer = malloc(1000000 * sizeof(Stack));
    if (stack_pointer == NULL) {  /* It judges the condition. */
        free(log_pointer);
        allocation_failed();
        return NULL;
    }
    log_pointer->stack = stack_pointer;  /* The value of log_pointer->stack is updated by stack_pointer */
    log_pointer->top = 0;  /* The value of log_pointer->top is updated by -1 */
    log_pointer->size = 1000000;  /* The value of log_pointer->size is updated by 1 */
    return log_pointer;  /* The function returns log_pointer */
}

/* Frees the given Logs and all associated memory. */
void free_logs(Logs *logs) {
    /* YOUR CODE HERE */
    free(logs->stack);
    free(logs);
    return;
}

/* Record the current status and save in a logs component.
   Think about what information needs to be recorded.
   Logs can resize to fit an arbitrary number of entries. 
   (so you should use dynamic memory allocation).
 */
void record(Instruction inst, Processor *cpu, Byte *mem, Logs *logs) {


    /* (log_state_type) encode = 2xx. x1: reg, x2: mem */
    /* for expanding Log */
    /*Stack *tmp = NULL;*/
    Address address;
    Stack *tmp = NULL; 
    
    /* R-type: only reg*/
    if (inst.opcode.opcode == 0x33) {
        /* save pc's value */
        logs->stack[logs->top].pc_val = cpu->PC;
        /* save rd's value */
        logs->stack[logs->top].addr = inst.rtype.rd;
        logs->stack[logs->top].val = cpu->reg[inst.rtype.rd];
        logs->stack[logs->top].log_data_type = 210;
    }
    /* I-type except jalr/ecall: only reg */
    else if (inst.opcode.opcode == 0x03 || inst.opcode.opcode == 0x13) {
        /* save pc's value */
        logs->stack[logs->top].pc_val = cpu->PC;
        /* save rd's value */
        logs->stack[logs->top].addr = inst.itype.rd;
        logs->stack[logs->top].val = cpu->reg[inst.itype.rd];
        logs->stack[logs->top].log_data_type = 210;
    }
    /* jalr: change reg and PC */
    else if (inst.opcode.opcode == 0x67) {
        /* save pc's value */
        logs->stack[logs->top].pc_val = cpu->PC;
        /* save rd's value */
        logs->stack[logs->top].addr = inst.itype.rd;
        logs->stack[logs->top].val = cpu->reg[inst.itype.rd];
        logs->stack[logs->top].log_data_type = 210;
    }
    /* ecall : none */
    else if (inst.opcode.opcode == 0x73) {
        /* save pc's value */
        logs->stack[logs->top].pc_val = cpu->PC;
        logs->stack[logs->top].log_data_type = 200;
    }
    /* S-type: change mem */
    else if (inst.opcode.opcode == 0x23) {
        /* save pc's value */
        logs->stack[logs->top].pc_val = cpu->PC;
        /* set type information */
        switch (inst.stype.funct3) {
            case 0:
                /* byte */
                logs->stack[logs->top].mem_length =
                LENGTH_BYTE; /* The value of val_length is updated by LENGTH_BYTE */
                break;
                /* half */
            case 1:
                logs->stack[logs->top].mem_length = LENGTH_HALF_WORD; /* The value of val_length is
                                                                        updated by LENGTH_HALF_WORD */
                break;
                /* word */
            case 2:
                logs->stack[logs->top].mem_length =
                LENGTH_WORD; /* The value of val_length is updated by LENGTH_WORD */
                break;
            default:
                return;
        }
        /* save mem's value */
        /* data */
        address = cpu->reg[inst.stype.rs1] + get_store_offset(inst);
        if (address >= 0x10000000 && address < 0x10040000) {
            logs->stack[logs->top].log_data_type = 201;
            address = address - DATA_BASE + 0x40000;
            /* size will be handled by undo() */
            /* judge the size of data */
            /* word */
            if (logs->stack[logs->top].mem_length == LENGTH_WORD) {
                logs->stack[logs->top].val = mem[address] +  ((unsigned int)mem[address + 1] << 8) +
                                                ((unsigned int)mem[address + 2] << 16) + 
                                                ((unsigned int)mem[address + 3] << 24);
            }
            /* half */
            else if (logs->stack[logs->top].mem_length == LENGTH_HALF_WORD) {
                logs->stack[logs->top].val = mem[address] + ((unsigned int)mem[address + 1] << 8);
            }
            /* byte */
            else if (logs->stack[logs->top].mem_length == LENGTH_BYTE) {
                logs->stack[logs->top].val = mem[address];
            }
            /* addr should be the real address. for undo() */
            logs->stack[logs->top].addr = address;
        }
        /* stack */
        else if (address <= 0x7FFFFFFC && address >= 0x7FF7FFFC) {
            logs->stack[logs->top].log_data_type = 201;
            address = 0x100000 - (STACK_BASE - address);
            /* size will be handled by undo() */
            /* judge the size of data */
            if (logs->stack[logs->top].mem_length == LENGTH_WORD) {
                logs->stack[logs->top].val = mem[address] +  ((unsigned int)mem[address + 1] << 8) +
                                            ((unsigned int)mem[address + 2] << 16) + 
                                            ((unsigned int)mem[address + 3] << 24);
            }
            /* half */
            else if (logs->stack[logs->top].mem_length == LENGTH_HALF_WORD) {
                logs->stack[logs->top].val = mem[address] + ((unsigned int)mem[address + 1] << 8);
            }
            /* byte */
            else if (logs->stack[logs->top].mem_length == LENGTH_BYTE) {
                logs->stack[logs->top].val = mem[address];
            }
            /*   logs->stack[logs->top].val = mem[address] + (mem[address + 1] << 8) +
                                            (mem[address + 2] << 16) + (mem[address + 3] << 24); */
            logs->stack[logs->top].addr = address;
        }
        else {
            logs->stack[logs->top].log_data_type = 200;
            return;
        }
    }
    /* SB-type: PC */
    else if (inst.opcode.opcode == 0x63) {
        /* save pc's value */
        logs->stack[logs->top].pc_val = cpu->PC;
        logs->stack[logs->top].log_data_type = 200;
    }
    /* U-type: reg */
    else if (inst.opcode.opcode == 0x37 || inst.opcode.opcode == 0x17) {
        /* save pc's value */
        logs->stack[logs->top].pc_val = cpu->PC;
        /* save rd's value */
        logs->stack[logs->top].addr = inst.utype.rd;
        logs->stack[logs->top].val = cpu->reg[inst.utype.rd];
        logs->stack[logs->top].log_data_type = 210;
    }
    /* UJ-type: reg and PC */
    else if (inst.opcode.opcode == 0x6f) {
        /* save pc's value */
        logs->stack[logs->top].pc_val = cpu->PC;
        /* save rd's value */
        logs->stack[logs->top].addr = inst.ujtype.rd;
        logs->stack[logs->top].val = cpu->reg[inst.ujtype.rd];
        logs->stack[logs->top].log_data_type = 210;
    }
    else {
        return;
    }
    logs->top++;
    /* Check whether the stack is overflow */
    if (logs->top >= logs->size) {
        logs->size *= 2;
        /* sometimes it copy the whole data to a new area !!! */
        /*tmp = logs->stack;*/
        tmp = realloc(logs->stack, logs->size * sizeof(Stack));
        if (tmp == NULL) { /* It judges the condition. */
            /*!!!!!!!!!! caution here !!!!!!!!!*/
            /* free(tmp); */
            allocation_failed();
            /* memory leak ??? */
            return;
        }
        logs->stack = tmp;
    }
}


void record11111(Instruction inst, Processor *cpu, Byte *mem, Logs *logs)
{
    /* YOUR CODE HERE */
    Word rd;
    Word address;
    int rs2;
    /* The iterator */
    int i = 0; 
    int val_length = 0;
    int mem_offset = 0;
    /* for expanding Log */
    Stack *tmp = NULL;
    /* an index for each log */
    logs->top++;
    /* Check whether the stack is overflow */
    if (logs->top >= logs->size) { 
        logs->size *= 2;
        /* sometimes it copy the whole data to a new area !!! */
        tmp = realloc(logs->stack, logs->size * sizeof(Stack));
        if (tmp == NULL) {  /* It judges the condition. */
            allocation_failed();
            /* memory leak ??? */
            return;
        }
        logs->stack = tmp;
    }
    /* SB type, only PC has changed */
    if (inst.opcode.opcode == 0x63) { 
        logs->stack[logs->top].log_data_type = 0;  /* The value of logs->stack[logs->top].log_data_type is updated by 0 */
        logs->stack[logs->top].pc_val = cpu->PC;  /* The value of logs->stack[logs->top].pc_val is updated by cpu->PC */
     }
    /* S-type, the value in memory has changed */
    else if (inst.opcode.opcode == 0x23) {
        logs->stack[logs->top].log_data_type = 1;  /* The value of logs->stack[logs->top].log_data_type is updated by 1 */
        logs->stack[logs->top].pc_val = cpu->PC;  /* The value of logs->stack[logs->top].pc_val is updated by cpu->PC */
        rs2 = inst.stype.rs2;  /* The value of rs2 is updated by inst.stype.rs2 */
        address = cpu->reg[rs2] + get_store_offset(inst);
        if ((address < STACK_BASE && address - DATA_BASE >= 0x40000)   /* It judges the condition. */
            || (address < DATA_BASE && address - TEXT_BASE >= 0x40000) 
            || address >= 0x80000000) {
            return;
        }
        switch (inst.stype.funct3) {
            case 0:
                val_length = LENGTH_BYTE;  /* The value of val_length is updated by LENGTH_BYTE */
                 break;
            case 1:
                val_length = LENGTH_HALF_WORD;  /* The value of val_length is updated by LENGTH_HALF_WORD */
                 break;
            case 2:
                val_length = LENGTH_WORD;  /* The value of val_length is updated by LENGTH_WORD */
                 break;
            default:
                return;
                break;
        }
        logs->stack[logs->top].mem_length = val_length;  /* The value of logs->stack[logs->top].mem_length is updated by val_length */
        i = 0;  /* The value of i is updated by 0 */
        logs->stack[logs->top].val = 0;  /* The value of logs->stack[logs->top].val is updated by 0 */
        if (address >= DATA_BASE && address < STACK_BASE) { /* Data */
            mem_offset = address - DATA_BASE;
            i = val_length - 1;  /* The value of i is updated by 0 */
            logs->stack[logs->top].val = 0;
            while (i >= 0) {  /* It executes a "while" loop */
                logs->stack[logs->top].val <<= 8;
                logs->stack[logs->top].val += mem[mem_offset + 0x40000 + i];
                i--;
            }
        }
        else if (address < DATA_BASE) { /* Text */
            mem_offset = address - TEXT_BASE;
            i = val_length - 1;  /* The value of i is updated by 0 */
            logs->stack[logs->top].val = 0;
            while (i >= 0) {  /* It executes a "while" loop */
                logs->stack[logs->top].val <<= 8;
                logs->stack[logs->top].val += mem[mem_offset + i];
                i--;
            }
        }
        else if (address >= STACK_BASE) { /* Stack */
            mem_offset = address - STACK_BASE;
            i = val_length - 1;  /* The value of i is updated by 0 */
            logs->stack[logs->top].val = 0;
            while (i >= 0) {  /* It executes a "while" loop */
                logs->stack[logs->top].val <<= 8;
                logs->stack[logs->top].val += mem[mem_offset + 0x80000 + i];
                i--;
            }
        }
         /* To be done */
    }
    else { /* Other types, the value of register has changed */
        logs->stack[logs->top].log_data_type = 2;  /* The value of logs->stack[logs->top].log_data_type is updated by 2 */
        logs->stack[logs->top].pc_val = cpu->PC;  /* The value of logs->stack[logs->top].pc_val is updated by cpu->PC */
        rd = inst.rtype.rd;  /* The value of rd is updated by inst.rtype.rd */
        logs->stack[logs->top].addr = rd;  /* The value of logs->stack[logs->top].addr is updated by rd */
        logs->stack[logs->top].val = cpu->reg[rd];  /* The value of logs->stack[logs->top].val is updated by cpu->reg[i] */
     }
    return;
}

/* Back to the previous state recorded in a logs component.
   if success, return 0.
   if fail, for example, no previous state, return -1.
 */
/* top points to the first empty stack! */
int undo(Processor *cpu, Byte *mem, Logs *logs) {
    int top;
    Stack *stk_top;
    /* top stack */
    if (logs->top <= 0) { /* No previous state */
        return -1; /* The function returns -1 */
    }
    logs->top--;
    top = logs->top;
    stk_top = &(logs->stack[top]);
    /* reg and mem can't be changed at the same time */
    /* reg */
    if (stk_top->log_data_type /10 == 21) {
        cpu->reg[stk_top->addr] = stk_top->val;
    }
    /* mem */
    else if (stk_top->log_data_type % 10 == 1) {
        /* for sw */
        if (stk_top->mem_length == LENGTH_WORD) {
            mem[stk_top->addr] = stk_top->val & 0xFF;
            mem[stk_top->addr + 1] = (stk_top->val >> 8) & 0xFF;
            mem[stk_top->addr + 2] = (stk_top->val >> 16) & 0xFF;
            mem[stk_top->addr + 3] = (stk_top->val >> 24) & 0xFF;
        }
        /* for sh */
        else if (stk_top->mem_length == LENGTH_HALF_WORD) {
            mem[stk_top->addr] = stk_top->val & 0xFF;
            mem[stk_top->addr + 1] = (stk_top->val >> 8) & 0xFF;
        }
        /* for sb */
        else if (stk_top->mem_length == LENGTH_BYTE) {
            mem[stk_top->addr] = stk_top->val & 0xFF;
        }
    }
    cpu->PC = stk_top->pc_val;
    return 0;
}

int undo1111(Processor *cpu, Byte *mem, Logs *logs) {
    /* YOUR CODE HERE */
    int i = 0; /* The iterator */
    if (logs->top == 0) { /* No previous state */
        return -1;  /* The function returns -1 */
    }
    if (logs->stack[logs->top].log_data_type == 0) {   /* It judges the condition. */
        cpu->PC = logs->stack[logs->top].pc_val;  /* The value of cpu->PC is updated by logs->stack[logs->top].pc_val */
     }
    if (logs->stack[logs->top].log_data_type == 1) {  /* It judges the condition. */
        cpu->PC = logs->stack[logs->top].pc_val;  /* The value of cpu->PC is updated by logs->stack[logs->top].pc_val */
        while (i < logs->stack[logs->top].mem_length) {  /* It executes a "while" loop */
            *(mem + logs->stack[logs->top].addr + i) = logs->stack[logs->top].val;
            i++;
        }
    }
    if (logs->stack[logs->top].log_data_type == 2) {  /* It judges the condition. */
        cpu->PC = logs->stack[logs->top].pc_val;  /* The value of cpu->PC is updated by logs->stack[logs->top].pc_val */
        cpu->reg[logs->stack[logs->top].addr] = logs->stack[logs->top].val;  /* The value of cpu->reg[logs->stack[logs->top].addr] is updated by logs->stack[logs->top].val */
     }
    logs->top--;
    return 0;  /* The function returns 0 */
}

/* Add function definition here if you need */
