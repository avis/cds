/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_COMPILER_GCC_DEFS_H
#define __CDS_COMPILER_GCC_DEFS_H

// Compiler version
#define CDS_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if CDS_COMPILER_VERSION < 40300
#   error "Compiler version error. GCC version 4.3.0 and above is supported"
#endif

// Compiler name
#ifdef __VERSION__
#   define  CDS_COMPILER__NAME    ("GNU C++ " __VERSION__)
#else
#   define  CDS_COMPILER__NAME    "GNU C++"
#endif
#define  CDS_COMPILER__NICK        "gcc"

// OS interface && OS name
#if defined( __linux__ )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_LINUX
#   define CDS_OS__NAME         "linux"
#   define CDS_OS__NICK         "linux"
#elif defined( __sun__ )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_SUN_SOLARIS
#   define CDS_OS__NAME         "Sun Solaris"
#   define CDS_OS__NICK         "sun"
#elif defined( __hpux__ )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_HPUX
#   define CDS_OS__NAME         "HP-UX"
#   define CDS_OS__NICK         "hpux"
#elif defined( _AIX )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_AIX
#   define CDS_OS__NAME         "AIX"
#   define CDS_OS__NICK         "aix"
#elif defined( __FreeBSD__ )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_FREE_BSD
#   define CDS_OS__NAME         "FreeBSD"
#   define CDS_OS__NICK         "freebsd"
#elif defined( __OpenBSD__ )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_OPEN_BSD
#   define CDS_OS__NAME         "OpenBSD"
#   define CDS_OS__NICK         "openbsd"
#elif defined( __NetBSD__ )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_NET_BSD
#   define CDS_OS__NAME         "NetBSD"
#   define CDS_OS__NICK         "netbsd"
#elif defined(__MINGW32__) || defined( __MINGW64__)
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
#elif defined(sparc) || defined (__sparc__)
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_SPARC
#   define CDS_PROCESSOR__NAME   "Sparc"
#   define CDS_PROCESSOR__NICK   "sparc"
#   ifdef __arch64__
#       define CDS_BUILD_BITS   64
#    else
#       error Sparc 32bit is not supported
#    endif
#elif defined( __ia64__)
#    define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_IA64
#    define CDS_BUILD_BITS        64
#    define CDS_PROCESSOR__NAME   "Intel IA64"
#    define CDS_PROCESSOR__NICK   "ia64"
#elif defined(_ARCH_PPC64)
#    define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_PPC64
#    define CDS_BUILD_BITS        64
#    define CDS_PROCESSOR__NAME   "IBM PowerPC64"
#    define CDS_PROCESSOR__NICK   "ppc64"
#else
#    error "GCC: unknown processor architecture. Compilation aborted"
#endif

#if CDS_OS_TYPE == CDS_OS_MINGW
#   ifdef CDS_BUILD_LIB
#       define CDS_EXPORT_API          __declspec(dllexport)
#   else
#       define CDS_EXPORT_API          __declspec(dllimport)
#   endif
#else
#   ifndef __declspec
#       define __declspec( _x )
#   endif
#endif

#if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#   define CDS_STDCALL __attribute__((stdcall))
#else
#   define CDS_STDCALL
#endif

#define alignof __alignof__

// ***************************************
// C++11 features

// Variadic template support (only if -std=c++0x compile-line option provided)
#if CDS_COMPILER_VERSION >= 40300
#   define CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
#endif

// RValue
#if CDS_COMPILER_VERSION >= 40300
#   define CDS_RVALUE_SUPPORT
#   define CDS_MOVE_SEMANTICS_SUPPORT
#endif

// Default template arguments for function templates
#if CDS_COMPILER_VERSION >= 40300
#   define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT
#endif

// C++11 inline namespace
#if CDS_COMPILER_VERSION >= 40400
#   define CDS_CXX11_INLINE_NAMESPACE_SUPPORT
#endif

// Delete definition and explicitly-defaulted function
#if CDS_COMPILER_VERSION >= 40400
// C++11 delete definition ( function declaration = delete)
#   define CDS_CXX11_DELETE_DEFINITION_SUPPORT

// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#   define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
#endif

// Lambda
#if CDS_COMPILER_VERSION >= 40500
#   define CDS_CXX11_LAMBDA_SUPPORT
#   if CDS_COMPILER_VERSION < 40800
#       define CDS_BUG_STATIC_MEMBER_IN_LAMBDA
#   endif
#endif

// Explicit conversion operator
#if CDS_COMPILER_VERSION >= 40500
#   define CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT
#endif

#if CDS_COMPILER_VERSION >= 40600
#   define CDS_CONSTEXPR    constexpr
#   define CDS_NOEXCEPT_SUPPORT        noexcept
#   define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)
#   if CDS_COMPILER_VERSION >= 40600 && CDS_COMPILER_VERSION < 40700
// GCC 4.6.x does not allow noexcept specification in defaulted function
// void foo() noexcept = default
// error: function ‘foo’ defaulted on its first declaration must not have an exception-specification
#       define CDS_NOEXCEPT_DEFAULTED
#       define CDS_NOEXCEPT_DEFAULTED_(expr)
#   endif
#else
#   define CDS_CONSTEXPR
#   define CDS_NOEXCEPT_SUPPORT
#   define CDS_NOEXCEPT_SUPPORT_(expr)
#endif


// C++11 atomic support
#if CDS_COMPILER_VERSION >= 40700
#   define CDS_CXX11_ATOMIC_SUPPORT     1
#elif defined(CDS_CXX11_ATOMIC_GCC)
#   define CDS_CXX11_ATOMIC_SUPPORT     1
#endif

// C++11 template alias
#if CDS_COMPILER_VERSION >= 40700
#   define CDS_CXX11_TEMPLATE_ALIAS_SUPPORT
#endif

// C++11 thread_local keyword
#if CDS_COMPILER_VERSION >= 40800
#   define CDS_CXX11_THREAD_LOCAL_SUPPORT
#endif


#if defined( CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT ) && defined(CDS_MOVE_SEMANTICS_SUPPORT) && CDS_COMPILER_VERSION >= 40400 && CDS_COMPILER_VERSION < 40600
    // GCC 4.4 - 4.5 bug: move ctor & assignment operator cannot be defaulted
#   define CDS_DISABLE_DEFAULT_MOVE_CTOR
#endif

#include <cds/compiler/cstdint_std.h>

// Thread support library (thread, mutex, condition variable)
#if CDS_COMPILER_VERSION >= 40400
#   define CDS_CXX11_STDLIB_THREAD
#   define CDS_CXX11_STDLIB_MUTEX
#   define CDS_CXX11_STDLIB_CONDITION_VARIABLE
#endif

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))


#include <cds/compiler/gcc/compiler_barriers.h>

#endif // #ifndef __CDS_COMPILER_GCC_DEFS_H
