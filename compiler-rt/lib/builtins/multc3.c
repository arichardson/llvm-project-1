//===-- multc3.c - Implement __multc3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __multc3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#define QUAD_PRECISION
#include "int_lib.h"
#include "int_math.h"
#include "fp_lib.h"

#if defined(CRT_HAS_F128)

// Returns: the product of a + ib and c + id

COMPILER_RT_ABI Qcomplex __multc3(f128 a, f128 b, f128 c, f128 d) {
  f128 ac = a * c;
  f128 bd = b * d;
  f128 ad = a * d;
  f128 bc = b * c;
  Qcomplex z;
  COMPLEX128_REAL(z) = ac - bd;
  COMPLEX128_IMAGINARY(z) = ad + bc;
  if (crt_isnan(COMPLEX128_REAL(z)) && crt_isnan(COMPLEX128_IMAGINARY(z))) {
    int recalc = 0;
    if (crt_isinf(a) || crt_isinf(b)) {
      a = crt_copysignf128(crt_isinf(a) ? 1 : 0, a);
      b = crt_copysignf128(crt_isinf(b) ? 1 : 0, b);
      if (crt_isnan(c))
        c = crt_copysignf128(0, c);
      if (crt_isnan(d))
        d = crt_copysignf128(0, d);
      recalc = 1;
    }
    if (crt_isinf(c) || crt_isinf(d)) {
      c = crt_copysignf128(crt_isinf(c) ? 1 : 0, c);
      d = crt_copysignf128(crt_isinf(d) ? 1 : 0, d);
      if (crt_isnan(a))
        a = crt_copysignf128(0, a);
      if (crt_isnan(b))
        b = crt_copysignf128(0, b);
      recalc = 1;
    }
    if (!recalc &&
        (crt_isinf(ac) || crt_isinf(bd) || crt_isinf(ad) || crt_isinf(bc))) {
      if (crt_isnan(a))
        a = crt_copysignf128(0, a);
      if (crt_isnan(b))
        b = crt_copysignf128(0, b);
      if (crt_isnan(c))
        c = crt_copysignf128(0, c);
      if (crt_isnan(d))
        d = crt_copysignf128(0, d);
      recalc = 1;
    }
    if (recalc) {
      COMPLEX128_REAL(z) = CRT_INFINITY * (a * c - b * d);
      COMPLEX128_IMAGINARY(z) = CRT_INFINITY * (a * d + b * c);
    }
  }
  return z;
}

#endif
