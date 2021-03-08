// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_fixtfdi

#include "int_lib.h"
#include <stdio.h>

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
di_int __fixtfdi(f128 a);

int test__fixtfdi(f128 a, di_int expected)
{
    di_int x = __fixtfdi(a);
    int ret = (x != expected);

    if (ret)
    {
        printf("error in test__fixtfdi(%.20Lf) = %llX, "
               "expected %llX\n", a, x, expected);
    }
    return ret;
}

char assumption_1[sizeof(f128) * CHAR_BIT == 128] = {0};

int main()
{
    if (test__fixtfdi(makeInf128(), 0x7fffffffffffffffLL))
        return 1;
    if (test__fixtfdi(0, 0x0))
        return 1;
    if (test__fixtfdi(0x1.23456789abcdefp+5L, 0x24LL))
        return 1;
    if (test__fixtfdi(0x1.23456789abcdefp-3L, 0x0LL))
        return 1;
    if (test__fixtfdi(0x1.23456789abcdef12345678p+20L, 0x123456LL))
        return 1;
    if (test__fixtfdi(0x1.23456789abcdef12345678p+40L, 0x123456789abLL))
        return 1;
    if (test__fixtfdi(0x1.23456789abcdef12345678p+60L, 0x123456789abcdef1LL))
        return 1;
    if (test__fixtfdi(0x1.23456789abcdefp+256L, 0x7fffffffffffffffLL))
        return 1;
    if (test__fixtfdi(-0x1.23456789abcdefp+20L, 0xffffffffffedcbaaLL))
        return 1;
    if (test__fixtfdi(-0x1.23456789abcdefp+40L, 0xfffffedcba987655LL))
        return 1;
    if (test__fixtfdi(-0x1.23456789abcdefp+256L, 0x8000000000000000LL))
        return 1;

    printf("skipped\n");

    return 0;
}
#endif
