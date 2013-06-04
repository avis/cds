/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_DETAILS_HASH_FUNCTOR_SELECTOR_H
#define __CDS_DETAILS_HASH_FUNCTOR_SELECTOR_H

//@cond

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   include <functional>
#elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
    // GCC 4.3+
#   include <functional>
#else
    // Default, use boost implementation
#   include <boost/tr1/functional.hpp>
#endif

namespace cds { namespace details {

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   if CDS_COMPILER_VERSION >= 1600
        // MSVC 2010 and above
        using std::hash         ;
#   else
        // MSVC 2008
        using std::tr1::hash    ;
#   endif
#elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
    // GCC 4.3+
    using std::hash             ;
#else
    // Default, use boost implementation
    using std::tr1::hash    ;
#endif

}} // namespace cds::details
//@endcond

#endif // __CDS_DETAILS_HASH_FUNCTOR_SELECTOR_H
