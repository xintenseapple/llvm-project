//===-- Common internal contructs -------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SUPPORT_COMMON_H
#define LLVM_LIBC_SUPPORT_COMMON_H

#define LIBC_INLINE_ASM __asm__ __volatile__

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(x, 0)
#endif
#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

#ifndef LLVM_LIBC_FUNCTION_ATTR
#define LLVM_LIBC_FUNCTION_ATTR
#endif

#ifndef LIBC_INLINE
#define LIBC_INLINE inline
#endif

#if defined(__AMDGPU__) || defined(__NVPTX__)
#define PACKAGE_FOR_GPU
#endif

// GPU targets do not support aliasing.
#if defined(LLVM_LIBC_PUBLIC_PACKAGING) && defined(PACKAGE_FOR_GPU)
#define LLVM_LIBC_FUNCTION(type, name, arglist)                                \
  LLVM_LIBC_FUNCTION_ATTR decltype(__llvm_libc::name)                          \
      __##name##_impl__ __asm__(#name);                                        \
  type __##name##_impl__ arglist
// MacOS needs to be excluded because it does not support aliasing.
#elif defined(LLVM_LIBC_PUBLIC_PACKAGING) && (!defined(__APPLE__))
#define LLVM_LIBC_FUNCTION(type, name, arglist)                                \
  LLVM_LIBC_FUNCTION_ATTR decltype(__llvm_libc::name)                          \
      __##name##_impl__ __asm__(#name);                                        \
  decltype(__llvm_libc::name) name [[gnu::alias(#name)]];                      \
  type __##name##_impl__ arglist
#else
#define LLVM_LIBC_FUNCTION(type, name, arglist) type name arglist
#endif

namespace __llvm_libc {
namespace internal {
constexpr bool same_string(char const *lhs, char const *rhs) {
  for (; *lhs || *rhs; ++lhs, ++rhs)
    if (*lhs != *rhs)
      return false;
  return true;
}
} // namespace internal
} // namespace __llvm_libc

// LLVM_LIBC_IS_DEFINED checks whether a particular macro is defined.
// Usage: constexpr bool kUseAvx = LLVM_LIBC_IS_DEFINED(__AVX__);
//
// This works by comparing the stringified version of the macro with and without
// evaluation. If FOO is not undefined both stringifications yield "FOO". If FOO
// is defined, one stringification yields "FOO" while the other yields its
// stringified value "1".
#define LLVM_LIBC_IS_DEFINED(macro)                                            \
  !__llvm_libc::internal::same_string(                                         \
      LLVM_LIBC_IS_DEFINED__EVAL_AND_STRINGIZE(macro), #macro)
#define LLVM_LIBC_IS_DEFINED__EVAL_AND_STRINGIZE(s) #s

#endif // LLVM_LIBC_SUPPORT_COMMON_H
