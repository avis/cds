/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#include "hdr_deque.h"
#include <cds/gc/hp.h>
#include <cds/container/michael_deque.h>

namespace deque {
#define TEST(X) void DequeHeaderTest::test_##X() { test<X>(); }

    namespace cc = cds::container   ;
    namespace co = cds::opt         ;

    namespace {
        typedef cds::gc::HP HP  ;
        typedef deque::DequeHeaderTest::value_type value_type ;

        typedef cc::MichaelDeque< HP, value_type > MichaelDeque_HP ;

        /// HP + item counter
        typedef cc::MichaelDeque< HP, value_type
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::sequential_consistent >
        > MichaelDeque_HP_ic ;

        /// HP + stat
        typedef cc::MichaelDeque< HP, value_type
            ,co::stat< cds::intrusive::michael_deque::stat<> >
        > MichaelDeque_HP_stat ;

        /// HP + noalign
        typedef cc::MichaelDeque< HP, value_type
            ,co::alignment< co::no_special_alignment >
            ,co::item_counter< cds::atomicity::item_counter >
        > MichaelDeque_HP_noalign ;

    }   // namespace

    TEST( MichaelDeque_HP)
    TEST( MichaelDeque_HP_ic)
    TEST( MichaelDeque_HP_stat)
    TEST( MichaelDeque_HP_noalign)

} // namespace deque

CPPUNIT_TEST_SUITE_REGISTRATION(deque::DequeHeaderTest)    ;
