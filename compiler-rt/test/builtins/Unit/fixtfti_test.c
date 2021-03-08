// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_fixtfti

#include "int_lib.h"
#include <stdio.h>

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
ti_int __fixtfti(f128 a);

int test__fixtfti(f128 a, ti_int expected)
{
    ti_int x = __fixtfti(a);
    int ret = (x != expected);

    if (ret)
    {
        twords xt;
        xt.all = x;

        twords expectedt;
        expectedt.all = expected;

        printf("error in test__fixtfti(%.20f) = 0x%.16llX%.16llX, "
               "expected 0x%.16llX%.16llX\n",
               (double)a, xt.s.high, xt.s.low, expectedt.s.high, expectedt.s.low);
    }
    return ret;
}

char assumption_1[sizeof(f128) * CHAR_BIT == 128] = {0};

int main()
{
    if (test__fixtfti(makeInf128(), make_ti(0x7fffffffffffffffLL,
                                            0xffffffffffffffffLL)))
        return 1;
    if (test__fixtfti(0, make_ti(0x0LL, 0x0LL)))
        return 1;
    if (test__fixtfti(0x1.23456789abcdefp+5Q, make_ti(0x0LL, 0x24LL)))
        return 1;
    if (test__fixtfti(0x1.23456789abcdefp-3Q, make_ti(0x0LL, 0x0LL)))
        return 1;
    if (test__fixtfti(0x1.23456789abcdef12345678p+20Q,
                      make_ti(0x0LL, 0x123456LL)))
        return 1;
    if (test__fixtfti(0x1.23456789abcdef123456789abcdep+112Q,
                      make_ti(0x123456789abcdLL, 0xef123456789abcdeLL)))
        return 1;
    if (test__fixtfti(-0x1.23456789abcdef123456789abcdep+112Q,
                      make_ti(0xFFFEDCBA98765432LL, 0x10EDCBA987654322LL)))
        return 1;
    if (test__fixtfti(0x1.23456789abcdefp+256Q, make_ti(0x7fffffffffffffffLL,
                                                        0xffffffffffffffffLL)))
        return 1;
    if (test__fixtfti(-0x1.23456789abcdefp+20Q, make_ti(0xffffffffffffffffLL,
                                                        0xffffffffffedcbaaLL)))
        return 1;
    if (test__fixtfti(-0x1.23456789abcdefp+256Q, make_ti(0x8000000000000000LL,
                                                         0x0)))
        return 1;

    return 0;
}
#endif
