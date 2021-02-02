#ifndef SUPPORT_ASSERT_MACROS_H
#define SUPPORT_ASSERT_MACROS_H

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <string>

namespace assert_macros_detail {
template <typename T>
inline std::ostream& escape_string(std::ostream& o, const T& begin, const T end) {
    for (T it = begin; it != end; ++it) {
        o << "\\x" << (unsigned long)*it;
    }
    return o;
}

template <typename T>
inline void assert_eq_failed(const std::basic_string<T>& actual,
                             const T& expected, const char* actual_str,
                             const char* expected_str) {
  std::cerr << "FAIL: " << actual_str << " (";
  escape_string(std::cerr, actual) << ") != " << expected_str << " (" << expected << ")\n";
}
template <typename T, typename U>
inline void assert_eq_failed(const T& actual, const U& expected,
                             const char* actual_str, const char* expected_str) {
  std::cerr << "FAIL: " << actual_str << " (" << actual
            << ") != " << expected_str << " (" << expected << ")\n";
}
} // namespace assert_macros_detail

// Note: we expand use the if inside the macro to ensure that 0 is treated as
// NULL in assertions such as ASSERT_EQUAL(x, 0).
#define ASSERT_EQUAL(actual, expected)                                         \
  do {                                                                         \
    if (actual != expected) {                                                  \
      assert_macros_detail::assert_eq_failed(actual, expected, #actual,        \
                                             #expected);                       \
    }                                                                          \
    assert(actual == expected);                                                \
  } while (false)

#endif
