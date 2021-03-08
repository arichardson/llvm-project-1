// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_fixunstfsi

#include <stdio.h>

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
unsigned int __fixunstfsi(f128 a);

int test__fixunstfsi(f128 a, unsigned int expected)
{
    unsigned int x = __fixunstfsi(a);
    int ret = (x != expected);

    if (ret)
    {
        printf("error in test__fixunstfsi(%.20Lf) = %u, "
               "expected %u\n", a, x, expected);
    }
    return ret;
}

char assumption_1[sizeof(f128) * CHAR_BIT == 128] = {0};

int main()
{
    if (test__fixunstfsi(makeInf128(), UINT32_C(0xffffffff)))
        return 1;
    if (test__fixunstfsi(0, UINT32_C(0x0)))
        return 1;
    if (test__fixunstfsi(0x1.23456789abcdefp+5, UINT32_C(0x24)))
        return 1;
    if (test__fixunstfsi(0x1.23456789abcdefp-3, UINT32_C(0x0)))
        return 1;
    if (test__fixunstfsi(0x1.23456789abcdefp+20, UINT32_C(0x123456)))
        return 1;
    if (test__fixunstfsi(0x1.23456789abcdefp+40, UINT32_C(0xffffffff)))
        return 1;
    if (test__fixunstfsi(0x1.23456789abcdefp+256, UINT32_C(0xffffffff)))
        return 1;
    if (test__fixunstfsi(-0x1.23456789abcdefp+3, UINT32_C(0x0)))
        return 1;

    if (test__fixunstfsi(0x1.p+32, 0xFFFFFFFFLL))
        return 1;

    return 0;
}
#endif
