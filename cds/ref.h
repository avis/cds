/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_REF_H
#define __CDS_REF_H

#include <cds/details/defs.h>
#include <boost/ref.hpp>

//@cond
namespace cds {

    using boost::ref    ;

    namespace details {
        template <typename T>
        struct unwrap_reference
        {
            typedef T type  ;
            static T apply(T f)
            {
                return f ;
            }
        };

        template <typename T>
        struct unwrap_reference<T&>
        {
            typedef T& type ;
            static T& apply( T& f )
            {
                return f ;
            }
        };

        template <typename T>
        struct unwrap_reference<boost::reference_wrapper<T> >
        {
            typedef T& type ;
            static T& apply( boost::reference_wrapper<T> f )
            {
                return f.get() ;
            }
        };
    }

    template <typename T>
    static inline typename details::unwrap_reference<T>::type unref( T f )
    {
        return cds::details::unwrap_reference<T>::apply(f)   ;
    }
}
//@endcond

#endif // #ifndef __CDS_REF_H
