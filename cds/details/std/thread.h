/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_DETAILS_STD_THREAD_H
#define __CDS_DETAILS_STD_THREAD_H

//@cond

#include <cds/details/defs.h>

#ifdef CDS_CXX11_STDLIB_THREAD
#   include <thread>
    namespace cds_std {
        using std::thread ;
    }
#else
#   include <boost/thread/thread.hpp>
    namespace cds_std {
        using boost::thread ;
    }
#endif

//@endcond

#endif // #ifndef __CDS_DETAILS_STD_THREAD_H
