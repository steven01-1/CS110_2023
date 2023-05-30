/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/
#include <stdio.h>  /* include a header */
#include <stdlib.h>  /* include a header */
#include <string.h>  /* include a header */
  
#include "assembler.h"  /* include a header */
#include "tables.h"  /* include a header */
#include "translate.h"  /* include a header */
#include "translate_utils.h"  /* include a header */
  
#define MAX_ARGS 3  
#define BUF_SIZE 1024  
const char *IGNORE_CHARS = " \f\n\r\t\v,()";  
  
/*******************************
 * Helper Functions
 *******************************/
  
/* you should not be calling this function yourself. */
static void raise_label_error(uint32_t input_line, const char *label)  
{  
    write_to_log("Error - invalid label at line %d: %s\n", input_line, label);  
}  
  
/* call this function if more than MAX_ARGS arguments are found while parsing
   arguments.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.

   EXTRA_ARG should contain the first extra argument encountered.
 */
static void raise_extra_argument_error(uint32_t input_line,  
                                       const char *extra_arg)  
{  
    write_to_log("Error - extra argument at line %d: %s\n", input_line,  
                 extra_arg);  
}  
  
/* You should call this function if write_original_code() or translate_inst()
   returns 0.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.
 */
static void raise_instruction_error(uint32_t input_line, const char *name,  
                                    char **args, int num_args)  
{  
  
    write_to_log("Error - invalid instruction at line %d: ", input_line);  
    log_inst(name, args, num_args);  
}  
  
/* Truncates the string at the first occurrence of the '#' character. */
static void skip_comments(char *str)  
{  
    char *comment_start = strchr(str, '#');  
    if (comment_start)  /* It judges the condition. */
    {  
        *comment_start = '\0';  /* The value of *comment_start is updated by '\0' */
     }  
}  
  
/* Reads STR and determines whether it is a label (ends in ':'), and if so,
   whether it is a valid label, and then tries to add it to the symbol table.

   INPUT_LINE is which line of the input file we are currently processing. Note
   that the first line is line 1 and that empty lines are included in this
   count.

   BYTE_OFFSET is the offset of the NEXT instruction (should it exist).

   Four scenarios can happen:
    1. STR is not a label (does not end in ':'). Returns 0.
    2. STR ends in ':', but is not a valid label. Returns -1.
    3a. STR ends in ':' and is a valid label. Addition to symbol table fails.
        Returns -1.
    3b. STR ends in ':' and is a valid label. Addition to symbol table succeeds.
        Returns 1.
 */
static int add_if_label(uint32_t input_line, char *str, uint32_t byte_offset,  /* It judges the condition. */
                        SymbolTable *symtbl, int check_align)  
{  
      
    size_t len = strlen(str);  
    if (str[len - 1] == ':')  /* It judges the condition. */
    {  
        str[len - 1] = '\0';  
        if (is_valid_label(str))  /* It judges the condition. */
        {  
            /* add to table and return number of tokens, otherwise return -1 */
            if (add_to_table(symtbl, str, byte_offset, check_align) == 0)  /* It judges the condition. */
            {  
                return 1;  /* The function returns 1 */
            }  
            else  
            {  
                return -1;  /* The function returns -1 */
            }  
        }  
        else  
        {  
            /* we raise error if no valid label is found */
            raise_label_error(input_line, str);  
            return -1;  /* The function returns -1 */
        }  
    }  
    else  
    {  
        return 0;  /* The function returns 0 */
    }  
}  
  
/*******************************
 * Implement the Following
 *******************************/
  
