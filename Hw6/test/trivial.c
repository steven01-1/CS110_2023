#include "../threadpool.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#define Ntestcase1 10
pthread_spinlock_t spin;

void test_drain_buffer(void *arg) {
  int *count = (int *)arg;
  pthread_spin_lock(&spin);
  (*count)++;
  pthread_spin_unlock(&spin);
}

void pt(void *arg) {
  char *t = (char *)arg;
  pthread_spin_lock(&spin);
  printf("%s\n", t);
  pthread_spin_unlock(&spin);
}

int main(void) {
  threadpool_t *pool;
  pthread_spin_init(&spin, 0);

  // char *le1[] = {"a1", "a2", "a3", "a4", "a5", "a6"};
  // char le[100];
  /* test case 1: wait queue drain */
  int count = 0;
  pool = threadpool_create(4, Ntestcase1, 1);
  for (int i = 0; i < Ntestcase1; i++) {
    // assert(threadpool_add_task(pool, test_drain_buffer, &count));
    // scanf("%s", le);
    // printf("read: %s\n\n", le);
    bool state = threadpool_add_task(pool, test_drain_buffer, &count);
    if (!state) {
      printf("too much\n");
    }

  }
  // assert(threadpool_destroy(pool));
  threadpool_destroy(pool);
  // printf("count: %d\n", count);
  assert(count == Ntestcase1);
  pthread_spin_destroy(&spin);
  return 0;
}
