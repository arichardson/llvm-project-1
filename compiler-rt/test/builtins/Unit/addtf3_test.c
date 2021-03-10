// RUN: %clang_builtins %s %librt -Wall -Wextra -Werror=format -o %t && %run %t
// REQUIRES: librt_has_addtf3

#include <stdio.h>

#include "int_lib.h"
#include "fp_test_fenv.h"

#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else

// Returns: a + b
COMPILER_RT_ABI f128 __addtf3(f128 a, f128 b);

int _test__addtf3(int line, f128 a, f128 b, int expectedExc,
                  uint64_t expectedHi, uint64_t expectedLo) {
  feclearexcept(FE_ALL_EXCEPT);
  f128 x = __addtf3(a, b);
  int actualExc = fetestexcept(FE_ALL_EXCEPT);
  int ret = compareResultF128(x, expectedHi, expectedLo);
  if (ret) {
    printMismatchF128TwoArg(__FILE__, line, "__addtf3", a, b, x,
                            expectedHi, expectedLo);
  }
  if (crt_sets_fp_exc_flags() && actualExc != expectedExc) {
    fprintf(stderr, "%s:%d: Got unexpected floating-point exception %#x, expected %#x\n",
            __FILE__, line, actualExc, expectedExc);
    return 1;
  }
  return ret;
}
#define test__addtf3(...) _test__addtf3(__LINE__, __VA_ARGS__)

int main() {
  // qNaN + any = qNaN
  if (test__addtf3(makeQNaN128(),
                   0x1.23456789abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // NaN + any = NaN
  if (test__addtf3(makeNaN128(UINT64_C(0x800030000000)),
                   0x1.23456789abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;

  // sNaN + any = qNaN, FE_INVALID
  if (test__addtf3(makeNaN128(UINT64_C(0x12345)),
                   0x1.23456789abcdefp+5Q,
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;

  // any + sNaN = qNaN, FE_INVALID
  if (test__addtf3(0x1.23456789abcdefp+5Q,
                   makeNaN128(UINT64_C(0x12345)),
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;

  // inf + inf = inf
  if (test__addtf3(makeInf128(),
                   makeInf128(),
                   0,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // inf + any = inf
  if (test__addtf3(makeInf128(),
                   0x1.2335653452436234723489432abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // inf + -inf = NaN, FE_INVALID
  if (test__addtf3(makeInf128(),
                   makeNegativeInf128(),
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // -inf + inf = NaN, FE_INVALID
  if (test__addtf3(makeNegativeInf128(),
                   makeInf128(),
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;

  // any + any
  if (test__addtf3(/* 0x1.23456734245345543849abcdefp+5Q */
                   fromRep128(UINT64_C(0x4004234567342453), UINT64_C(0x45543849abcdef00)),
                   /* 0x1.edcba52449872455634654321fp-1Q */
                   fromRep128(UINT64_C(0x3ffeedcba5244987), UINT64_C(0x2455634654321f00)),
                   0,
                   UINT64_C(0x40042afc95c8b579),
                   UINT64_C(0x61e58dd6c51eb77c)))
    return 1;

#if (defined(__arm__) || defined(__aarch64__)) && defined(__ARM_FP) || \
    defined(i386) || defined(__x86_64__)
  // Rounding mode tests on supported architectures
  const f128 m = assertF128Representation(1234.0Q, UINT64_C(0x4009348000000000), UINT64_C(0x0000000000000000));
  // Closest approximation of 0.1 for f128:
  const f128 n = assertF128Representation(0.1Q, UINT64_C(0x3ffb999999999999), UINT64_C(0x999999999999999a));
  fesetround(FE_UPWARD);
  if (test__addtf3(m, n, FE_INEXACT,
                   UINT64_C(0x4009348666666666),
                   UINT64_C(0x6666666666666667)))
    return 1;

  fesetround(FE_DOWNWARD);
  if (test__addtf3(m, n, FE_INEXACT,
                   UINT64_C(0x4009348666666666),
                   UINT64_C(0x6666666666666666)))
    return 1;

  fesetround(FE_TOWARDZERO);
  if (test__addtf3(m, n, FE_INEXACT,
                   UINT64_C(0x4009348666666666),
                   UINT64_C(0x6666666666666666)))
    return 1;

  fesetround(FE_TONEAREST);
  if (test__addtf3(m, n, FE_INEXACT,
                   UINT64_C(0x4009348666666666),
                   UINT64_C(0x6666666666666666)))
    return 1;
#endif
  return 0;
}

#endif
