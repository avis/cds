/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_DETAILS_STD_CONDITION_VARIABLE_H
#define __CDS_DETAILS_STD_CONDITION_VARIABLE_H

//@cond

#include <cds/details/defs.h>

#ifdef CDS_CXX11_STDLIB_CONDITION_VARIABLE
#   include <condition_variable>
    namespace cds_std {
        using std::condition_variable ;
        using std::condition_variable_any ;
    }
#else
#   include <boost/thread/condition_variable.hpp>
    namespace cds_std {
        using boost::condition_variable ;
        using boost::condition_variable_any ;
    }
#endif

//@endcond

#endif // #ifndef __CDS_DETAILS_STD_CONDITION_VARIABLE_H
