#include "fp_mode.h"
#include "fp_test.h"
#include <fenv.h>

static inline bool crt_sets_fp_exc_flags(void) {
  static int result = -1;
  if (result == -1) {
    int old_except = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept(FE_ALL_EXCEPT);
    __fe_raise_invalid();
    result = fetestexcept(FE_INVALID) == FE_INVALID;
    if (old_except != 0)
      feraiseexcept(old_except);
  }
  return result;
}
