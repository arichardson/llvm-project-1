// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_floatunsitf

#include "int_lib.h"
#include <stdio.h>

#include "fp_test.h"
#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
COMPILER_RT_ABI f128 __floatunsitf(unsigned int a);

int test__floatunsitf(unsigned int a, uint64_t expectedHi, uint64_t expectedLo)
{
    f128 x = __floatunsitf(a);
    int ret = compareResultF128(x, expectedHi, expectedLo);

    if (ret){
        printf("error in test__floatunsitf(%u) = %.20Lf, "
               "expected %.20Lf\n", a, x, fromRep128(expectedHi, expectedLo));
    }
    return ret;
}

char assumption_1[sizeof(f128) * CHAR_BIT == 128] = {0};

int main()
{
    if (test__floatunsitf(0x7fffffff, UINT64_C(0x401dfffffffc0000), UINT64_C(0x0)))
        return 1;
    if (test__floatunsitf(0, UINT64_C(0x0), UINT64_C(0x0)))
        return 1;
    if (test__floatunsitf(0xffffffff, UINT64_C(0x401efffffffe0000), UINT64_C(0x0)))
        return 1;
    if (test__floatunsitf(0x12345678, UINT64_C(0x401b234567800000), UINT64_C(0x0)))
        return 1;

    return 0;
}
#endif
