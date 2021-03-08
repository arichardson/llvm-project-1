// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_powitf2

#include <stdio.h>

#include "int_lib.h"
#include <math.h>

// Returns: a ^ b

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
COMPILER_RT_ABI f128 __powitf2(f128 a, int b);

int _test__powitf2(int line, f128 a, int b, f128 expected) {
  f128 x = __powitf2(a, b);
  // Note: We use __builtin_signbit() here to avoid a potential long double conversion.
  int correct = (x == expected) && (__builtin_signbit(x) == __builtin_signbit(expected));
  if (!correct) {
    printMismatchF128TwoArg(__FILE__, line, "__powitf2", a, b, x,
                            toRep128(expected) >> 64, (uint64_t)toRep128(expected));
  }
  return !correct;
}

#define test__powitf2(...) _test__powitf2(__LINE__, __VA_ARGS__)

int main() {
  if (test__powitf2(0, 0, 1))
    return 1;
  if (test__powitf2(1, 0, 1))
    return 1;
  if (test__powitf2(1.5, 0, 1))
    return 1;
  if (test__powitf2(2, 0, 1))
    return 1;
  if (test__powitf2(INFINITY, 0, 1))
    return 1;

  if (test__powitf2(-0., 0, 1))
    return 1;
  if (test__powitf2(-1, 0, 1))
    return 1;
  if (test__powitf2(-1.5, 0, 1))
    return 1;
  if (test__powitf2(-2, 0, 1))
    return 1;
  if (test__powitf2(-INFINITY, 0, 1))
    return 1;

  if (test__powitf2(0, 1, 0))
    return 1;
  if (test__powitf2(0, 2, 0))
    return 1;
  if (test__powitf2(0, 3, 0))
    return 1;
  if (test__powitf2(0, 4, 0))
    return 1;
  if (test__powitf2(0, INT_MAX - 1, 0))
    return 1;
  if (test__powitf2(0, INT_MAX, 0))
    return 1;

  if (test__powitf2(-0., 1, -0.))
    return 1;
  if (test__powitf2(-0., 2, 0))
    return 1;
  if (test__powitf2(-0., 3, -0.))
    return 1;
  if (test__powitf2(-0., 4, 0))
    return 1;
  if (test__powitf2(-0., INT_MAX - 1, 0))
    return 1;
  if (test__powitf2(-0., INT_MAX, -0.))
    return 1;

  if (test__powitf2(1, 1, 1))
    return 1;
  if (test__powitf2(1, 2, 1))
    return 1;
  if (test__powitf2(1, 3, 1))
    return 1;
  if (test__powitf2(1, 4, 1))
    return 1;
  if (test__powitf2(1, INT_MAX - 1, 1))
    return 1;
  if (test__powitf2(1, INT_MAX, 1))
    return 1;

  if (test__powitf2(INFINITY, 1, INFINITY))
    return 1;
  if (test__powitf2(INFINITY, 2, INFINITY))
    return 1;
  if (test__powitf2(INFINITY, 3, INFINITY))
    return 1;
  if (test__powitf2(INFINITY, 4, INFINITY))
    return 1;
  if (test__powitf2(INFINITY, INT_MAX - 1, INFINITY))
    return 1;
  if (test__powitf2(INFINITY, INT_MAX, INFINITY))
    return 1;

  if (test__powitf2(-INFINITY, 1, -INFINITY))
    return 1;
  if (test__powitf2(-INFINITY, 2, INFINITY))
    return 1;
  if (test__powitf2(-INFINITY, 3, -INFINITY))
    return 1;
  if (test__powitf2(-INFINITY, 4, INFINITY))
    return 1;
  if (test__powitf2(-INFINITY, INT_MAX - 1, INFINITY))
    return 1;
  if (test__powitf2(-INFINITY, INT_MAX, -INFINITY))
    return 1;

  if (test__powitf2(0, -1, INFINITY))
    return 1;
  if (test__powitf2(0, -2, INFINITY))
    return 1;
  if (test__powitf2(0, -3, INFINITY))
    return 1;
  if (test__powitf2(0, -4, INFINITY))
    return 1;
  if (test__powitf2(0, INT_MIN + 2, INFINITY))
    return 1;
  if (test__powitf2(0, INT_MIN + 1, INFINITY))
    return 1;
  if (test__powitf2(0, INT_MIN, INFINITY))
    return 1;

  if (test__powitf2(-0., -1, -INFINITY))
    return 1;
  if (test__powitf2(-0., -2, INFINITY))
    return 1;
  if (test__powitf2(-0., -3, -INFINITY))
    return 1;
  if (test__powitf2(-0., -4, INFINITY))
    return 1;
  if (test__powitf2(-0., INT_MIN + 2, INFINITY))
    return 1;
  if (test__powitf2(-0., INT_MIN + 1, -INFINITY))
    return 1;
  if (test__powitf2(-0., INT_MIN, INFINITY))
    return 1;

  if (test__powitf2(1, -1, 1))
    return 1;
  if (test__powitf2(1, -2, 1))
    return 1;
  if (test__powitf2(1, -3, 1))
    return 1;
  if (test__powitf2(1, -4, 1))
    return 1;
  if (test__powitf2(1, INT_MIN + 2, 1))
    return 1;
  if (test__powitf2(1, INT_MIN + 1, 1))
    return 1;
  if (test__powitf2(1, INT_MIN, 1))
    return 1;

  if (test__powitf2(INFINITY, -1, 0))
    return 1;
  if (test__powitf2(INFINITY, -2, 0))
    return 1;
  if (test__powitf2(INFINITY, -3, 0))
    return 1;
  if (test__powitf2(INFINITY, -4, 0))
    return 1;
  if (test__powitf2(INFINITY, INT_MIN + 2, 0))
    return 1;
  if (test__powitf2(INFINITY, INT_MIN + 1, 0))
    return 1;
  if (test__powitf2(INFINITY, INT_MIN, 0))
    return 1;

  if (test__powitf2(-INFINITY, -1, -0.))
    return 1;
  if (test__powitf2(-INFINITY, -2, 0))
    return 1;
  if (test__powitf2(-INFINITY, -3, -0.))
    return 1;
  if (test__powitf2(-INFINITY, -4, 0))
    return 1;
  if (test__powitf2(-INFINITY, INT_MIN + 2, 0))
    return 1;
  if (test__powitf2(-INFINITY, INT_MIN + 1, -0.))
    return 1;
  if (test__powitf2(-INFINITY, INT_MIN, 0))
    return 1;

  if (test__powitf2(2, 10, 1024.Q))
    return 1;
  if (test__powitf2(-2, 10, 1024.Q))
    return 1;
  if (test__powitf2(2, -10, 1 / 1024.Q))
    return 1;
  if (test__powitf2(-2, -10, 1 / 1024.Q))
    return 1;

  if (test__powitf2(2, 19, 524288.))
    return 1;
  if (test__powitf2(-2, 19, -524288.))
    return 1;
  if (test__powitf2(2, -19, 1 / 524288.Q))
    return 1;
  if (test__powitf2(-2, -19, -1 / 524288.Q))
    return 1;

  if (test__powitf2(2, 31, 2147483648.Q))
    return 1;
  if (test__powitf2(-2, 31, -2147483648.Q))
    return 1;
  if (test__powitf2(2, -31, 1 / 2147483648.Q))
    return 1;
  if (test__powitf2(-2, -31, -1 / 2147483648.Q))
    return 1;

  return 0;
}
#endif
