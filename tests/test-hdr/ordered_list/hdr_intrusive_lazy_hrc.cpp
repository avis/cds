/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#include "ordered_list/hdr_intrusive_lazy.h"
#include <cds/intrusive/lazy_list_hrc.h>

namespace ordlist {

    void IntrusiveLazyListHeaderTest::HRC_base_cmp()
    {
        typedef base_int_item< cds::gc::HRC > item ;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list    ;
        test_int<list>()    ;
    }
    void IntrusiveLazyListHeaderTest::HRC_base_less()
    {
        typedef base_int_item< cds::gc::HRC > item ;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list    ;
        test_int<list>()    ;
    }
    void IntrusiveLazyListHeaderTest::HRC_base_cmpmix()
    {
        typedef base_int_item< cds::gc::HRC > item ;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list    ;
        test_int<list>()    ;
    }
    void IntrusiveLazyListHeaderTest::HRC_base_ic()
    {
        typedef base_int_item< cds::gc::HRC > item ;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list    ;
        test_int<list>()    ;
    }

}
