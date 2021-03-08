#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "int_types.h"

#ifdef COMPILER_RT_HAS_FLOAT16
#define TYPE_FP16 _Float16
#else
#define TYPE_FP16 uint16_t
#endif

#ifdef CRT_HAS_F128
_Static_assert(sizeof(f128) == 16, "");
#endif

enum EXPECTED_RESULT {
    LESS_0, LESS_EQUAL_0, EQUAL_0, GREATER_0, GREATER_EQUAL_0, NEQUAL_0
};

static inline TYPE_FP16 fromRep16(uint16_t x)
{
#ifdef COMPILER_RT_HAS_FLOAT16
    TYPE_FP16 ret;
    memcpy(&ret, &x, sizeof(ret));
    return ret;
#else
    return x;
#endif
}

static inline float fromRep32(uint32_t x)
{
    float ret;
    memcpy(&ret, &x, 4);
    return ret;
}

static inline double fromRep64(uint64_t x)
{
    double ret;
    memcpy(&ret, &x, 8);
    return ret;
}

#ifdef CRT_HAS_F128
static inline f128 fromRep128(uint64_t hi, uint64_t lo) {
  __uint128_t x = ((__uint128_t)hi << 64) + lo;
  f128 ret;
  memcpy(&ret, &x, 16);
  return ret;
}
#endif

static inline uint16_t toRep16(TYPE_FP16 x)
{
#ifdef COMPILER_RT_HAS_FLOAT16
    uint16_t ret;
    memcpy(&ret, &x, sizeof(ret));
    return ret;
#else
    return x;
#endif
}

static inline uint32_t toRep32(float x)
{
    uint32_t ret;
    memcpy(&ret, &x, 4);
    return ret;
}

static inline uint64_t toRep64(double x)
{
    uint64_t ret;
    memcpy(&ret, &x, 8);
    return ret;
}

#ifdef CRT_HAS_F128
static inline __uint128_t toRep128(f128 x) {
  __uint128_t ret;
  memcpy(&ret, &x, 16);
  return ret;
}
#endif

static inline int compareResultH(TYPE_FP16 result,
                                 uint16_t expected)
{
    uint16_t rep = toRep16(result);

    if (rep == expected){
        return 0;
    }
    // test other possible NaN representation(signal NaN)
    else if (expected == 0x7e00U){
        if ((rep & 0x7c00U) == 0x7c00U &&
            (rep & 0x3ffU) > 0){
            return 0;
        }
    }
    return 1;
}

static inline int compareResultF(float result,
                                 uint32_t expected)
{
    uint32_t rep = toRep32(result);

    if (rep == expected){
        return 0;
    }
    // test other possible NaN representation(signal NaN)
    else if (expected == 0x7fc00000U){
        if ((rep & 0x7f800000U) == 0x7f800000U &&
            (rep & 0x7fffffU) > 0){
            return 0;
        }
    }
    return 1;
}

static inline int compareResultD(double result,
                                 uint64_t expected)
{
    uint64_t rep = toRep64(result);

    if (rep == expected){
        return 0;
    }
    // test other possible NaN representation(signal NaN)
    else if (expected == 0x7ff8000000000000UL){
        if ((rep & 0x7ff0000000000000UL) == 0x7ff0000000000000UL &&
            (rep & 0xfffffffffffffUL) > 0){
            return 0;
        }
    }
    return 1;
}

#ifdef CRT_HAS_F128
// return 0 if equal
// use two 64-bit integers intead of one 128-bit integer
// because 128-bit integer constant can't be assigned directly
static inline int compareResultF128(f128 result,
                                    uint64_t expectedHi,
                                    uint64_t expectedLo) {
  __uint128_t rep = toRep128(result);
  uint64_t hi = rep >> 64;
  uint64_t lo = rep;

  if (hi == expectedHi && lo == expectedLo) {
    return 0;
  }
  // test other possible NaN representation(signal NaN)
  else if (expectedHi == 0x7fff800000000000UL && expectedLo == 0x0UL) {
    if ((hi & 0x7fff000000000000UL) == 0x7fff000000000000UL &&
        ((hi & 0xffffffffffffUL) > 0 || lo > 0)) {
      return 0;
    }
  }
  return 1;
}

#if __LDBL_MANT_DIG__ != 113
// Cast to double instead of long double in the printf() call to avoid
// calling __trunctfxf2 since we don't have an implementation for that yet.
#define F128_ARG(x, xrep) ((double)(x)), (unsigned long long)(xrep >> 64), (unsigned long)(xrep)
#define F128_FMT "%.20g / 0x%016llx%016llx"
#else
#define F128_ARG(x, xrep) ((long double)(x)), (unsigned long long)(xrep >> 64), (unsigned long)(xrep)
#define F128_FMT "%.20Lg / 0x%016llx%016llx"
#endif

