#include <stdio.h>  /* include a header */
#include <stdlib.h>  /* include a header */
#include "emulator.h"  /* include a header */
#include "execute.h"  /* include a header */
#include "logs.h"  /* include a header */
#include "mem.h"  /* include a header */


#define MAX_SIZE 50

/* WARNING: DO NOT CHANGE THIS FILE.
   YOU PROBABLY DON'T EVEN NEED TO LOOK AT IT... */

int run(Emulator *emulator, int mode)
{
    int i = 0;
    if (emulator->cpu.stat!=1 || emulator->cpu.PC >= emulator->end)  /* It judges the condition. */
        return -1;  /* The function returns -1 */
    for (i = 0;; i++)  /* It executes a "for" loop */
    {
        if(step(emulator, mode)==-1)  /* It judges the condition. */ {
            return 0;  /* The function returns 0 */
        }
    }
    return 0;  /* The function returns 0 */
}

int step(Emulator *emulator, int mode)
{
    Instruction instruction;
    int i,j,k;

    /* check cpu stat and current PC*/
    if (emulator->cpu.stat!=1 || emulator->cpu.PC >= emulator->end)  /* It judges the condition. */
        return -1;  /* The function returns -1 */
    
    /* enforce $0 being hard-wired to 0 */
    emulator->cpu.reg[0] = 0;  /* The value of emulator->cpu.reg[0] is updated by 0 */

    /* fetch an instruction */
    instruction.bits = load(emulator->memory, emulator->cpu.PC, LENGTH_WORD, 1);

    /* recode an undo log of instruction */
    record(instruction, &(emulator->cpu), emulator->memory, emulator->logs);

    /* execute an instruction */
    execute(instruction, &(emulator->cpu), emulator->memory);

    /* enforce $0 being hard-wired to 0 */
    emulator->cpu.reg[0] = 0;  /* The value of emulator->cpu.reg[0] is updated by 0 */
 
    /* trace in mode 1 */
    if(mode==1){  /* It judges the condition. */
        for(i=0;i<8;i++){  /* It executes a "for" loop */
            for(j=0;j<4;j++){  /* It executes a "for" loop */
                k=4*i+j;  /* The value of k is updated by 4*i+j */
                 if(k<10)  /* It judges the condition. */
                    write_to_log(" x%d=0x%08x ",k,emulator->cpu.reg[k]);
                else
                    write_to_log("x%d=0x%08x ",k,emulator->cpu.reg[k]);
            }
            write_to_log("\n");
        }
        write_to_log("\n");
    }

    /* check next PC */
    if (emulator->cpu.PC >= emulator->end && emulator->cpu.stat==1){  /* It judges the condition. */
        emulator->cpu.stat=0;  /* The value of emulator->cpu.stat is updated by 0 */
         write_to_log("Exited with error code %d.\n", emulator->cpu.reg[10]);
    }

    return 0;  /* The function returns 0 */
}


int prev(Emulator *emulator)
{
    if (undo(&(emulator->cpu), emulator->memory, emulator->logs)==-1)  /* It judges the condition. */
        return -1;  /* The function returns -1 */
    emulator->cpu.stat=1;  /* The value of emulator->cpu.stat is updated by 1 */
     return 0;  /* The function returns 0 */
}

int trace(Emulator *emulator)
{
    return run(emulator, 1);
}

int dump(Emulator *emulator)
{
    Address pc = TEXT_BASE;
    Instruction instruction;
    while(pc < emulator->end){  /* It executes a "while" loop */
        instruction.bits = load(emulator->memory, pc, LENGTH_WORD, 1);
        write_to_log("%08x\n", instruction.bits);
        pc+=4;  /* The value of pc+ is updated by 4 */
     }
    return 0;  /* The function returns 0 */
}

void load_program(Emulator *emulator, FILE *text, FILE *data)
{
    char line[MAX_SIZE];
    int instruction, offset = 0;

    /* load text */
    if (text)  /* It judges the condition. */
    {
        rewind(text);
        while (fgets(line, MAX_SIZE, text) != NULL)  /* It executes a "while" loop */
        {
            instruction = (int32_t)strtol(line, NULL, 16);
            emulator->memory[MEMORY_TEXT + offset] = instruction & 0xFF;
            emulator->memory[MEMORY_TEXT + offset + 1] =
                (instruction >> 8) & 0xFF;
            emulator->memory[MEMORY_TEXT + offset + 2] =
                (instruction >> 16) & 0xFF;
            emulator->memory[MEMORY_TEXT + offset + 3] =
                (instruction >> 24) & 0xFF;
            offset += 4;
        }

        /* record the end address of the program */
        emulator->end = MEMORY_TEXT + offset;
    }
    
    instruction = 0;  /* The value of instruction is updated by 0 */
     offset = 0;  /* The value of offset is updated by 0 */
 
    /* load data */
    if (data)  /* It judges the condition. */
    {
        rewind(data);
        while (fgets(line, MAX_SIZE, data) != NULL)  /* It executes a "while" loop */
        {
            instruction = (int32_t)strtol(line, NULL, 16);
            emulator->memory[MEMORY_DATA + offset] = instruction & 0xFF;
            emulator->memory[MEMORY_DATA + offset + 1] =
                (instruction >> 8) & 0xFF;
            emulator->memory[MEMORY_DATA + offset + 2] =
                (instruction >> 16) & 0xFF;
            emulator->memory[MEMORY_DATA + offset + 3] =
                (instruction >> 24) & 0xFF;
            offset += 4;
        }
    }
}

Emulator *create_emulator(FILE *text, FILE *data)
{
    Emulator *emulator;
    emulator = (Emulator *)calloc(1, sizeof(Emulator));
    if(emulator==NULL)  /* It judges the condition. */
        return NULL;  /* The function returns NULL */

    emulator->memory = (Byte *)calloc(MEMORY_SIZE, sizeof(Byte));
    if((emulator->memory)==NULL) {  /* It judges the condition. */
        free(emulator);
        return NULL;  /* The function returns NULL */
    }

    emulator->logs = create_logs();  /* The value of emulator->logs is updated by create_logs() */
     
    load_program(emulator, text, data);
    /* set stack pointer */
    emulator->cpu.reg[2]=STACK_BASE;  /* The value of emulator->cpu.reg[2] is updated by STACK_BASE */
     /* set global pointer */
    emulator->cpu.reg[3]=DATA_BASE;  /* The value of emulator->cpu.reg[3] is updated by DATA_BASE */
     emulator->cpu.stat=1;  /* The value of emulator->cpu.stat is updated by 1 */
     return emulator;  /* The function returns emulator */
}

int free_emulator(Emulator *emulator)
{
    free_logs(emulator->logs);
    free(emulator->memory);
    free(emulator);
    return 0;  /* The function returns 0 */
}
