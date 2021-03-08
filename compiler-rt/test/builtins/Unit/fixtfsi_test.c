// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_fixtfsi

#include <stdio.h>

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
int __fixtfsi(f128 a);

int test__fixtfsi(f128 a, int expected)
{
    int x = __fixtfsi(a);
    int ret = (x != expected);

    if (ret){
        printf("error in test__fixtfsi(%.20Lf) = %d, "
               "expected %d\n", a, x, expected);
    }
    return ret;
}

char assumption_1[sizeof(f128) * CHAR_BIT == 128] = {0};

int main()
{
    if (test__fixtfsi(makeInf128(), 0x7fffffff))
        return 1;
    if (test__fixtfsi(0, 0x0))
        return 1;
    if (test__fixtfsi(0x1.23456789abcdefp+5, 0x24))
        return 1;
    if (test__fixtfsi(0x1.23456789abcdefp-3, 0x0))
        return 1;
    if (test__fixtfsi(0x1.23456789abcdefp+20, 0x123456))
        return 1;
    if (test__fixtfsi(0x1.23456789abcdefp+40, 0x7fffffff))
        return 1;
    if (test__fixtfsi(0x1.23456789abcdefp+256, 0x7fffffff))
        return 1;
    if (test__fixtfsi(-0x1.23456789abcdefp+20, 0xffedcbaa))
        return 1;
    if (test__fixtfsi(-0x1.23456789abcdefp+40, 0x80000000))
        return 1;

    return 0;
}
#endif
