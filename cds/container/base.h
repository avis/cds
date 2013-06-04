/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_CONTAINER_BASE_H
#define __CDS_CONTAINER_BASE_H

#include <cds/intrusive/base.h>
#include <cds/details/allocator.h>

namespace cds {

/// Standard (non-intrusive) containers
/**
    @ingroup cds_nonintrusive_containers
    This namespace contains implementations of non-intrusive (std-like) lock-free containers.
*/
namespace container {

    /// Common options for non-intrusive containers
    /** @ingroup cds_nonintrusive_helper
        This namespace contains options for non-intrusive containers that is, in general, the same as for the intrusive containers.
        It imports all definitions from cds::opt and cds::intrusive::opt namespaces
    */
    namespace opt {
        using namespace cds::intrusive::opt ;
    }   // namespace opt

    /// @defgroup cds_nonintrusive_containers Non-intrusive containers
    /** @defgroup cds_nonintrusive_helper Helper structs for non-intrusive containers
        @ingroup cds_nonintrusive_containers
    */

    /** @defgroup cds_nonintrusive_stack Stacks
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_queue Queues
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_deque Deque
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_map Maps and Sets
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_list Lists
        @ingroup cds_nonintrusive_containers
    */

}   // namespace container
}   // namespace cds

#endif // #ifndef __CDS_CONTAINER_BASE_H