static inline void printMismatchF128(f128 result, uint64_t expectedHi,
                                     uint64_t expectedLo) {
  __uint128_t resultRep = toRep128(result);
  __uint128_t expectedRep = ((__uint128_t)expectedHi << 64) | expectedLo;
  f128 expected = fromRep128(expectedHi, expectedLo);
  fprintf(stderr, "expected " F128_FMT " but got " F128_FMT "\n",
          F128_ARG(expected, expectedRep),
          F128_ARG(result, resultRep));
}

static inline void printMismatchF128OneArg(const char *file, int line, const char *func,
                                           f128 arg1, f128 result,
                                           uint64_t expectedHi, uint64_t expectedLo) {
  __uint128_t arg1Rep = toRep128(arg1);
  fprintf(stderr, "%s:%d: error in %s(" F128_FMT "): ",
          file, line, func, F128_ARG(arg1, arg1Rep));
  printMismatchF128(result, expectedHi, expectedLo);
}

static inline void printMismatchF128TwoArg(const char *file, int line, const char *func,
                                           f128 arg1, f128 arg2, f128 result,
                                           uint64_t expectedHi, uint64_t expectedLo) {
  __uint128_t arg1Rep = toRep128(arg1);
  __uint128_t arg2Rep = toRep128(arg2);
  fprintf(stderr, "%s:%d: error in %s(" F128_FMT ", " F128_FMT "): ",
          file, line, func, F128_ARG(arg1, arg1Rep),
          F128_ARG(arg2, arg2Rep));
  printMismatchF128(result, expectedHi, expectedLo);
}

#undef F128_ARG
#undef F128_FMT

static inline f128 _assertF128Representation(const char *str, f128 value, uint64_t expectedHi,
                                             uint64_t expectedLo) {
  if (compareResultF128(value, expectedHi, expectedLo)) {
    fprintf(stderr, "Unexpected representation of %s:", str);
    printMismatchF128(value, expectedHi, expectedLo);
    abort();
  }
  return value;
}
#define assertF128Representation(value, ...) _assertF128Representation(#value, value, __VA_ARGS__)

#endif

static inline int compareResultCMP(long result,
                                   enum EXPECTED_RESULT expected) {
  switch (expected) {
  case LESS_0:
    if (result < 0)
      return 0;
    break;
  case LESS_EQUAL_0:
    if (result <= 0)
      return 0;
    break;
  case EQUAL_0:
    if (result == 0)
      return 0;
    break;
  case NEQUAL_0:
    if (result != 0)
      return 0;
    break;
  case GREATER_EQUAL_0:
    if (result >= 0)
      return 0;
    break;
  case GREATER_0:
    if (result > 0)
      return 0;
    break;
  default:
    return 1;
  }
  return 1;
}

static inline char *expectedStr(enum EXPECTED_RESULT expected)
{
    switch(expected){
        case LESS_0:
            return "<0";
        case LESS_EQUAL_0:
            return "<=0";
        case EQUAL_0:
            return "=0";
        case NEQUAL_0:
            return "!=0";
        case GREATER_EQUAL_0:
            return ">=0";
        case GREATER_0:
            return ">0";
        default:
            return "";
    }
    return "";
}

static inline TYPE_FP16 makeQNaN16(void)
{
    return fromRep16(0x7e00U);
}

static inline float makeQNaN32(void)
{
    return fromRep32(0x7fc00000U);
}

static inline double makeQNaN64(void)
{
    return fromRep64(0x7ff8000000000000UL);
}

#ifdef CRT_HAS_F128
static inline f128 makeQNaN128(void) {
  return fromRep128(0x7fff800000000000UL, 0x0UL);
}
#endif

static inline TYPE_FP16 makeNaN16(uint16_t rand)
{
    return fromRep16(0x7c00U | (rand & 0x7fffU));
}

static inline float makeNaN32(uint32_t rand)
{
    return fromRep32(0x7f800000U | (rand & 0x7fffffU));
}

static inline double makeNaN64(uint64_t rand)
{
    return fromRep64(0x7ff0000000000000UL | (rand & 0xfffffffffffffUL));
}

#ifdef CRT_HAS_F128
static inline f128 makeNaN128(uint64_t rand) {
  return fromRep128(0x7fff000000000000UL | (rand & 0xffffffffffffUL), 0x0UL);
}
#endif

static inline TYPE_FP16 makeInf16(void)
{
    return fromRep16(0x7c00U);
}

static inline TYPE_FP16 makeNegativeInf16(void) {
  return fromRep16(0xfc00U);
}

static inline float makeInf32(void)
{
    return fromRep32(0x7f800000U);
}

static inline float makeNegativeInf32(void)
{
    return fromRep32(0xff800000U);
}

static inline double makeInf64(void)
{
    return fromRep64(0x7ff0000000000000UL);
}

static inline double makeNegativeInf64(void)
{
    return fromRep64(0xfff0000000000000UL);
}

#ifdef CRT_HAS_F128
static inline f128 makeInf128(void) {
  return fromRep128(0x7fff000000000000UL, 0x0UL);
}

static inline f128 makeNegativeInf128(void) {
  return fromRep128(0xffff000000000000UL, 0x0UL);
}
#endif
