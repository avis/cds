/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_THREADING_AUTO_DETECT_H
#define __CDS_THREADING_AUTO_DETECT_H

#if defined(CDS_THREADING_AUTODETECT)
    // Auto-detect appropriate threading model
#   if CDS_COMPILER == CDS_COMPILER_MSVC
        // For MSVC, CDS_THREADING_MSVC and CDS_THREADING_WIN_TLS is supported.
        // CDS_THREADING_MSVC must be explicitly defined if needed
#       if !defined(CDS_THREADING_MSVC) && !defined(CDS_THREADING_WIN_TLS) && !defined(CDS_THREADING_CXX11)
#           define CDS_THREADING_WIN_TLS
#       endif
#   elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
        // For GCC, CDS_THREADING_GCC and CDS_THREADING_PTHREAD is supported
        // CDS_THREADING_GCC must be explicitly defined if needed
#       if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
#           if !defined(CDS_THREADING_GCC) && !defined(CDS_THREADING_WIN_TLS) && !defined(CDS_THREADING_CXX11)
#               define CDS_THREADING_WIN_TLS
#           endif
#       elif !defined(CDS_THREADING_GCC) && !defined(CDS_THREADING_PTHREAD) && !defined(CDS_THREADING_CXX11)
#           define CDS_THREADING_PTHREAD
#       endif
#   else
#       error "Unknown compiler for cds::threading"
#   endif
#endif // #if defined(CDS_THREADING_AUTODETECT)



#if defined(CDS_THREADING_MSVC)
#   include <cds/threading/details/msvc.h>
#elif defined(CDS_THREADING_WIN_TLS)
#   include <cds/threading/details/wintls.h>
#elif defined(CDS_THREADING_PTHREAD)
#   include <cds/threading/details/pthread.h>
#elif defined(CDS_THREADING_GCC)
#   include <cds/threading/details/gcc.h>
#elif defined(CDS_THREADING_CXX11)
#   include <cds/threading/details/cxx11.h>
#elif !defined(CDS_THREADING_USER_DEFINED)
#   error "You must define one of CDS_THREADING_xxx macro before compiling the application"
#endif

#endif // #ifndef __CDS_THREADING_AUTO_DETECT_H
