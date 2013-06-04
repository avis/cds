/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_DETAILS_MAKE_CONST_TYPE_H
#define __CDS_DETAILS_MAKE_CONST_TYPE_H

#include <cds/details/defs.h>

namespace cds { namespace details {

    //@cond
    template <typename T, bool B>
    struct make_const_type
    {
        typedef T      type ;
        typedef T *    pointer     ;
        typedef T &    reference   ;
    };
    template<typename T>
    struct make_const_type<T, true>
    {
        typedef T const      type ;
        typedef T const *    pointer   ;
        typedef T const &    reference ;
    };

    //@endcond

}}  // namespace cds::details

#endif  // #ifndef __CDS_DETAILS_MAKE_CONST_TYPE_H
