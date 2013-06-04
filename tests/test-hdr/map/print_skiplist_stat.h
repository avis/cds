/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_TESTHDR_MAP_PRINT_SKIPLIST_STAT_H
#define __CDS_TESTHDR_MAP_PRINT_SKIPLIST_STAT_H

#include "unit/print_skip_list_stat.h"

namespace misc {

    template <typename Stat>
    struct print_skiplist_stat  ;

    template <>
    struct print_skiplist_stat< cds::intrusive::skip_list::stat >
    {
        template <class Set>
        std::string operator()( Set const& s, const char * pszHdr )
        {
            std::stringstream st    ;
            st << "\t\t" << pszHdr << "\n"
                << s.statistics()
                ;
            return st.str() ;
        }
    };

    template<>
    struct print_skiplist_stat< cds::intrusive::skip_list::empty_stat >
    {
        template <class Set>
        std::string operator()( Set const& /*s*/, const char * /*pszHdr*/ )
        {
            return std::string() ;
        }
    };

}   // namespace misc

#endif // #ifndef __CDS_TESTHDR_MAP_PRINT_SKIPLIST_STAT_H
