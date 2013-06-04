/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __UNIT_LOCK_WIN32_LOCK_H
#define __UNIT_LOCK_WIN32_LOCK_H

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

#define UNIT_LOCK_WIN_CS

namespace lock {
    namespace win {
        // Win32 critical section
        class CS {
            CRITICAL_SECTION    m_cs    ;

        public:
            CS()    { ::InitializeCriticalSection( &m_cs ) ; }
            ~CS()   { ::DeleteCriticalSection( &m_cs ) ; }

            void lock()     { ::EnterCriticalSection( &m_cs ) ; }
            void unlock()   { ::LeaveCriticalSection( &m_cs)  ; }
            bool tryLock()  { ::TryEnterCriticalSection( &m_cs ) ; }
        };

        class Mutex {
            HANDLE  m_hMutex    ;
        public:

            Mutex()     { m_hMutex = ::CreateMutex( NULL, false, NULL ) ; }
            ~Mutex()    { ::CloseHandle( m_hMutex ) ; }

            void lock()     { ::WaitForSingleObject( m_hMutex, INFINITE ); }
            void unlock()   { ::ReleaseMutex( m_hMutex ); }
            bool tryLock()  { return ::WaitForSingleObject( m_hMutex, 0) == WAIT_OBJECT_0; }
        } ;

    } // namespace win
}   // namespace lock

#endif  // defined(_WIN32) || defined(_WIN64)
#endif  // #ifndef __UNIT_LOCK_WIN32_LOCK_H
