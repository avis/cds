/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDSUNIT_STD_HASH_SET_H
#define __CDSUNIT_STD_HASH_SET_H

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == 1500
#   include "set2/std_hash_set_vc9.h"
#else
#   include "set2/std_hash_set_std.h"
#endif

#endif // #ifndef __CDSUNIT_STD_HASH_SET_H
