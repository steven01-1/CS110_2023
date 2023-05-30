#include "threadpool.h"
#include "ringbuffer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static bool lock_acquire(threadpool_t *pool) {
  return pthread_mutex_lock(&(pool->pool_lock)) == 0;
}

static bool lock_release(threadpool_t *pool) {
  return pthread_mutex_unlock(&(pool->pool_lock)) == 0;
}

static void deallocate_all(threadpool_t *pool) {
  /* 1. Invalid address */
  if (pool == NULL) {
    return;
  }

  pthread_cond_destroy(&(pool->notify));
  pthread_mutex_destroy(&(pool->pool_lock));

  free(pool->thread_list);
  ringbuffer_destroy(pool->task_list);

  free(pool);
}

static void *threadpool_thread(void *threadpool) {
  /* 0. Reinterpret the pointer */
  threadpool_t *pool = (threadpool_t *)threadpool;

  /* 1. Make sure the pool is valid */
  assert(pool != NULL);

  while (true) {
    /* 2. Lock */
    assert(lock_acquire(pool));

    /* 3. Keep waiting for condition */
    while (ringbuffer_is_empty(pool->task_list) && (!pool->shutdown)) {
      assert(pthread_cond_wait(&(pool->notify), &(pool->pool_lock)) == 0);
    }

    /* 4. If shutdown */
    if (pool->shutdown) {
      // TODO: sync?
      if (!pool->sync) {
        assert(lock_release(pool));
        pthread_exit(NULL);
      }
      if (ringbuffer_is_empty(pool->task_list)) {
        assert(lock_release(pool));
        pthread_exit(NULL);
      }
    }

    /* 5. Fetch next task */
    // TODO: fetch next task
    threadpool_task_t task;
    int pop_status = ringbuffer_pop(pool->task_list, &task);

    /* 6. Unlock */
    assert(lock_release(pool));

    /* 7. Get to work */
    // TODO: run next task
    if (pop_status) {
      task.func(task.args);
    }
  }
}

// TODO: modify this function, make sure it is memory safe
threadpool_t *threadpool_create(size_t thread_count, size_t queue_size,
                                bool sync) {
  /* 0. Parameters should be reasonable */
  if (thread_count == 0 || queue_size == 0 || queue_size < thread_count) {
    return NULL;
  }

  /* 1. Allocate memory for pool */
  threadpool_t *pool = (threadpool_t *)malloc(sizeof(threadpool_t));

  /* 2. If fail */
  if (!pool) {
    return NULL;
  }

  /* 3. Allocate memory for workers and tasks */
  pool->thread_list = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
  pool->task_list = ringbuffer_create(queue_size);
  pool->sync = sync;

  /* 4. If fail */
  if (!pool->thread_list || !pool->task_list) {
    return NULL;
  }

  /* 5. Initialize lock */
  pthread_mutexattr_t safeLock;
  pthread_mutexattr_init(&safeLock);
  pthread_mutexattr_settype(&safeLock, PTHREAD_MUTEX_ERRORCHECK);
  /* Note: this kind of mutex provides error checking.
   * Error would occur when:
   * 1. A thread attempt to re-lock when holding the lock.
   * 2. A thread attempt to release the lock when not holding it.
   * 3. A thread attempt to release an unlocked lock.
   * */
  if (pthread_mutex_init(&pool->pool_lock, &safeLock) != 0) {
    return NULL;
  }

  /* 6. Initialize conditional variable */
  if (pthread_cond_init(&(pool->notify), NULL) != 0) {
    return NULL;
  }

  /* 7. Initialize other stuff */
  pool->thread_count = 0;
  pool->shutdown = false;

  /* 8. Start worker threads */
  for (size_t i = 0; i < thread_count; i++, pool->thread_count = i) {
    // TODO: pthread_create may fail ... but when?
    // RTFM!
    int status = pthread_create(&(pool->thread_list[i]), NULL,
                                threadpool_thread, (void *)pool);
    if (status != 0) {
      deallocate_all(pool);
      return NULL;
    }
  }
  return pool;
}

bool threadpool_add_task(threadpool_t *pool, void (*func)(void *), void *args) {
  /* 0. Parameters should be reasonable */
  if (!pool || !func) {
    return false;
  }

  /* 1. Acquire lock */
  if (!lock_acquire(pool)) {
    return false;
  }

  /* 2. If the pool is not available for newcomers */
  if (ringbuffer_is_full(pool->task_list) || pool->shutdown) {
    /* Note: don't forget to release locks here */
    lock_release(pool);
    return false;
  }

  /* 3. Add a task to ringbuffer */
  // TODO: implement
  threadpool_task_t v;
  v.args = args;
  v.func = func;
  // ringbuffer_push(pool->task_list, v);
  if (!ringbuffer_push(pool->task_list, v)) {
    lock_release(pool);
    return false;
  }

  /* 4. Wake up a random available worker */
  if (pthread_cond_signal(&(pool->notify)) != 0) {
    lock_release(pool);
    return false;
  }

  /* 5. Success */

  lock_release(pool);
  return true;
}

bool threadpool_destroy(threadpool_t *pool) {
  /* 0. Parameter should be reasonable */
  if (!pool) {
    return false;
  }

  /* 1. Acquire lock */
  if (!lock_acquire(pool)) {
    return false;
  }

  /* 2. If the pool is not available already shutting down */
  if (pool->shutdown) {
    lock_release(pool);
    return false;
  }

  /* 3. Start the shutting down process */
  pool->shutdown = true;

  /* 4. Wake up all workers to exit */
  if (pthread_cond_broadcast(&(pool->notify)) != 0) {
    lock_release(pool);
    return false;
  }

  /* 5. Release lock */
  lock_release(pool);

  /* 6. Wait for all threads terminate */
  // TODO: join all worker threads
  for (size_t i = 0; i < pool->thread_count; ++i) {
    // if (pthread_join(pool->thread_list[i], NULL) != 0)
    //     return false;
    pthread_join(pool->thread_list[i], NULL);
  }

  /* 7. Deallocate all resources given */
  // TODO: implement
  deallocate_all(pool);
  return true;
}

