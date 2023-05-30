#include "stdio.h"
#include <stdint.h>
#include <time.h>

#define getsize(type) {\
  unsigned int szlib[18] = {\
  sizeof(char),      sizeof(short),        sizeof(short int), sizeof(int),\
  sizeof(long int),  sizeof(unsigned int), sizeof(void *),    sizeof(size_t),\
  sizeof(float),     sizeof(double),       sizeof(int8_t),    sizeof(int16_t),\
  sizeof(int32_t),   sizeof(int64_t),      sizeof(time_t),    sizeof(clock_t),\
  sizeof(struct tm), sizeof(NULL)};\
  char lib[18][20] = {\
  "char",         "short",   "short int", "int",    "long int",\
  "unsigned int", "void *",  "size_t",    "float",  "double",\
  "int8_t",       "int16_t",   "int32_t",     "int64_t", "time_t",\
  "clock_t",        "struct tm", "NULL"};\
  int i = 0;\
  for (i = 0; i < 18; ++i) {\
    int flag = 1;\
    int j = 0;\
    while (type[j] != '\0' || lib[i][j] != '\0') {\
      if (type[j] != lib[i][j]) {\
        flag = 0;\
        break;\
      }\
      ++j;\
    }\
    if (flag == 1) {\
      printf("Size of %s : %u\n", lib[i], szlib[i]);\
      break;\
    }\
  }\
  }

int main() {
  char input[20] = "0000000000000000000";
  int j = 0;
  char c = 0;
  while ((c = getchar()) != '\n') {
    input[j++] = c;
  }
  input[j] = '\0';
  getsize(input);
  return 0;
}
