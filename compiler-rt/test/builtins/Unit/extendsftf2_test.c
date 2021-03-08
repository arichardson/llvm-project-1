// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_extendsftf2

#include "int_lib.h"
#include <stdio.h>

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
COMPILER_RT_ABI f128 __extendsftf2(float a);

int _test__extendsftf2(int line, float a, uint64_t expectedHi, uint64_t expectedLo) {
  f128 x = __extendsftf2(a);
  int ret = compareResultF128(x, expectedHi, expectedLo);

  if (ret) {
    printf("%s:%d: error in __extendsftf2(%f): ", __FILE__, line, a);
    printMismatchF128(x, expectedHi, expectedLo);
  }
  return ret;
}

#define test__extendsftf2(...) _test__extendsftf2(__LINE__, __VA_ARGS__)


int main()
{
    // qNaN
    if (test__extendsftf2(makeQNaN32(),
                          UINT64_C(0x7fff800000000000),
                          UINT64_C(0x0)))
        return 1;
    // NaN
    if (test__extendsftf2(makeNaN32(UINT32_C(0x410000)),
                          UINT64_C(0x7fff820000000000),
                          UINT64_C(0x0)))
        return 1;
    // inf
    if (test__extendsftf2(makeInf32(),
                          UINT64_C(0x7fff000000000000),
                          UINT64_C(0x0)))
        return 1;
    // zero
    if (test__extendsftf2(0.0f, UINT64_C(0x0), UINT64_C(0x0)))
        return 1;

    if (test__extendsftf2(0x1.23456p+5f,
                          UINT64_C(0x4004234560000000),
                          UINT64_C(0x0)))
        return 1;
    if (test__extendsftf2(0x1.edcbap-9f,
                          UINT64_C(0x3ff6edcba0000000),
                          UINT64_C(0x0)))
        return 1;
    if (test__extendsftf2(0x1.23456p+45f,
                          UINT64_C(0x402c234560000000),
                          UINT64_C(0x0)))
        return 1;
    if (test__extendsftf2(0x1.edcbap-45f,
                          UINT64_C(0x3fd2edcba0000000),
                          UINT64_C(0x0)))
        return 1;

    return 0;
}
#endif
