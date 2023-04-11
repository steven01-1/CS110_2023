/* Include the system headers we need */
#include <stdlib.h>
#include <stdio.h>

/* Include our header */
#include "vector.h"

struct vector_t {
    size_t size;
    int *data;
};

static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(1);
}

/* Another suboptimal way of creating a vector */
vector_t also_bad_vector_new() {
    /* Create the vector */
    vector_t v;

    /* Initialize attributes */
    v.size = 1;
    v.data = malloc(sizeof(int));
    if (v.data == NULL) {
        allocation_failed();
    }
    printf("%p\n", v.data);
    v.data[0] = 0;
    return v;
}

int main() {
  vector_t srt = also_bad_vector_new();
printf("%p\n", srt.data);
  printf("%d", srt.data[0]);
}




