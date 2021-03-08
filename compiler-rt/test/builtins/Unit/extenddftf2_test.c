// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_extenddftf2

#include "int_lib.h"
#include "fp_test.h"
#include <stdio.h>

#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
COMPILER_RT_ABI f128 __extenddftf2(double a);

int _test__extenddftf2(int line, double a, uint64_t expectedHi, uint64_t expectedLo)
{
    f128 x = __extenddftf2(a);
    int ret = compareResultF128(x, expectedHi, expectedLo);

    if (ret){
        fprintf(stderr, "%s:%d: error in test__extenddftf2(%f): ", __FILE__, line, a);
        printMismatchF128(x, expectedHi, expectedLo);
    }
    return ret;
}

#define test__extenddftf2(...) _test__extenddftf2(__LINE__, __VA_ARGS__)


int main()
{
    // qNaN
    if (test__extenddftf2(makeQNaN64(),
                          UINT64_C(0x7fff800000000000),
                          UINT64_C(0x0)))
        return 1;
    // NaN
    if (test__extenddftf2(makeNaN64(UINT64_C(0x7100000000000)),
                          UINT64_C(0x7fff710000000000),
                          UINT64_C(0x0)))
        return 1;
    // inf
    if (test__extenddftf2(makeInf64(),
                          UINT64_C(0x7fff000000000000),
                          UINT64_C(0x0)))
        return 1;
    // zero
    if (test__extenddftf2(0.0, UINT64_C(0x0), UINT64_C(0x0)))
        return 1;

    if (test__extenddftf2(0x1.23456789abcdefp+5,
                          UINT64_C(0x400423456789abcd),
                          UINT64_C(0xf000000000000000)))
        return 1;
    if (test__extenddftf2(0x1.edcba987654321fp-9,
                          UINT64_C(0x3ff6edcba9876543),
                          UINT64_C(0x2000000000000000)))
        return 1;
    if (test__extenddftf2(0x1.23456789abcdefp+45,
                          UINT64_C(0x402c23456789abcd),
                          UINT64_C(0xf000000000000000)))
        return 1;
    if (test__extenddftf2(0x1.edcba987654321fp-45,
                          UINT64_C(0x3fd2edcba9876543),
                          UINT64_C(0x2000000000000000)))
        return 1;

    return 0;
}
#endif
