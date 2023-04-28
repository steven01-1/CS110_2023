/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia
   to be part of the project in Feb 2023.
*/
  
#include "tables.h"  /* include a header */
#include <stdio.h>  /* include a header */
#include <stdlib.h>  /* include a header */
#include <string.h>  /* include a header */
  
const int SYMBOLTBL_NON_UNIQUE = 0;  
const int SYMBOLTBL_UNIQUE_NAME = 1;  
  
/*******************************
 * Helper Functions
 *******************************/
  
void addr_alignment_incorrect() {  
  write_to_log("Error: address is not a multiple of 4.\n");  
}  
  
void name_already_exists(const char *name) {  
  write_to_log("Error: name '%s' already exists in table.\n", name);  
}  
  
void write_sym(FILE *output, uint32_t addr, const char *name) {  
  fprintf(output, "%u\t%s\n", addr, name);  
}  
  
/*******************************
 * Symbol Table Functions
 *******************************/
  
/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time.
   If memory allocation fails, you should call allocation_failed().
   Mode will be either SYMBOLTBL_NON_UNIQUE or SYMBOLTBL_UNIQUE_NAME. You will
   need to store this value for use during add_to_table().
 */
  
SymbolTable *create_table(int mode) {  
  /* YOUR CODE HERE */
  /* creat a symble table */
  SymbolTable *sy_table = malloc(20 * sizeof(SymbolTable));  
  if (!sy_table)  /* It judges the condition. */
    allocation_failed();  
  sy_table->mode = mode;  /* The value of sy_table->mode is updated by mode */
   sy_table->tbl = calloc(20, sizeof(Symbol));  
  if (!sy_table->tbl)  /* It judges the condition. */
    allocation_failed();  
  sy_table->len = 0;  /* The value of sy_table->len is updated by 0 */
   sy_table->cap = 20;  /* The value of sy_table->cap is updated by 20 */
   return sy_table;  /* The function returns sy_table */
}  
  
/* Frees the given SymbolTable and all associated memory. */
  
void free_table(SymbolTable *table) {  
  /* YOUR CODE HERE */
  uint32_t i;  
  for (i = 0; i < table->len; ++i) {  /* It executes a "for" loop */
    free(table->tbl[i].name);  
  }  
  free(table->tbl);  
  free(table);  
}  
  
/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE.
   1. ADDR is given as the byte offset from the first instruction.
   2. The SymbolTable must be able to resize itself as more elements are added.

   3. Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.  done

   4. If ADDR is not word-aligned and check_align==1, you should call
   addr_alignment_incorrect() and return -1.  done

   5. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists
   in the table, you should call name_already_exists() and return -1.  done

   6.If memory allocation fails, you should call allocation_failed().  done

   Otherwise, you should store the symbol name and address and return 0.  done
 */
int add_to_table(SymbolTable *table, const char *name, uint32_t addr,  
                 int check_align) {  
  /* YOUR CODE HERE */
  uint32_t i;  
  char *new_name;  
  if (check_align == 1)  /* It judges the condition. */
    addr_alignment_incorrect();  
  /* name-unique check */
  if (table->mode == SYMBOLTBL_UNIQUE_NAME) {  /* It judges the condition. */
    int flag = 0;  
    for (i = 0; i < table->len; i++) {  /* It executes a "for" loop */
      if (strcmp(table->tbl[i].name, name) == 0) {  /* It judges the condition. */
        flag = 1;  /* The value of flag is updated by 1 */
         break;  
      }  
    }  
    if (flag) {  /* It judges the condition. */
      name_already_exists(name);  
      return -1;  /* The function returns -1 */
    }  
  }  
  /* it is full */
  if (table->len == table->cap) {  /* It judges the condition. */
    Symbol *new_syb = calloc(2 * table->cap, sizeof(Symbol));  
    if (!new_syb)  /* It judges the condition. */
      allocation_failed();  
    for (i = 0; i < table->len; i++) {  /* It executes a "for" loop */
      new_syb[i].addr = table->tbl[i].addr;  /* The value of new_syb[i].addr is updated by table->tbl[i].addr */
       new_syb[i].name = table->tbl[i].name;  /* The value of new_syb[i].name is updated by table->tbl[i].name */
     }  
    free(table->tbl);  
    table->tbl = new_syb;  /* The value of table->tbl is updated by new_syb */
     table->cap *= 2;  
  }  
  /* add element */
  new_name = calloc(strlen(name) + 1, sizeof(char));  
  strcpy(new_name, name);  
  table->tbl[table->len].name = new_name;  /* The value of table->tbl[table->len].name is updated by new_name */
   table->tbl[table->len].addr = addr;  /* The value of table->tbl[table->len].addr is updated by addr */
   table->len++;  
  return 0;  /* The function returns 0 */
}  
  
/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable *table, const char *name) {  /* It executes a "for" loop */
  /* YOUR CODE HERE */
  uint32_t i;  
  int flag = 0;  
  int pos;  
  for (i = 0; i < table->len; i++) {  /* It executes a "for" loop */
    if (strcmp(table->tbl[i].name, name) == 0) {  /* It judges the condition. */
      flag = 1;  /* The value of flag is updated by 1 */
       pos = i;  /* The value of pos is updated by i */
       break;  
    }  
  }  
  if (!flag)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  return table->tbl[pos].addr;  /* The function returns table->tbl[pos].addr */
}  
  
/* Writes the SymbolTable TABLE to OUTPUT. You should use write_sym() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable *table, FILE *output) {  
  /* YOUR CODE HERE */
  uint32_t i;  
  for (i = 0; i < table->len; i++) {  /* It executes a "for" loop */
    write_sym(output, table->tbl[i].addr, table->tbl[i].name);  
  }  
}  
  
/* DO NOT MODIFY THIS LINE BELOW */
/* ===================================================================== */
/* Read the symbol table file written by the `write_table` function, and
   restore the symbol table.
 */
SymbolTable *create_table_from_file(int mode, FILE *file) {  
  SymbolTable *symtbl = create_table(mode);  
  char buf[1024];  
  while (fgets(buf, sizeof(buf), file)) {  /* It executes a "while" loop */
    char *name, *addr_str, *end_ptr;  
    uint32_t addr;  
    addr_str = strtok(buf, "\t");  
    /* create table based on these tokens */
    addr = strtol(addr_str, &end_ptr, 0);  
    name = strtok(NULL, "\n");  
    /* add the name to table, one by one */
    add_to_table(symtbl, name, addr, 0);  
  }  
  return symtbl;  /* The function returns symtbl */
}  
