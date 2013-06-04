/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_GC_HZP_DETAILS_HP_TYPE_H
#define __CDS_GC_HZP_DETAILS_HP_TYPE_H

#include <cds/gc/details/retired_ptr.h>

namespace cds {
    namespace gc {
        namespace hzp {

            /// Hazard pointer
            typedef void *    hazard_pointer    ;

            /// Pointer to function to free (destruct and deallocate) retired pointer of specific type
            typedef cds::gc::details::free_retired_ptr_func free_retired_ptr_func ;
        }
    }
}

#endif // #ifndef __CDS_GC_HZP_DETAILS_HP_TYPE_H


