// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_multf3

#include <stdio.h>

#include "int_lib.h"
#include "fp_test_fenv.h"

#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else

// Returns: a * b
COMPILER_RT_ABI f128 __multf3(f128 a, f128 b);

int _test__multf3(int line, f128 a, f128 b, int expectedExc,
                  uint64_t expectedHi, uint64_t expectedLo) {
  feclearexcept(FE_ALL_EXCEPT);
  f128 x = __multf3(a, b);
  int actualExc = fetestexcept(FE_ALL_EXCEPT);
  int ret = compareResultF128(x, expectedHi, expectedLo);

  if (ret) {
    printMismatchF128TwoArg(__FILE__, line, "__multf3", a, b, x,
                            expectedHi, expectedLo);
  }
  if (crt_sets_fp_exc_flags() && actualExc != expectedExc) {
    fprintf(stderr, "%s:%d: Got unexpected floating-point exception %#x, expected %#x\n",
            __FILE__, line, actualExc, expectedExc);
    return 1;
  }
  return ret;
}
#define test__multf3(...) _test__multf3(__LINE__, __VA_ARGS__)

int main() {
  // qNaN * any = qNaN
  if (test__multf3(makeQNaN128(),
                   0x1.23456789abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // any * qNaN = qNaN
  if (test__multf3(0x1.23456789abcdefp+5Q,
                   makeQNaN128(),
                   0,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // sNaN * any = qNaN, FE_INVALID
  if (test__multf3(makeNaN128(UINT64_C(0x100030000000)),
                   0x1.23456789abcdefp+5Q,
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // any * sNaN = qNaN, FE_INVALID
  if (test__multf3(0x1.23456789abcdefp+5Q,
                   makeNaN128(UINT64_C(0x100030000000)),
                   FE_INVALID,
                   UINT64_C(0x7fff800000000000),
                   UINT64_C(0x0)))
    return 1;
  // inf * any = inf
  if (test__multf3(makeInf128(),
                   0x1.23456789abcdefp+5Q,
                   0,
                   UINT64_C(0x7fff000000000000),
                   UINT64_C(0x0)))
    return 1;

  // +-inf * +-0 = qNaN, FE_INVALID
  if (test__multf3(makeInf128(), 0.0, FE_INVALID, UINT64_C(0x7fff800000000000), UINT64_C(0x0)))
    return 1;
  if (test__multf3(makeInf128(), -0.0, FE_INVALID, UINT64_C(0x7fff800000000000), UINT64_C(0x0)))
    return 1;
  if (test__multf3(makeNegativeInf128(), 0.0, FE_INVALID, UINT64_C(0x7fff800000000000), UINT64_C(0x0)))
    return 1;
  if (test__multf3(makeNegativeInf128(), -0.0, FE_INVALID, UINT64_C(0x7fff800000000000), UINT64_C(0x0)))
    return 1;
  // +-0 * +-inf = qNaN, FE_INVALID
  if (test__multf3(0.0, makeInf128(), FE_INVALID, UINT64_C(0x7fff800000000000), UINT64_C(0x0)))
    return 1;
  if (test__multf3(-0.0, makeInf128(), FE_INVALID, UINT64_C(0x7fff800000000000), UINT64_C(0x0)))
    return 1;
  if (test__multf3(0.0, makeNegativeInf128(), FE_INVALID, UINT64_C(0x7fff800000000000), UINT64_C(0x0)))
    return 1;
  if (test__multf3(-0.0, makeNegativeInf128(), FE_INVALID, UINT64_C(0x7fff800000000000), UINT64_C(0x0)))
    return 1;
  // any * any
  if (test__multf3(0x1.2eab345678439abcdefea56782346p+5Q,
                   0x1.edcb34a235253948765432134674fp-1Q,
                   0,
                   UINT64_C(0x400423e7f9e3c9fc),
                   UINT64_C(0xd906c2c2a85777c4)))
    return 1;
  if (test__multf3(0x1.353e45674d89abacc3a2ebf3ff4ffp-50Q,
                   0x1.ed8764648369535adf4be3214567fp-9Q,
                   0,
                   UINT64_C(0x3fc52a163c6223fc),
                   UINT64_C(0xc94c4bf0430768b4)))
    return 1;
  if (test__multf3(0x1.234425696abcad34a35eeffefdcbap+456Q,
                   0x451.ed98d76e5d46e5f24323dff21ffp+600Q,
                   0,
                   UINT64_C(0x44293a91de5e0e94),
                   UINT64_C(0xe8ed17cc2cdf64ac)))
    return 1;
  if (test__multf3(0x1.4356473c82a9fabf2d22ace345defp-234Q,
                   0x1.eda98765476743ab21da23d45678fp-455Q,
                   0,
                   UINT64_C(0x3d4f37c1a3137cae),
                   UINT64_C(0xfc6807048bc2836a)))
    return 1;
  // underflow
  if (test__multf3(0x1.23456734245345p-10000Q,
                   0x1.edcba524498724p-6497Q,
                   0,
                   UINT64_C(0x0),
                   UINT64_C(0x0)))
    return 1;

  return 0;
}

#endif
