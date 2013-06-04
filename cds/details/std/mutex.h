/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_DETAILS_STD_MUTEX_H
#define __CDS_DETAILS_STD_MUTEX_H

//@cond

#include <cds/details/defs.h>

#ifdef CDS_CXX11_STDLIB_MUTEX
#   include <mutex>
    namespace cds_std {
        using std::mutex ;
        using std::recursive_mutex ;
        using std::unique_lock ;
    }
#else
#   include <boost/thread/mutex.hpp>
#   include <boost/thread/recursive_mutex.hpp>
    namespace cds_std {
        using boost::mutex ;
        using boost::recursive_mutex ;
        using boost::unique_lock ;
    }
#endif

//@endcond

#endif // #ifndef __CDS_DETAILS_STD_MUTEX_H
