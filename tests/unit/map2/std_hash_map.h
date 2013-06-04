/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDSUNIT_STD_HASH_MAP_H
#define __CDSUNIT_STD_HASH_MAP_H

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   include "map2/std_hash_map_vc.h"
#elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
#   include "map2/std_hash_map_gcc.h"
#else
#   error "unordered_map is undefined for this compiler"
#endif

#endif // #ifndef __CDSUNIT_STD_HASH_MAP_H
