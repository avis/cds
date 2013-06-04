/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#include <cds/container/rwqueue.h>

#include "queue/queue_test_header.h"

namespace queue {
    void Queue_TestHeader::RWQueue_()
    {
        testNoItemCounter<
            cds::container::RWQueue<
                int
                ,cds::opt::lock_type< cds::SpinLock >
            >
        >()    ;
    }

    void Queue_TestHeader::RWQueue_Counted()
    {
        testWithItemCounter<
            cds::container::RWQueue<
            int
            ,cds::opt::lock_type< cds::SpinLock >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        >()    ;
    }
}
