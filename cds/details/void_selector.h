/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_DETAILS_VOID_SELECTOR_H
#define __CDS_DETAILS_VOID_SELECTOR_H

#include <cds/details/std/type_traits.h>

namespace cds {
    namespace details {

        /// Void type selector
        /**
            This metafunction is equal to the following expression:
            \code
            std::conditional< std::is_same< T, void >::value, Void, NoVoid >::type
            \endcode

            The \p NoVoid is optional, default is \p NoVoid == T
        */
        template <typename T, typename Void, typename NoVoid = T>
        struct void_selector: public std::conditional< std::is_same< T, void >::value, Void, NoVoid >
        {};

    }    // namespace details
} // namespace cds

#endif // #ifndef __CDS_DETAILS_VOID_SELECTOR_H
