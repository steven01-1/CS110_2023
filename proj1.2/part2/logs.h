#ifndef LOGS_H
#define LOGS_H

#include "emulator.h"
#include "mem.h"


/* Complete the following definition of Logs and implement the following
   functions. You are free to add structs or functions, but you must keep
   structure Logs. And you cannot change a function that has already been given.
 */

 /* Add structure definition here if you need */
typedef struct {
    short log_data_type; /* 0 = None, 1 = Memory, 2 = Register*/
    short mem_length; /* The length of the data stored in memory */
    Word addr; /* The addresss of memory or register. The address is real address for memory */
    unsigned val; /* The value */
    unsigned pc_val; /* The value of PC*/
} Stack;


typedef struct
{
    /* YOUR CODE HERE */
    Stack* stack;
    /* the index of each log */
    int top;
    int size;
} Logs;

/* IMPLEMENT ME - see documentation in logs.c */
Logs *create_logs();

/* IMPLEMENT ME - see documentation in logs.c */
void free_logs(Logs *logs);

/* IMPLEMENT ME - see documentation in logs.c */
void record(Instruction inst, Processor *cpu, Byte *mem, Logs *logs);

/* IMPLEMENT ME - see documentation in logs.c */
int undo(Processor *cpu, Byte *mem, Logs *logs);

/* Add function declaration here if you need */

#endif