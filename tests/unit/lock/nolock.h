/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __UNIT_LOCK_NOLOCK_H
#define __UNIT_LOCK_NOLOCK_H

namespace lock {
    // Win32 critical section
    class NoLock {
    public:
        void lock()     {}
        void unlock()   {}
        bool tryLock()  { return true; }
    };
}

#endif // #ifndef __UNIT_LOCK_NOLOCK_H
