/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


//@cond
#ifndef __CDS_DETAILS_STD_TUPLE_H
#define __CDS_DETAILS_STD_TUPLE_H

#include <cds/details/defs.h>

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION < 1600
// MS VC 2008
#include <boost/tuple/tuple.hpp>

namespace std {
    using boost::tuple  ;
    using boost::get    ;
    using boost::make_tuple ;
    using boost::tie    ;

    template <class Tuple>
    struct tuple_size {
        static size_t const value = boost::tuples::length<Tuple>::value ;
    };

}   // namespace std
#else
#   include <tuple>
#endif

#endif // #ifndef __CDS_DETAILS_STD_MEMORY_H
//@endcond
