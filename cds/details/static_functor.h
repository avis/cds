/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_DETAILS_STATIC_FUNCTOR_H
#define __CDS_DETAILS_STATIC_FUNCTOR_H

//@cond
namespace cds { namespace details {

    template <class Functor, typename T>
    struct static_functor
    {
        static void call( T * p )
        {
            Functor()( p ) ;
        }
    };

}} // namespace cds::details
//@endcond

#endif // #ifndef __CDS_DETAILS_STATIC_FUNCTOR_H
