/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDSTEST_SIZE_CHECK_H
#define __CDSTEST_SIZE_CHECK_H

#include <cds/cxx11_atomic.h>

namespace misc {

    template <typename ItemCounter>
    struct size_checker {
        template <typename Container>
        bool operator()( Container const& c, size_t nSize ) const
        {
            return c.size() == nSize ;
        }
    };

    template<>
    struct size_checker< cds::atomicity::empty_item_counter > {
        template <typename Container>
        bool operator()( Container const& /*c*/, size_t /*nSize*/ ) const
        {
            return true ;
        }
    };

    template <class Container>
    static inline bool check_size( Container const& cont, size_t nSize )
    {
        return size_checker<typename Container::item_counter>()( cont, nSize ) ;
    }

} // namespace misc

#endif // #ifndef __CDSTEST_SIZE_CHECK_H
