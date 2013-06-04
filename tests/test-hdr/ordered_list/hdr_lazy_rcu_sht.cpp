/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#include "ordered_list/hdr_lazy.h"
#include <cds/urcu/signal_threaded.h>
#include <cds/container/lazy_list_rcu.h>

namespace ordlist {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_threaded<> >    rcu_type ;

        struct RCU_SHT_cmp_traits: public cc::lazy_list::type_traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare ;
        };
    }
#endif
    void LazyListTestHeader::RCU_SHT_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_SHT_cmp_traits > list   ;
        test< list >()  ;

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list   ;
        test< opt_list >()  ;
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHT_less_traits: public cc::lazy_list::type_traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less ;
        };
    }
#endif
    void LazyListTestHeader::RCU_SHT_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_SHT_less_traits > list   ;
        test< list >()  ;

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list   ;
        test< opt_list >()  ;
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHT_cmpmix_traits: public cc::lazy_list::type_traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare ;
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>  less    ;
        };
    }
#endif
    void LazyListTestHeader::RCU_SHT_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_SHT_cmpmix_traits > list   ;
        test< list >()  ;

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list   ;
        test< opt_list >()  ;
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHT_ic_traits: public cc::lazy_list::type_traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less ;
            typedef cds::atomicity::item_counter item_counter   ;
        };
    }
#endif
    void LazyListTestHeader::RCU_SHT_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_SHT_ic_traits > list   ;
        test< list >()  ;

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list   ;
        test< opt_list >()  ;
#endif
    }

}   // namespace ordlist

