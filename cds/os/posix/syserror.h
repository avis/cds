/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_OS_POSIX_SYSERROR_H
#define __CDS_OS_POSIX_SYSERROR_H

#include <stdlib.h>     // system
#include <pthread.h>
#include <sched.h>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <string.h>

namespace cds { namespace OS {
    namespace posix {
        /// Posix error code type
        typedef int                error_code    ;

        /// Get text for system error \p nCode
        static inline std::string getSystemErrorText( error_code nCode )
        {
            char *msg = strerror( nCode )    ;
            return std::string( msg )        ;
        }

        /// Get curent error code (returns \p errno)
        static inline error_code  getErrorCode()
        {
            return errno ;
        }

    }    // namespace posix

    using posix::error_code            ;
    using posix::getErrorCode        ;
    using posix::getSystemErrorText    ;
}} // namespace cds::OS


#endif // #ifndef __CDS_OS_POSIX_SYSERROR_H
