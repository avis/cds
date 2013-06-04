/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_COMPILER_GCC_COMPILER_BARRIERS_H
#define __CDS_COMPILER_GCC_COMPILER_BARRIERS_H

#define CDS_COMPILER_RW_BARRIER  __asm__ __volatile__ ( "" ::: "memory" )
#define CDS_COMPILER_R_BARRIER   CDS_COMPILER_RW_BARRIER
#define CDS_COMPILER_W_BARRIER   CDS_COMPILER_RW_BARRIER

#endif  // #ifndef __CDS_COMPILER_GCC_COMPILER_BARRIERS_H
