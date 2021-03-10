// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_divtf3

#include "int_lib.h"
#include <stdio.h>

#include "fp_test_fenv.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else

// Returns: a / b
COMPILER_RT_ABI f128 __divtf3(f128 a, f128 b);

int _test__divtf3(int line, f128 a, f128 b, int expectedExc,
                  uint64_t expectedHi, uint64_t expectedLo) {
  feclearexcept(FE_ALL_EXCEPT);
  f128 x = __divtf3(a, b);
  int actualExc = fetestexcept(FE_ALL_EXCEPT);
  int ret = compareResultF128(x, expectedHi, expectedLo);
  if (ret) {
    printMismatchF128TwoArg(__FILE__, line, "__divtf3", a, b, x,
                            expectedHi, expectedLo);
  }
  if (crt_sets_fp_exc_flags() && actualExc != expectedExc) {
    fprintf(stderr, "%s:%d: Got unexpected floating-point exception %#x, expected %#x\n",
            __FILE__, line, actualExc, expectedExc);
    return 1;
  }
  return ret;
}
#define test__divtf3(...) _test__divtf3(__LINE__, __VA_ARGS__)

int main() {
  // Returned NaNs are assumed to be qNaN by default

  // qNaN / any = qNaN
  if (test__divtf3(makeQNaN128(),
                   0x1.23456789abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // sNaN / any = qNaN, FE_INVALID
  if (test__divtf3(makeNaN128(UINT64_C(0x30000000)),
                   0x1.23456789abcdefp+5Q,
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // any / qNaN = qNaN
  if (test__divtf3(0x1.23456789abcdefp+5Q,
                   makeQNaN128(),
                   0,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // any / sNaN = NaN, FE_INVALID
  if (test__divtf3(0x1.23456789abcdefp+5Q,
                   makeNaN128(UINT64_C(0x30000000)),
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;

  // +Inf / positive = +Inf
  if (test__divtf3(makeInf128(), 3.Q, 0,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // +Inf / negative = -Inf
  if (test__divtf3(makeInf128(), -3.Q, 0,
                   UINT64_C(0xffff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // -Inf / positive = -Inf
  if (test__divtf3(makeNegativeInf128(), 3.Q, 0,
                   UINT64_C(0xffff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // -Inf / negative = +Inf
  if (test__divtf3(makeNegativeInf128(), -3.Q, 0,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;

  // Inf / Inf = qNaN, FE_INVALID
  if (test__divtf3(makeInf128(), makeInf128(), FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  if (test__divtf3(makeInf128(), makeNegativeInf128(), FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  if (test__divtf3(makeNegativeInf128(), makeInf128(), FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  if (test__divtf3(makeNegativeInf128(), makeNegativeInf128(), FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // +-0.0 / +-0.0 = qNaN, FE_INVALID
  if (test__divtf3(+0x0.0p+0Q, +0x0.0p+0Q, FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  if (test__divtf3(+0x0.0p+0Q, -0x0.0p+0Q, FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  if (test__divtf3(-0x0.0p+0Q, +0x0.0p+0Q, FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  if (test__divtf3(-0x0.0p+0Q, -0x0.0p+0Q, FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // +0.0 / +Inf = +0.0
  if (test__divtf3(+0x0.0p+0Q, makeInf128(), 0,
                   UINT64_C(0x0),
                   UINT64_C(0x0)))
    return 1;
  // +Inf / +0.0 = +Inf
  if (test__divtf3(makeInf128(), +0x0.0p+0Q, 0,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;

  // positive / +0.0 = +Inf
  if (test__divtf3(+1.0Q, +0x0.0p+0Q, FE_DIVBYZERO,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // positive / -0.0 = -Inf
  if (test__divtf3(+1.0Q, -0x0.0p+0Q, FE_DIVBYZERO,
                   UINT64_C(0xffff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // negative / +0.0 = -Inf
  if (test__divtf3(-1.0Q, +0x0.0p+0Q, FE_DIVBYZERO,
                   UINT64_C(0xffff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // negative / -0.0 = +Inf
  if (test__divtf3(-1.0Q, -0x0.0p+0Q, FE_DIVBYZERO,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;

  // 1/3
  if (test__divtf3(1.Q, 3.Q, 0,
                   UINT64_C(0x3ffd555555555555),
                   UINT64_C(0x5555555555555555)))
    return 1;
  // smallest normal result
  if (test__divtf3(0x1.0p-16381Q, 2.Q, 0,
                   UINT64_C(0x0001000000000000),
                   UINT64_C(0x0)))
    return 1;

  // divisor is exactly 1.0
  if (test__divtf3(0x1.0p+0Q,
                   0x1.0p+0Q,
                   0,
                   UINT64_C(0x3fff000000000000),
                   UINT64_C(0x0)))
    return 1;
  // divisor is truncated to exactly 1.0 in UQ1.63
  if (test__divtf3(0x1.0p+0Q,
                   0x1.0000000000000001p+0Q,
                   0,
                   UINT64_C(0x3ffeffffffffffff),
                   UINT64_C(0xfffe000000000000)))
    return 1;

  // smallest normal value divided by 2.0
  if (test__divtf3(0x1.0p-16382Q, 2.Q, 0, UINT64_C(0x0000800000000000), UINT64_C(0x0)))
    return 1;
  // smallest subnormal result
  if (test__divtf3(0x1.0p-16382Q, 0x1p+112Q, 0, UINT64_C(0x0), UINT64_C(0x1)))
    return 1;

  // any / any
  if (test__divtf3(0x1.a23b45362464523375893ab4cdefp+5Q,
                   0x1.eedcbaba3a94546558237654321fp-1Q,
                   0,
                   UINT64_C(0x4004b0b72924d407),
                   UINT64_C(0x0717e84356c6eba2)))
    return 1;
  if (test__divtf3(0x1.a2b34c56d745382f9abf2c3dfeffp-50Q,
                   0x1.ed2c3ba15935332532287654321fp-9Q,
                   0,
                   UINT64_C(0x3fd5b2af3f828c9b),
                   UINT64_C(0x40e51f64cde8b1f2)))
    return 1;
  if (test__divtf3(0x1.2345f6aaaa786555f42432abcdefp+456Q,
                   0x1.edacbba9874f765463544dd3621fp+6400Q,
                   0,
                   UINT64_C(0x28c62e15dc464466),
                   UINT64_C(0xb5a07586348557ac)))
    return 1;
  if (test__divtf3(0x1.2d3456f789ba6322bc665544edefp-234Q,
                   0x1.eddcdba39f3c8b7a36564354321fp-4455Q,
                   0,
                   UINT64_C(0x507b38442b539266),
                   UINT64_C(0x22ce0f1d024e1252)))
    return 1;
  if (test__divtf3(0x1.2345f6b77b7a8953365433abcdefp+234Q,
                   0x1.edcba987d6bb3aa467754354321fp-4055Q,
                   0,
                   UINT64_C(0x50bf2e02f0798d36),
                   UINT64_C(0x5e6fcb6b60044078)))
    return 1;
  if (test__divtf3(6.72420628622418701252535563464350521E-4932Q,
                   2.Q,
                   0,
                   UINT64_C(0x0001000000000000),
                   UINT64_C(0)))
    return 1;

  return 0;
}

#endif
