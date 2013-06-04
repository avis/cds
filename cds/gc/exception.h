/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_GC_EXCEPTION_H
#define __CDS_GC_EXCEPTION_H

#include <cds/details/defs.h>

namespace cds { namespace gc {

    /// %Exception "Too little hazard pointers"
    CDS_DECLARE_EXCEPTION( too_little_hazard_pointers, "Too little hazard pointers" ) ;

}} // namespace cds::gc

#endif // #ifndef __CDS_GC_EXCEPTION_H
