/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


//@cond
#ifndef __CDS_DETAILS_STD_TYPE_TRAITS_H
#define __CDS_DETAILS_STD_TYPE_TRAITS_H

#include <cds/details/defs.h>
#include <type_traits>

// -----------------------------------------------------------------
// std::integral_constant, std::true_type, std::false_type

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500

namespace std {
    using std::tr1::integral_constant   ;
    using std::tr1::true_type   ;
    using std::tr1::false_type  ;
}   // namespace std
#endif


// -----------------------------------------------------------------
// std::is_same

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500

namespace std {
    using std::tr1::is_same ;
}   // namespace std
#endif


// -----------------------------------------------------------------
// std::conditional

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500

#include <boost/type_traits/conditional.hpp>
namespace std {
    using boost::conditional    ;
}   // namespace std
#endif

// -----------------------------------------------------------------
// std::decay

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500

#include <boost/type_traits/decay.hpp>
namespace std {
    using boost::decay  ;
}   // namespace std
#endif

// -----------------------------------------------------------------
// std::enable_if

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500

namespace std {

    template<bool Test, class Type = void>
    struct enable_if
    {};

    template<class Type>
    struct enable_if<true, Type>
    {
        typedef Type type;
    };

}   // namespace std
#endif

// -----------------------------------------------------------------
// std::remove_const, std::remove_cv, std::remove_volatile
#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500
namespace std {
    using std::tr1::remove_const ;
    using std::tr1::remove_volatile ;
    using std::tr1::remove_cv ;
    using std::tr1::remove_reference ;
}
#endif


//@endcond
#endif  // #ifndef __CDS_DETAILS_STD_TYPE_TRAITS_H
