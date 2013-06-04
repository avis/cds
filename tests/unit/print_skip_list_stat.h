/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __UNIT_PRINT_SKIP_LIST_STAT_H
#define __UNIT_PRINT_SKIP_LIST_STAT_H

#include <cds/intrusive/skip_list_base.h>
#include <ostream>

namespace std {

    static inline ostream& operator <<( ostream& o, cds::intrusive::skip_list::stat const& s )
    {
        o << "Skip-list stat [cds::intrusive::skip_list::stat]\n"
            << "\t\t node height (add/delete):\n"
            ;
        for ( unsigned int i = 0; i < 9; ++i )
            o << "\t\t\t[ " << (i + 1) << "]: " << s.m_nNodeHeightAdd[i].get() << '/' << s.m_nNodeHeightDel[i].get() << "\n" ;
        for ( unsigned int i = 9; i < sizeof(s.m_nNodeHeightAdd) / sizeof(s.m_nNodeHeightAdd[0] ); ++i )
            o << "\t\t\t[" << (i + 1) << "]: " << s.m_nNodeHeightAdd[i].get() << '/' << s.m_nNodeHeightDel[i].get() << "\n" ;

        return o
            << "\t\t     m_nInsertSuccess: " << s.m_nInsertSuccess.get()    << "\n"
            << "\t\t      m_nInsertFailed: " << s.m_nInsertFailed.get()     << "\n"
            << "\t\t     m_nInsertRetries: " << s.m_nInsertRetries.get()    << "\n"
            << "\t\t       m_nEnsureExist: " << s.m_nEnsureExist.get()      << "\n"
            << "\t\t         m_nEnsureNew: " << s.m_nEnsureNew.get()        << "\n"
            << "\t\t     m_nUnlinkSuccess: " << s.m_nUnlinkSuccess.get()    << "\n"
            << "\t\t      m_nUnlinkFailed: " << s.m_nUnlinkFailed.get()     << "\n"
            << "\t\t      m_nEraseSuccess: " << s.m_nEraseSuccess.get()     << "\n"
            << "\t\t       m_nEraseFailed: " << s.m_nEraseFailed.get()      << "\n"
            << "\t\t   m_nFindFastSuccess: " << s.m_nFindFastSuccess.get()  << "\n"
            << "\t\t    m_nFindFastFailed: " << s.m_nFindFastFailed.get()   << "\n"
            << "\t\t   m_nFindSlowSuccess: " << s.m_nFindSlowSuccess.get()  << "\n"
            << "\t\t    m_nFindSlowFailed: " << s.m_nFindSlowFailed.get()   << "\n"
            ;
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::skip_list::empty_stat const& s )
    {
        return o ;
    }

} // namespace std

#endif // #ifndef __UNIT_PRINT_SKIP_LIST_STAT_H
