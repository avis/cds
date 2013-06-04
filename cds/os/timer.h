/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_OS_TIMER_H
#define __CDS_OS_TIMER_H
//@cond

#include <cds/details/defs.h>

#if CDS_OS_TYPE == CDS_OS_WIN32 || CDS_OS_TYPE == CDS_OS_WIN64 || CDS_OS_TYPE == CDS_OS_MINGW
#    include <cds/os/win/timer.h>
#elif CDS_OS_TYPE == CDS_OS_LINUX
#    include <cds/os/linux/timer.h>
#elif CDS_OS_TYPE == CDS_OS_SUN_SOLARIS
#    include <cds/os/sunos/timer.h>
#elif CDS_OS_TYPE == CDS_OS_HPUX
#    include <cds/os/hpux/timer.h>
#elif CDS_OS_TYPE == CDS_OS_AIX
#    include <cds/os/aix/timer.h>
#elif CDS_OS_TYPE == CDS_OS_FREE_BSD
#   include <cds/os/free_bsd/timer.h>
#else
//************************************************************************
// Other OSes
//************************************************************************
#    error Unknown operating system. Compilation aborted.
#endif

//@endcond
#endif    // #ifndef __CDS_OS_TIMER_H
