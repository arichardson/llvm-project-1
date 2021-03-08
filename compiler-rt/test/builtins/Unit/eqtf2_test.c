// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_comparetf2

#include <stdio.h>

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
long __eqtf2(f128 a, f128 b);

int test__eqtf2(f128 a, f128 b, enum EXPECTED_RESULT expected)
{
    long x = __eqtf2(a, b);
    int ret = compareResultCMP(x, expected);

    if (ret){
        printf("error in test__eqtf2(%.20Lf, %.20Lf) = %d, "
               "expected %s\n", a, b, x, expectedStr(expected));
    }
    return ret;
}

char assumption_1[sizeof(f128) * CHAR_BIT == 128] = {0};

int main()
{
    // NaN
    if (test__eqtf2(makeQNaN128(),
                    0x1.234567890abcdef1234567890abcp+3L,
                    NEQUAL_0))
        return 1;
    // <
    // exp
    if (test__eqtf2(0x1.234567890abcdef1234567890abcp-3L,
                    0x1.234567890abcdef1234567890abcp+3L,
                    NEQUAL_0))
        return 1;
    // mantissa
    if (test__eqtf2(0x1.234567890abcdef1234567890abcp+3L,
                    0x1.334567890abcdef1234567890abcp+3L,
                    NEQUAL_0))
        return 1;
    // sign
    if (test__eqtf2(-0x1.234567890abcdef1234567890abcp+3L,
                    0x1.234567890abcdef1234567890abcp+3L,
                    NEQUAL_0))
        return 1;
    // ==
    if (test__eqtf2(0x1.234567890abcdef1234567890abcp+3L,
                    0x1.234567890abcdef1234567890abcp+3L,
                    EQUAL_0))
        return 1;
    // >
    // exp
    if (test__eqtf2(0x1.234567890abcdef1234567890abcp+3L,
                    0x1.234567890abcdef1234567890abcp-3L,
                    NEQUAL_0))
        return 1;
    // mantissa
    if (test__eqtf2(0x1.334567890abcdef1234567890abcp+3L,
                    0x1.234567890abcdef1234567890abcp+3L,
                    NEQUAL_0))
        return 1;
    // sign
    if (test__eqtf2(0x1.234567890abcdef1234567890abcp+3L,
                    -0x1.234567890abcdef1234567890abcp+3L,
                    NEQUAL_0))
        return 1;

    return 0;
}
#endif
