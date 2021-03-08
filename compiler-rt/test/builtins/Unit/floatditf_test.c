// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_floatditf

#include "int_lib.h"
#include <math.h>
#include <complex.h>
#include <stdio.h>

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
// Returns: long integer converted to f128

COMPILER_RT_ABI f128 __floatditf(long long a);

int test__floatditf(long long a, uint64_t expectedHi, uint64_t expectedLo)
{
    f128 x = __floatditf(a);
    int ret = compareResultF128(x, expectedHi, expectedLo);

    if (ret)
        printf("error in __floatditf(%Ld) = %.20Lf, "
               "expected %.20Lf\n", a, x, fromRep128(expectedHi, expectedLo));
    return ret;
}

char assumption_1[sizeof(f128) * CHAR_BIT == 128] = {0};

int main()
{
    if (test__floatditf(0x7fffffffffffffff, UINT64_C(0x403dffffffffffff), UINT64_C(0xfffc000000000000)))
        return 1;
    if (test__floatditf(0x123456789abcdef1, UINT64_C(0x403b23456789abcd), UINT64_C(0xef10000000000000)))
        return 1;
    if (test__floatditf(0x2, UINT64_C(0x4000000000000000), UINT64_C(0x0)))
        return 1;
    if (test__floatditf(0x1, UINT64_C(0x3fff000000000000), UINT64_C(0x0)))
        return 1;
    if (test__floatditf(0x0, UINT64_C(0x0), UINT64_C(0x0)))
        return 1;
    if (test__floatditf(0xffffffffffffffff, UINT64_C(0xbfff000000000000), UINT64_C(0x0)))
        return 1;
    if (test__floatditf(0xfffffffffffffffe, UINT64_C(0xc000000000000000), UINT64_C(0x0)))
        return 1;
    if (test__floatditf(-0x123456789abcdef1, UINT64_C(0xc03b23456789abcd), UINT64_C(0xef10000000000000)))
        return 1;
    if (test__floatditf(0x8000000000000000, UINT64_C(0xc03e000000000000), UINT64_C(0x0)))
        return 1;

    return 0;
}
#endif
