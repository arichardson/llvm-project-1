//===-- divtc3.c - Implement __divtc3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __divtc3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#define QUAD_PRECISION
#include "fp_lib.h"

#if defined(CRT_HAS_F128)

// Returns: the quotient of (a + ib) / (c + id)

COMPILER_RT_ABI Qcomplex __divtc3(f128 __a, f128 __b, f128 __c, f128 __d) {
  int __ilogbw = 0;
  f128 __logbw = __compiler_rt_logbf128(
      __compiler_rt_fmaxf128(crt_fabsf128(__c), crt_fabsf128(__d)));
  if (crt_isfinite(__logbw)) {
    __ilogbw = (int)__logbw;
    __c = __compiler_rt_scalbnf128(__c, -__ilogbw);
    __d = __compiler_rt_scalbnf128(__d, -__ilogbw);
  }
  f128 __denom = __c * __c + __d * __d;
  Qcomplex z;
  COMPLEX128_REAL(z) =
      __compiler_rt_scalbnf128((__a * __c + __b * __d) / __denom, -__ilogbw);
  COMPLEX128_IMAGINARY(z) =
      __compiler_rt_scalbnf128((__b * __c - __a * __d) / __denom, -__ilogbw);
  if (crt_isnan(COMPLEX128_REAL(z)) && crt_isnan(COMPLEX128_IMAGINARY(z))) {
    if ((__denom == 0.0) && (!crt_isnan(__a) || !crt_isnan(__b))) {
      COMPLEX128_REAL(z) = crt_copysignf128(CRT_INFINITY, __c) * __a;
      COMPLEX128_IMAGINARY(z) = crt_copysignf128(CRT_INFINITY, __c) * __b;
    } else if ((crt_isinf(__a) || crt_isinf(__b)) && crt_isfinite(__c) &&
               crt_isfinite(__d)) {
      __a = crt_copysignf128(crt_isinf(__a) ? (f128)1.0 : (f128)0.0, __a);
      __b = crt_copysignf128(crt_isinf(__b) ? (f128)1.0 : (f128)0.0, __b);
      COMPLEX128_REAL(z) = CRT_INFINITY * (__a * __c + __b * __d);
      COMPLEX128_IMAGINARY(z) = CRT_INFINITY * (__b * __c - __a * __d);
    } else if (crt_isinf(__logbw) && __logbw > 0.0 && crt_isfinite(__a) &&
               crt_isfinite(__b)) {
      __c = crt_copysignf128(crt_isinf(__c) ? (f128)1.0 : (f128)0.0, __c);
      __d = crt_copysignf128(crt_isinf(__d) ? (f128)1.0 : (f128)0.0, __d);
      COMPLEX128_REAL(z) = 0.0 * (__a * __c + __b * __d);
      COMPLEX128_IMAGINARY(z) = 0.0 * (__b * __c - __a * __d);
    }
  }
  return z;
}

#endif
