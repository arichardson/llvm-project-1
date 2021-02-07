// RUN: %clangxx_tsan -O1 %s -o %t && %run %t | FileCheck %s
// REQUIRES: freebsd
// FreeBSD libc uses _pthread_* calls (with an extra underscore). Check that
// those are also intercepted and no false-positive race is reported here.
#include "test.h"

int Global;
pthread_mutex_t mtx;

extern "C" int _pthread_mutex_lock(pthread_mutex_t *);
extern "C" int _pthread_mutex_unlock(pthread_mutex_t *);

void *Thread1(void *x) {
  pthread_mutex_lock(&mtx);
  Global++;
  pthread_mutex_unlock(&mtx);
  return NULL;
}

void *Thread2(void *x) {
  _pthread_mutex_lock(&mtx);
  Global++;
  _pthread_mutex_unlock(&mtx);
  return NULL;
}

int main() {
  barrier_init(&barrier, 2);
  pthread_mutex_init(&mtx, 0);
  pthread_t t[2];
  pthread_create(&t[0], NULL, Thread1, NULL);
  pthread_create(&t[1], NULL, Thread2, NULL);
  pthread_join(t[0], NULL);
  pthread_join(t[1], NULL);
  pthread_mutex_destroy(&mtx);
  printf("DONE\n");
  // CHECK-NOT: WARNING: ThreadSanitizer:
  // CHECK: DONE
  // CHECK-NOT: WARNING: ThreadSanitizer:
}