/* First pass of the assembler. You should implement pass_two() first.

   This function should read each line, strip all comments, scan for labels,
   and pass instructions to write_original_code(), pass data to write_static_data(). 
   The symbol table and data segment should also been built and written to 
   specified file. The input file may or may not be valid. Here are some guidelines:

   1. Only one of ".data" or ".text" may be present per line. It must be the 
      first token present. Once you see a ".data" or ".text", the rest part
      will be treat as data or text segment until the next ".data" or ".text"
      Appears. Ignore the rest token of this line. Default to text segment 
      if not indicated.
    
   2. For data segment, we only considering ".word", ".half", ".Byte" types.
      These types must be the first token of a line. The rest of the tokens 
      in this line will be stored as variables of that type in the data segment.
      You can assume that these numbers are legal with their types.

   3. Only one label may be present per line. It must be the first token
      present. Once you see a label, regardless of whether it is a valid label
      or invalid label, You can treat the NEXT token in this line as the 
      beginning of an instruction or a static data.

   4. If the first token is not a label, treat it as the name of an
      instruction. DO NOT try to check it is a valid instruction in this pass.
   
   5. Everything after the instruction name in the same line should be treated 
      as arguments to that instruction. If there are more than MAX_ARGS 
      arguments, call raise_extra_argument_error() and pass in the first extra 
      argument. Do not write that instruction to the original file (eg. don't call
      write_original_code())
   
   6. Only one instruction should be present per line. You do not need to do
       anything extra to detect this - it should be handled by guideline 5.
   
   8. A line containing only a label is valid. The address of the label should
      be the address of the next instruction or static data regardless of 
      whether there is a next instruction or static data or not.
   
   9. If an instruction contains an immediate, you should output it AS IS.
   
   10. Comments should always be skipped before any further process.

   11. Note that the starting address of the text segment is defined as TEXT_BASE
   in "../types.", and the starting address of the data segment is defined as 
   DATA_BASE in "../types.".

   12. Think about how you should use write_original_code() and write_static_data().
   
   Just like in pass_two(), if the function encounters an error it should NOT
   exit, but process the entire file and return -1. If no errors were
   encountered, it should return 0.
 */
  
