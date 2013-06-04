/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_COMPILER_CLANG_DEFS_H
#define __CDS_COMPILER_CLANG_DEFS_H

// Compiler version
#define CDS_COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)

// Compiler name
#define  CDS_COMPILER__NAME    ("clang " __clang_version__)
#define  CDS_COMPILER__NICK    "clang"

// OS interface && OS name
#if defined( __linux__ )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_LINUX
#   define CDS_OS__NAME         "linux"
#   define CDS_OS__NICK         "linux"
#elif defined(__MINGW32__) || defined(__MINGW64__)
#   define CDS_OS_INTERFACE     CDS_OSI_WINDOWS
#   define CDS_OS_TYPE          CDS_OS_MINGW
#   define CDS_OS__NAME         "MinGW"
#   define CDS_OS__NICK         "mingw"
#else
#    error GCC: unknown OS type. Compilation aborted
#endif

// Processor architecture
#if defined(__x86_64__) || defined(__amd64__) || defined(__amd64)
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_AMD64
#   define CDS_BUILD_BITS        64
#   define CDS_PROCESSOR__NAME   "Intel x86-64"
#   define CDS_PROCESSOR__NICK   "amd64"
#elif defined(__i386__)
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_X86
#   define CDS_BUILD_BITS        32
#   define CDS_PROCESSOR__NAME   "Intel x86"
#   define CDS_PROCESSOR__NICK   "x86"
#else
#    error "clang: unknown processor architecture. Compilation aborted"
#endif

#ifndef __declspec
#   define __declspec( _x )
#endif

#if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#   define CDS_STDCALL __attribute__((stdcall))
#else
#   define CDS_STDCALL
#endif

#define alignof __alignof__

// Variadic template support (only if -std=c++0x compile-line option provided)
#if __has_feature(cxx_variadic_templates)
#   define CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
#endif

// Default template arguments for function templates
#if __has_feature(cxx_default_function_template_args)
#   define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT
#endif

// C++11 atomic support - only for libc++
#if __has_feature(cxx_atomic) && defined(_LIBCPP_VERSION)
#   define CDS_CXX11_ATOMIC_SUPPORT     1
#endif

#if __has_feature(cxx_deleted_functions)
// C++11 delete definition ( function declaration = delete)
#   define CDS_CXX11_DELETE_DEFINITION_SUPPORT
#endif

#if __has_feature(cxx_defaulted_functions)
// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#   define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
#endif

// Explicit conversion operators
#if __has_feature(cxx_explicit_conversions)
#   define CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT
#endif

// C++11 template alias
#if __has_feature(cxx_alias_templates)
#   define CDS_CXX11_TEMPLATE_ALIAS_SUPPORT
#endif

// C++11 inline namespace
#if __has_feature(cxx_inline_namespaces)
#   define CDS_CXX11_INLINE_NAMESPACE_SUPPORT
#endif

// Lambda
#if __has_feature(cxx_lambdas)
#   define CDS_CXX11_LAMBDA_SUPPORT
#endif

// RValue
#if __has_feature(cxx_rvalue_references)
#   define CDS_RVALUE_SUPPORT
#   define CDS_MOVE_SEMANTICS_SUPPORT
#endif

#if __has_feature(cxx_constexpr)
#   define CDS_CONSTEXPR    constexpr
#else
#   define CDS_CONSTEXPR
#endif

#if __has_feature(cxx_noexcept)
#   define CDS_NOEXCEPT_SUPPORT        noexcept
#   define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)
#else
#   define CDS_NOEXCEPT_SUPPORT
#   define CDS_NOEXCEPT_SUPPORT_(expr)
#endif

#include <cds/compiler/cstdint_std.h>

// Thread support library (thread, mutex, condition variable)
#if CDS_COMPILER_VERSION >= 30100
#   if __has_include(<thread>)
#       define CDS_CXX11_STDLIB_THREAD
#   endif

#   if __has_include(<mutex>)
#       define CDS_CXX11_STDLIB_MUTEX
#   endif

#   if __has_include(<condition_variable>)
#       define CDS_CXX11_STDLIB_CONDITION_VARIABLE
#   endif
#endif

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))


#include <cds/compiler/gcc/compiler_barriers.h>

#endif // #ifndef __CDS_COMPILER_GCC_DEFS_H
