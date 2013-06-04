/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


//@cond
#ifndef __CDS_DETAILS_STD_MEMORY_H
#define __CDS_DETAILS_STD_MEMORY_H

#include <cds/details/defs.h>
#include <memory>

// -----------------------------------------------------------------
// std::unique_ptr

#if (CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500) || (CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION < 40400 )
// MS VC 2008, GCC 4.3

namespace std {
    template <typename T, typename Deleter>
    class unique_ptr: private auto_ptr<T>
    {
        typedef auto_ptr<T> base_class ;

        // copy ctor is deleted
        template <typename Q, typename D> unique_ptr( unique_ptr<Q, D> const& s ) ;

    public:
        unique_ptr() throw()
        {}

        explicit unique_ptr( T * p ) throw()
            : base_class( p )
        {}

        ~unique_ptr()
        {
            T * p = release() ;
            if ( p )
                Deleter()( p )  ;
        }

        T * operator ->() const throw()
        {
            return base_class::operator->() ;
        }

        T& operator *() throw()
        {
            return base_class::operator*() ;
        }

        unique_ptr& operator=( T * p )
        {
            base_class::operator=(p)    ;
            return *this ;
        }

        T * get() const throw()
        {
            return base_class::get() ;
        }

        T * release() throw()
        {
            return base_class::release() ;
        }
    };
}

#endif
// -----------------------------------------------------------------
// std::shared_ptr

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500
// MS VC 2008

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace std {
    using boost::shared_ptr     ;
    using boost::make_shared    ;
    using boost::allocate_shared;
    using boost::enable_shared_from_this   ;
}   // namespace std
#endif


#endif // #ifndef __CDS_DETAILS_STD_MEMORY_H
//@endcond
