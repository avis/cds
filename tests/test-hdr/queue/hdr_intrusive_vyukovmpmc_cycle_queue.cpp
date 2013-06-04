/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#include "hdr_intrusive_msqueue.h"
#include <cds/intrusive/vyukov_mpmc_cycle_queue.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test2<X>(); }

    namespace {
        struct item {
            int nVal    ;
            int nDisposeCount ;

            item()
                : nDisposeCount(0)
            {}
        };

        typedef ci::VyukovMPMCCycleQueue<
            item
            ,co::buffer< co::v::static_buffer< int, 1024 > >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::memory_model< co::v::sequential_consistent >
        > VyukovMPMCCycleQueue_stat ;

        typedef ci::VyukovMPMCCycleQueue<
            item
            ,co::buffer< co::v::static_buffer< int, 1024 > >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::relaxed_ordering >
        > VyukovMPMCCycleQueue_stat_ic ;

        class VyukovMPMCCycleQueue_dyn
            : public ci::VyukovMPMCCycleQueue<
                item
                ,co::buffer< co::v::dynamic_buffer< int > >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >
        {
            typedef ci::VyukovMPMCCycleQueue<
                item
                ,co::buffer< co::v::dynamic_buffer< int > >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            > base_class ;
        public:
            VyukovMPMCCycleQueue_dyn()
                : base_class( 1024 )
            {}
        };

        class VyukovMPMCCycleQueue_dyn_ic
            : public ci::VyukovMPMCCycleQueue<
                item
                ,co::buffer< co::v::dynamic_buffer< int > >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >
        {
            typedef ci::VyukovMPMCCycleQueue<
                item
                ,co::buffer< co::v::dynamic_buffer< int > >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            > base_class ;
        public:
            VyukovMPMCCycleQueue_dyn_ic()
                : base_class( 1024 )
            {}
        };
    }

    TEST(VyukovMPMCCycleQueue_stat)
    TEST(VyukovMPMCCycleQueue_stat_ic)
    TEST(VyukovMPMCCycleQueue_dyn)
    TEST(VyukovMPMCCycleQueue_dyn_ic)

} // namespace queue
