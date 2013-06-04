/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_REFCOUNTER_H
#define __CDS_REFCOUNTER_H

/*
    Refernce counting primitives

    Editions:
*/

#include <cds/cxx11_atomic.h>

namespace cds {

    /// Simple reference counter
    /**
        Thread-safe reference counter build on atomic primitives.
        \p T is one of integer types
    */
    template <typename T>
    class ref_counter
    {
        CDS_ATOMIC::atomic<T>   m_nRefCount    ;        ///< The reference counter

    public:
        typedef T   ref_counter_type  ; ///< The reference counter type

    public:
        /// Construct reference counter with specified value @p initValue
        ref_counter( T initValue = 0 )
            : m_nRefCount(initValue)
        {
            static_assert( sizeof(T) == sizeof( atomic_t ) , "ref_counter type error")    ;
        }

        /// Get current value of reference counter.
        /*
            The value loading has acquire semantics
        */
        T   value() const
        {
            return m_nRefCount.load( CDS_ATOMIC::memory_order_relaxed ) ;
        }

        /// Current value of reference counter
        /**
            The value loading has acquire semantics
        */
        operator T() const
        {
            return value();
        }

        /// Atomic increment
        /**
            The RMW inc operation has acquire/release semantics
        */
        void    inc()
        {
            m_nRefCount.fetch_add( 1, CDS_ATOMIC::memory_order_relaxed )    ;
        }

        /// Atomic decrement. Return @a true if reference counter is 0.
        /**
            The RMW dec operation has acquire/release semantics
        */
        bool    dec()
        {
            if ( m_nRefCount.fetch_sub( 1, CDS_ATOMIC::memory_order_relaxed ) == 1 ) {
                CDS_ATOMIC::atomic_thread_fence( CDS_ATOMIC::memory_order_release ) ;
                return true ;
            }
            return false ;
        }

        /// Atomic increment
        void operator ++()
        {
            inc()    ;
        }

        /// Atomic decrement
        bool operator--()
        {
            return dec()    ;
        }
    };

    /// Signed 32bit reference counter
    typedef ref_counter<cds::int32_t>       signed_ref_counter      ;

    /// Unsigned 32bit reference counter
    typedef ref_counter<cds::uint32_t>      unsigned_ref_counter    ;

} // namespace cds

#endif    // #ifndef __CDS_REFCOUNTER_H