int pass_one(FILE *input, FILE *original, FILE *symtbl, FILE *data) {  
    /* YOUR CODE HERE */
    /* to check errors */
    int err_chk = 0;  
    /* destination string , the buffer */
    char str[BUF_SIZE];  
    /* a token for each call of strtok() */
    char* token;  
    /* to write .data */
    Double buf = 0;  
    /* the first empty byte position of a double word */
    int buf_epos = 0;  
    /* record the line */
    int text_offset = 0;  
    /* number of instruciton written */
    int written_inst = 0;
    /* is data, not text. It is text without specificaiton */
    int is_data = 0;  
    /* record the line of the text */
    int line_count = 0;  
    /* data offset */
    int byte_offset = 0;  
    int check_align = 0;  
    int ail = 0;  
    char *name;  
    char *args[BUF_SIZE];
    int num_args = 0;
    /* c89 sucks! It is a piece of shit ! :( */
    /* build a table */
    SymbolTable *table_stack = create_table(SYMBOLTBL_UNIQUE_NAME);  
    memset(str, 0, BUF_SIZE);
    while (fgets(str, BUF_SIZE, input) != NULL) {  /* It executes a "while" loop */
      /* not EOF */
      line_count++;  
      /* skip comments */
      skip_comments(str);  
      /* check aligne. 1 stands for non-alignment */
      check_align = 0;  /* The value of check_align is updated by 0 */
       if (str[0] == ' ')  /* It judges the condition. */
        check_align = 1;  /* The value of check_align is updated by 1 */
       token = strtok(str, IGNORE_CHARS);  
      /* empty line */
      if (token == NULL) {  /* It judges the condition. */
        continue;  
      }  
      /* check if it is .data/.text label */
      if (strcmp(token, ".data") == 0) {  /* It judges the condition. */
        is_data = 1;  /* The value of is_data is updated by 1 */
         continue;  
      }  
      else if (strcmp(token, ".text") == 0) {  /* It judges the condition. */
        is_data = 0;  /* The value of is_data is updated by 0 */
         continue;  
      }  
      /* check if it is a label */
      ail =  
      add_if_label(line_count, token, byte_offset, table_stack, check_align);  /* It judges the condition. */
      if (ail == -1) {  /* It judges the condition. */
        err_chk = 1;  /* The value of err_chk is updated by 1 */
         token = strtok(NULL, IGNORE_CHARS);  
      }  
      /* is a label */
      else if (ail == 1) {  /* It judges the condition. */
        /* data label */
        if (is_data) {  /* It judges the condition. */
          write_sym(symtbl, DATA_BASE + byte_offset, token);
        }
        /* text label */
        else {
          write_sym(symtbl, TEXT_BASE + text_offset, token);
        }
        token = strtok(NULL, IGNORE_CHARS);  
      }  
      /* go to the next line */
      if (token == NULL)  /* It judges the condition. */
        continue;  
      /********** deal with .data **********/
      if (is_data == 1) {  /* It judges the condition. */
        /* word-type */
        if (strcmp(token, ".word") == 0) {  /* It judges the condition. */
          while ((token = strtok(NULL, IGNORE_CHARS)) != NULL) {  /* It executes a "while" loop */
            Word w = strtol(token, NULL, 0);  
            /* move bytes */
            buf += ((Double)w) << (8 * buf_epos);
            write_static_data(data, (Byte *)(&buf));
            byte_offset += 4;  
          }
        }  
        /* half-word */
        else if (strcmp(token, ".half") == 0) {  /* It judges the condition. */
          while ((token = strtok(NULL, IGNORE_CHARS)) != NULL) {  /* It executes a "while" loop */
            Half h = strtol(token, NULL, 0);  
            /* move bytes */
            buf += ((Double)h) << (8 * buf_epos);  
            byte_offset += 2;
            buf_epos += 2;
            if (buf_epos >= 4) { /* It judges the condition. */
              write_static_data(data, (Byte*)(&buf));  
              /* update buf empty position */
              buf_epos = buf_epos - 4;
            }
          }  
        }  
        /* .byte-type */
        else if (strcmp(token, ".byte") == 0) {  /* It judges the condition. */
          while ((token = strtok(NULL, IGNORE_CHARS)) != NULL) {  /* It executes a "while" loop */
            Byte b = strtol(token, NULL, 0);
            /* move bytes */
            buf += ((Double)b) << (8 * buf_epos);
            buf_epos += 1;
            byte_offset += 1;  
            if (buf_epos >= 4) {  /* It judges the condition. */
              write_static_data(data, (Byte*)(&buf));
              /* update buf empty position */
              buf_epos = buf_epos - 4;  
            }
          }
        }
        /* go to the next line */
        continue;  
      }  
      /********** deal with .text ***********/
      /* instruciton name */
      name = token;  /* The value of name is updated by token */
       /* char *args[BUF_SIZE]; */
      num_args = 0;  /* The value of num_args is updated by 0 */
       while ((token = strtok(NULL, IGNORE_CHARS)) != NULL) {  /* It executes a "while" loop */
        args[num_args++] = token;  /* The value of args[num_args++] is updated by token */
       }  
      if (num_args >MAX_ARGS) {  /* It judges the condition. */
        raise_extra_argument_error(line_count, args[3]);  
        err_chk = 1;  /* The value of err_chk is updated by 1 */
         /* go to the next line */
        continue;  
      }  
      if ((written_inst = write_original_code(original, name, args, num_args)) == 0) {  /* It judges the condition. */
        err_chk = 1;  /* The value of err_chk is updated by 1 */
         raise_instruction_error(line_count, name, args, num_args);  
      }  
      else {  
        text_offset += 4 * written_inst;  
      }  
    } /* fgets() */
    /* deal with .data at the last text line */
    if (buf_epos != 0) {  /* It judges the condition. */
      write_static_data(data, (Byte*)(&buf));  
      buf_epos = 0;  /* The value of buf_epos is updated by 0 */
   }  
  free_table(table_stack);  
  /* it has errors */
  if (err_chk)  /* It judges the condition. */
    return -1;  /* The function returns -1 */
  /* no error */
  return 0;  /* The function returns 0 */
}  
  
