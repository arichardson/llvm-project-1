// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_subtf3

#include <stdio.h>

#include "int_lib.h"
#include "fp_test_fenv.h"

#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else

// Returns: a - b
COMPILER_RT_ABI f128 __subtf3(f128 a, f128 b);

int _test__subtf3(int line, f128 a, f128 b, int expectedExc,
                  uint64_t expectedHi, uint64_t expectedLo) {
  feclearexcept(FE_ALL_EXCEPT);
  f128 x = __subtf3(a, b);
  int actualExc = fetestexcept(FE_ALL_EXCEPT);
  int ret = compareResultF128(x, expectedHi, expectedLo);
  if (ret) {
    printMismatchF128TwoArg(__FILE__, line, "__subtf3", a, b, x,
                            expectedHi, expectedLo);
  }
  if (crt_sets_fp_exc_flags() && actualExc != expectedExc) {
    fprintf(stderr, "%s:%d: Got unexpected floating-point exception %#x, expected %#x\n",
            __FILE__, line, actualExc, expectedExc);
    return 1;
  }
  return ret;
}

#define test__subtf3(...) _test__subtf3(__LINE__, __VA_ARGS__)

int main() {
  // qNaN - any = qNaN
  if (test__subtf3(makeQNaN128(),
                   0x1.23456789abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // NaN - any = NaN
  if (test__subtf3(makeNaN128(UINT64_C(0x800030000000)),
                   0x1.23456789abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;

  // sNaN - any = qNaN, FE_INVALID
  if (test__subtf3(makeNaN128(UINT64_C(0x12345)),
                   0x1.23456789abcdefp+5Q,
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // any - sNaN = qNaN, FE_INVALID
  if (test__subtf3(0x1.23456789abcdefp+5Q,
                   makeNaN128(UINT64_C(0x12345)),
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // inf - any = inf
  if (test__subtf3(makeInf128(),
                   0x1.23456789abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // inf - inf = NaN, FE_INVALID
  if (test__subtf3(makeInf128(),
                   makeInf128(),
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // -inf - -inf = NaN, FE_INVALID
  if (test__subtf3(makeNegativeInf128(),
                   makeNegativeInf128(),
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;

  // any - any
  if (test__subtf3(0x1.234567829a3bcdef5678ade36734p+5Q,
                   0x1.ee9d7c52354a6936ab8d7654321fp-1Q,
                   FE_INEXACT,
                   UINT64_C(0x40041b8af1915166),
                   UINT64_C(0xa44a7bca780a166c)))
    return 1;

#if (defined(__arm__) || defined(__aarch64__)) && defined(__ARM_FP) || \
    defined(i386) || defined(__x86_64__)
  // Rounding mode tests on supported architectures
  const f128 m = assertF128Representation(1234.02Q, UINT64_C(0x4009348147ae147a), UINT64_C(0xe147ae147ae147ae));
  const f128 n = assertF128Representation(0.01Q, UINT64_C(0x3ff847ae147ae147), UINT64_C(0xae147ae147ae147b));

  fesetround(FE_UPWARD);
  if (test__subtf3(m, n, FE_INEXACT,
                   UINT64_C(0x40093480a3d70a3d),
                   UINT64_C(0x70a3d70a3d70a3d7)))
    return 1;

  fesetround(FE_DOWNWARD);
  if (test__subtf3(m, n, FE_INEXACT,
                   UINT64_C(0x40093480a3d70a3d),
                   UINT64_C(0x70a3d70a3d70a3d6)))
    return 1;

  fesetround(FE_TOWARDZERO);
  if (test__subtf3(m, n, FE_INEXACT,
                   UINT64_C(0x40093480a3d70a3d),
                   UINT64_C(0x70a3d70a3d70a3d6)))
    return 1;

  fesetround(FE_TONEAREST);
  if (test__subtf3(m, n, FE_INEXACT,
                   UINT64_C(0x40093480a3d70a3d),
                   UINT64_C(0x70a3d70a3d70a3d7)))
    return 1;
#endif
  return 0;
}
#endif
