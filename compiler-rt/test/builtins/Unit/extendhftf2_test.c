// RUN: %clang_builtins %s %librt -o %t && %run %t
// REQUIRES: librt_has_extendhftf2

#include "int_lib.h"
#include <stdio.h>
#include "fp_test.h"

#if !defined(CRT_HAS_F128)
int main() {
  fprintf(stderr, "Missing f128 support - skipping.\n");
  return 0;
}
#else
COMPILER_RT_ABI f128 __extendhftf2(TYPE_FP16 a);

int _test__extendhftf2(int line, TYPE_FP16 a, uint64_t expectedHi, uint64_t expectedLo) {
  f128 x = __extendhftf2(a);
  int ret = compareResultF128(x, expectedHi, expectedLo);

  if (ret) {
    fprintf(stderr, "%s:%d: error in test__extendhftf2(%#.4x/%g): ",
            __FILE__, line, toRep16(a), (double)a);
    printMismatchF128(x, expectedHi, expectedLo);
  }
  return ret;
}
#define test__extendhftf2(...) _test__extendhftf2(__LINE__, __VA_ARGS__)


char assumption_1[sizeof(TYPE_FP16) * CHAR_BIT == 16] = {0};

int main() {
  // qNaN
  if (test__extendhftf2(makeQNaN16(),
                        UINT64_C(0x7fff800000000000),
                        UINT64_C(0x0)))
    return 1;
  // NaN
  if (test__extendhftf2(makeNaN16(UINT16_C(0x0100)),
                        UINT64_C(0x7fff400000000000),
                        UINT64_C(0x0)))
    return 1;
  // inf
  if (test__extendhftf2(makeInf16(),
                        UINT64_C(0x7fff000000000000),
                        UINT64_C(0x0)))
    return 1;
  if (test__extendhftf2(makeNegativeInf16(),
                        UINT64_C(0xffff000000000000),
                        UINT64_C(0x0)))
    return 1;
  // zero
  if (test__extendhftf2(fromRep16(0x0U),
                        UINT64_C(0x0), UINT64_C(0x0)))
    return 1;
  if (test__extendhftf2(fromRep16(0x8000U),
                        UINT64_C(0x8000000000000000),
                        UINT64_C(0x0)))
    return 1;
  // denormal
  if (test__extendhftf2(fromRep16(0x0010U),
                        UINT64_C(0x3feb000000000000),
                        UINT64_C(0x0000000000000000)))
    return 1;
  if (test__extendhftf2(fromRep16(0x0001U),
                        UINT64_C(0x3fe7000000000000),
                        UINT64_C(0x0000000000000000)))
    return 1;
  if (test__extendhftf2(fromRep16(0x8001U),
                        UINT64_C(0xbfe7000000000000),
                        UINT64_C(0x0000000000000000)))
    return 1;

  // pi
  if (test__extendhftf2(fromRep16(0x4248U),
                        UINT64_C(0x4000920000000000),
                        UINT64_C(0x0000000000000000)))
    return 1;
  if (test__extendhftf2(fromRep16(0xc248U),
                        UINT64_C(0xc000920000000000),
                        UINT64_C(0x0000000000000000)))
    return 1;

  if (test__extendhftf2(fromRep16(0x508cU),
                        UINT64_C(0x4004230000000000),
                        UINT64_C(0x0)))
    return 1;
  if (test__extendhftf2(fromRep16(0x1bb7U),
                        UINT64_C(0x3ff6edc000000000),
                        UINT64_C(0x0)))
    return 1;
}
#endif
