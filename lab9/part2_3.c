#include <emmintrin.h> /* header file for the SSE intrinsics */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float **a; // 40000 x 4
float **b; // 4 x 20000
float **c; // 40000 x 20000
float **c2; // 40000 x 20000

int n = 40000;
int p = 20000;

void init(void) {
    a = malloc(n * sizeof(float *));
    b = malloc(4 * sizeof(float *));
    c = malloc(n * sizeof(float *));
    c2 = malloc(n * sizeof(float *));
    for (int i = 0; i < n; ++i) {
        a[i] = malloc(4 * sizeof(float));
        c[i] = malloc(p * sizeof(float));
        c2[i] = malloc(p * sizeof(float));
    }
    for (int i = 0; i < 4; ++i) {
        b[i] = malloc(p * sizeof(float));
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 4; ++j) {
            a[i][j] = (float) rand() / (float) RAND_MAX;
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < p; ++j) {
            b[i][j] = (float) rand() / (float) RAND_MAX;
        }
    }
}

void check_correctness(char *msg) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            if (fabsf(c[i][j] - c2[i][j]) > 1e-6) {
                printf("%s incorrect!\n", msg);
                return;
            }
        }
    }
}

void naive_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // c = a * b
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            c[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("naive: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
}

void loop_unroll_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // c2 = a * b
    // TODO: implement me!
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            c2[i][j] = 0;
//            c2[i][j] += a[i][0] * b[0][j]; 为啥这里慢死了？？？都是8次访问，4次写入
//            c2[i][j] += a[i][1]* b[1][j];
//            c2[i][j] += a[i][2] * b[2][j];
//            c2[i][j] += a[i][3] * b[3][j];
        double t1 = a[i][0] * b[0][j], t2 = a[i][1] * b[1][j],
         t3 = a[i][2] * b[2][j], t4 = a[i][3] * b[3][j];
        c2[i][j] += t1;
        c2[i][j] += t2;
        c2[i][j] += t3;
        c2[i][j] += t4;

        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("unroll: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("loop_unroll_matmul");

}


void simd_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // c2 = a * b
    // TODO: implement me!
    // row a
    for (int i = 0; i < n; ++i) {
      // __m128 a0 =_mm_set1_ps(a[i][0]);
      // __m128 a1 =_mm_set1_ps(a[i][1]);
      // __m128 a2 =_mm_set1_ps(a[i][2]);
      // __m128 a3 =_mm_set1_ps(a[i][3]);
      __m128 a_array[4] = {_mm_set1_ps(a[i][0]), _mm_set1_ps(a[i][1]),
                           _mm_set1_ps(a[i][2]), _mm_set1_ps(a[i][3])};
        // ai0 to ai3
      // a column of b
      for (int k = 0; k < p; k += 4) {
        __m128 c0 = _mm_setzero_ps();
        for (int j = 0; j < 4; ++j) {
          __m128 b0 = _mm_loadu_ps((float *)&b[j][k]);
          c0 = _mm_add_ps(c0, _mm_mul_ps(a_array[j], b0));
        }
        _mm_storeu_ps((float *)&c2[i][k], c0);
      }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("simd: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("simd_matmul");
}

void loop_unroll_simd_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // c2 = a * b
    // TODO: implement me!
    // row a
    for (int i = 0; i < n; ++i) {
        __m128 a_array[4] = {_mm_set1_ps(a[i][0]), _mm_set1_ps(a[i][1]),
                             _mm_set1_ps(a[i][2]), _mm_set1_ps(a[i][3])};
        // ai0 to ai3
        // a column of b
        for (int k = 0; k < p; k += 4) {
            __m128 c0 = _mm_setzero_ps();

            __m128 b0 = _mm_loadu_ps((float *)&b[0][k]);
            __m128 b1 = _mm_loadu_ps((float *)&b[1][k]);
            __m128 b2 = _mm_loadu_ps((float *)&b[2][k]);
            __m128 b3 = _mm_loadu_ps((float *)&b[3][k]);
            __m128 tmp0 = _mm_mul_ps(a_array[0], b0),
            tmp1 = _mm_mul_ps(a_array[1], b1),
            tmp2 = _mm_mul_ps(a_array[2], b2),
            tmp3 = _mm_mul_ps(a_array[3], b3);
            c0 = _mm_add_ps(c0, tmp0);
            c0 = _mm_add_ps(c0, tmp1);
            c0 = _mm_add_ps(c0, tmp2);
            c0 = _mm_add_ps(c0, tmp3);

            _mm_storeu_ps((float *)&c2[i][k], c0);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("unroll+simd: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("loop_unroll_simd_matmul");
}


int main(void) {
    init();

    naive_matmul();
    simd_matmul();
     loop_unroll_matmul();
     loop_unroll_simd_matmul();

//     for (int i = 0; i < 4; i++) {
//         printf("%f %f\n", a[0][i], b[i][2]);
//     }
    return 0;
}