/* Second pass of the assembler.

   This function should read an original file and the corresponding symbol table
   file, translates it into basic code and machine code. You may assume:
    1. The input file contains no comments
    2. The input file contains no labels
    3. The input file contains at maximum one instruction per line
    4. All instructions have at maximum MAX_ARGS arguments
    5. The symbol table file is well formatted
    6. The symbol table file contains all the symbol required for translation
    7. The line may contain only one "-" which is placeholder you can ignore.
   If an error is reached, DO NOT EXIT the function. Keep translating the rest of
   the document, and at the end, return -1. Return 0 if no errors were encountered. */
  
  
int pass_two(FILE *original, FILE *symtbl, FILE *basic, FILE *machine) {  
  /* YOUR CODE HERE */
  int err_chk = 0;  
  char str[BUF_SIZE];  
  char *token;  
  char *name;  
  /* for PC pointer */
  uint32_t offset = 0;
  /* the number of written instrucitons */
  int inst_num;  
  /* conunt text lines */
  unsigned int line_count = 0;
  SymbolTable *tabel_stack =
  create_table_from_file(SYMBOLTBL_UNIQUE_NAME, symtbl);
  memset(str, 0, BUF_SIZE);
  while (fgets(str, BUF_SIZE, original) != NULL) {  /* It executes a "while" loop */
    char *args[MAX_ARGS] = {0, 0, 0};  
    int num_args = 0;  
    line_count++;  
    name = strtok(str, IGNORE_CHARS);
    if (name == NULL) {
      err_chk = -1;
      continue;
    }
    /* for place holder */
    if (strcmp(name, "-") == 0) {
      continue;
    }
    token = strtok(NULL, IGNORE_CHARS);
    while (token != NULL) {  /* It executes a "while" loop */
      args[num_args++] = token;  /* The value of args[num_args++] is updated by token */
       token = strtok(NULL, IGNORE_CHARS);  
    }  
    /* the number of instrucitons written */
    printf("%d %s\n", line_count, name);
    inst_num = translate_inst(basic, machine, name, args, num_args,
                              (line_count - 1) * 4, tabel_stack);
    if (inst_num == 0) {  /* It judges the condition. */
      err_chk = -1;  /* The value of err_chk is updated by -1 */
      raise_instruction_error(line_count, name, args, num_args);
      continue;
    }
    offset += 4 * inst_num;
  }  
  /* free symbol table after using */
  free_table(tabel_stack);  
  return err_chk;  /* The function returns err_chk */
}  
  
  
/*******************************
 * Do Not Modify Code Below
 *******************************/
  
  
/* Runs the two-pass assembler. Most of the actual work is done in pass_one()
   and pass_two().
 */
int assemble(int mode, FILE *input, FILE *data, FILE *symtbl, FILE *orgin, FILE *basic, FILE *text)  
{  
    int err = 0;  
  
    /* Execute pass one if mode two is not specified */
    if (mode != 2)  /* It judges the condition. */
    {  
        rewind(input);  
        if (pass_one(input, orgin, symtbl, data) != 0)  /* It judges the condition. */
        {  
            err = 1;  /* The value of err is updated by 1 */
         }  
    }  
  
    /* Execute pass two if mode one is not specified */
    if (mode != 1)  /* It judges the condition. */
    {  
        rewind(orgin);  
        rewind(symtbl);  
        if (pass_two(orgin, symtbl, basic, text) != 0)  /* It judges the condition. */
        {  
            err = 1;  /* The value of err is updated by 1 */
         }  
    }  
  
    /* Error handling */
    if (err) {  /* It judges the condition. */
        write_to_log("One or more errors encountered during assembly operation.\n");  
    } else {  
        write_to_log("Assembly operation completed successfully!\n");  
    }  
  
    return err;  /* The function returns err */
}  
