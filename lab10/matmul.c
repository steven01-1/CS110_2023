#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_SIZE 1024

void matmul_naive(double *a, double *b, double *c) {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      for (int k = 0; k < MATRIX_SIZE; k++) {
        c[i * MATRIX_SIZE + j] +=
        a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
      }
    }
  }
}

void matmul_optimized_slices(double *a, double *b, double *c) {
  omp_set_num_threads(32);
  int thread_ID = 0;
#pragma omp parallel private(thread_ID)
  {
    thread_ID = omp_get_thread_num();
    // int thread_ID = omp_get_thread_num ();
    for (int i = thread_ID; i < MATRIX_SIZE; i += 32) {
      for (int j = 0; j < MATRIX_SIZE; j++) {
        for (int k = 0; k < MATRIX_SIZE; k++) {
          c[i * MATRIX_SIZE + j] +=
          a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
        }
      }
    }
  }
}

void matmul_optimized_chunks(double *a, double *b, double *c) {
  omp_set_num_threads(32);
  int thread_ID = 0;
#pragma omp parallel private(thread_ID)
  {
    thread_ID = omp_get_thread_num();
    // printf("thread: %d\n", thread_ID);
    for (int i = (thread_ID << 5); i < (thread_ID << 5) + 32; i++) {
      for (int j = 0; j < MATRIX_SIZE; j++) {
        for (int k = 0; k < MATRIX_SIZE; k++) {
          c[i * MATRIX_SIZE + j] +=
          a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
        }
      }
    }
  }
}

int main() {
  double *a = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
  double *b = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
  double *c = (double *)calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(double));
  double *c2 = (double *)calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(double));
  double *c3 = (double *)calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(double));

  if (!a || !b || !c) {
    printf("Error: could not allocate memory.\n");
    return 1;
  }

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      a[i * MATRIX_SIZE + j] = (double)rand() / RAND_MAX;
      b[i * MATRIX_SIZE + j] = (double)rand() / RAND_MAX;
    }
  }

  double start_time = omp_get_wtime();
  matmul_naive(a, b, c);
  double end_time = omp_get_wtime();
  printf("Naive: %.5f seconds\n", end_time - start_time);

  for (int i = 0; i < 10; i++) {
    start_time = omp_get_wtime();
    matmul_optimized_slices(a, b, c2);
    end_time = omp_get_wtime();
    printf("Optimized (slices): %.5f seconds\n", end_time - start_time);

    start_time = omp_get_wtime();
    matmul_optimized_chunks(a, b, c3);
    end_time = omp_get_wtime();
    printf("Optimized (chunks): %.5f seconds\n\n", end_time - start_time);
  }

  // check correctness
  // for (int i = 0; i < MATRIX_SIZE; i++) {
  //   for (int j = 0; j < MATRIX_SIZE; j++) {
  //     if (c[i * MATRIX_SIZE + j] != c2[i * MATRIX_SIZE + j])
  //     {printf("false 1\n"); goto BK;}
  //     if (c[i * MATRIX_SIZE + j] != c3[i * MATRIX_SIZE + j])
  //     {printf("false 2: i = %d, j = %d\n", i, j); goto BK;}
  //   }
  // }

  // BK: ;

  free(a);
  free(b);
  free(c);

  return 0;
}
