/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef _CDSUNIT_MAP2_MAP_TYPES_H
#define _CDSUNIT_MAP2_MAP_TYPES_H

#include <cds/urcu/general_instant.h>
#include <cds/urcu/general_buffered.h>
#include <cds/urcu/general_threaded.h>
#include <cds/urcu/signal_buffered.h>
#include <cds/urcu/signal_threaded.h>

#include <cds/container/michael_kvlist_hp.h>
#include <cds/container/michael_kvlist_hrc.h>
#include <cds/container/michael_kvlist_ptb.h>
#include <cds/container/michael_kvlist_rcu.h>
#include <cds/container/michael_kvlist_nogc.h>

#include <cds/container/lazy_kvlist_hp.h>
#include <cds/container/lazy_kvlist_hrc.h>
#include <cds/container/lazy_kvlist_ptb.h>
#include <cds/container/lazy_kvlist_rcu.h>
#include <cds/container/lazy_kvlist_nogc.h>

#include <cds/container/michael_map.h>
#include <cds/container/michael_map_rcu.h>
#include <cds/container/michael_map_nogc.h>

#include <cds/container/split_list_map.h>
#include <cds/container/split_list_map_rcu.h>
#include <cds/container/split_list_map_nogc.h>

#include <cds/container/striped_map/std_list.h>
#include <cds/container/striped_map/std_map.h>
#include <cds/container/striped_map/std_hash_map.h>
#include <cds/container/cuckoo_map.h>

#include <cds/container/skip_list_map_hp.h>
#include <cds/container/skip_list_map_hrc.h>
#include <cds/container/skip_list_map_ptb.h>
#include <cds/container/skip_list_map_rcu.h>
#include <cds/container/skip_list_map_nogc.h>

#include <boost/version.hpp>
#if BOOST_VERSION >= 104800
#   include <cds/container/striped_map/boost_list.h>
#   include <cds/container/striped_map/boost_slist.h>
#   include <cds/container/striped_map/boost_map.h>
#   include <cds/container/striped_map/boost_flat_map.h>
#endif
#include <cds/container/striped_map/boost_unordered_map.h>
#include <cds/container/striped_map.h>

#include <cds/lock/spinlock.h>

#include "cppunit/cppunit_mini.h"
#include "lock/nolock.h"
#include "map2/std_map.h"
#include "map2/std_hash_map.h"
#include "michael_alloc.h"
#include "print_cuckoo_stat.h"
#include "print_skip_list_stat.h"

namespace map2 {
    namespace cc = cds::container   ;
    namespace co = cds::opt         ;

    typedef cds::urcu::gc< cds::urcu::general_instant<> >   rcu_gpi ;
    typedef cds::urcu::gc< cds::urcu::general_buffered<> >  rcu_gpb ;
    typedef cds::urcu::gc< cds::urcu::general_threaded<> >  rcu_gpt ;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    typedef cds::urcu::gc< cds::urcu::signal_buffered<> >  rcu_shb ;
    typedef cds::urcu::gc< cds::urcu::signal_threaded<> >  rcu_sht ;
#endif

    template <typename Key>
    struct cmp {
        int operator ()(Key const& k1, Key const& k2) const
        {
            if ( std::less<Key>( k1, k2 ) )
                return -1   ;
            return std::less<Key>( k2, k1 ) ? 1 : 0  ;
        }
    };

#define CDSUNIT_INT_COMPARE(t)  template <> struct cmp<t> { int operator()( t k1, t k2 ){ return (int)(k1 - k2); } }
    CDSUNIT_INT_COMPARE(char)   ;
    CDSUNIT_INT_COMPARE(unsigned char)  ;
    CDSUNIT_INT_COMPARE(int)  ;
    CDSUNIT_INT_COMPARE(unsigned int)  ;
    CDSUNIT_INT_COMPARE(long)  ;
    CDSUNIT_INT_COMPARE(unsigned long)  ;
    CDSUNIT_INT_COMPARE(long long)  ;
    CDSUNIT_INT_COMPARE(unsigned long long)  ;
#undef CDSUNIT_INT_COMPARE

    template <>
    struct cmp<std::string>
    {
        int operator()(std::string const& s1, std::string const& s2)
        {
            return s1.compare( s2 ) ;
        }
        int operator()(std::string const& s1, char const * s2)
        {
            return s1.compare( s2 ) ;
        }
        int operator()(char const * s1, std::string const& s2)
        {
            return -s2.compare( s1 ) ;
        }
    };

    template <typename K, typename V, CDS_DECL_OPTIONS10>
    class CuckooStripedMap:
        public cc::CuckooMap< K, V,
            typename cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,CDS_OPTIONS10
            >::type
        >
    {
    public:
        typedef typename cc::cuckoo::make_traits<
            co::mutex_policy< cc::cuckoo::striping<> >
            ,CDS_OPTIONS10
        >::type cuckoo_traits ;

        typedef cc::CuckooMap< K, V, cuckoo_traits > cuckoo_base_class   ;

    public:
        CuckooStripedMap( size_t nCapacity, size_t nLoadFactor )
            : cuckoo_base_class( nCapacity / (nLoadFactor * 16), (unsigned int) 4 )
        {}
    };

    template <typename K, typename V, CDS_DECL_OPTIONS10>
    class CuckooRefinableMap:
        public cc::CuckooMap< K, V,
            typename cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,CDS_OPTIONS10
            >::type
        >
    {
    public:
        typedef typename cc::cuckoo::make_traits<
            co::mutex_policy< cc::cuckoo::refinable<> >
            ,CDS_OPTIONS10
        >::type cuckoo_traits ;

        typedef cc::CuckooMap< K, V, cuckoo_traits > cuckoo_base_class   ;

    public:
        CuckooRefinableMap( size_t nCapacity, size_t nLoadFactor )
            : cuckoo_base_class( nCapacity / (nLoadFactor * 16), (unsigned int) 4 )
        {}
    };

    template <typename Key, typename Value>
    struct MapTypes {
        typedef co::v::hash<Key>    hash    ;
        typedef std::less<Key>      less    ;
        typedef cmp<Key>            compare ;

        struct equal_to {
            bool operator()( Key const& k1, Key const& k2 ) const
            {
                return compare()( k1, k2 ) == 0 ;
            }
        };

        struct hash2: public hash
        {
            size_t operator()( Key const& k ) const
            {
                size_t seed = ~hash::operator ()( k ) ;
                boost::hash_combine( seed, k );
                return seed     ;
            }
        };

        // ***************************************************************************
        // MichaelKVList

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_HP_cmp_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HP_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HP_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_HP_less_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HP_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::HP, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HP_less_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_HRC_cmp_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HRC_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HRC_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_HRC_less_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HRC_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::HRC, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HRC_less_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_PTB_cmp_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_PTB_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_PTB_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_PTB_less_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_PTB_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::PTB, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_PTB_less_michaelAlloc ;

        // RCU
        typedef cc::MichaelKVList< rcu_gpi, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_GPI_cmp_stdAlloc ;

        typedef cc::MichaelKVList< rcu_gpi, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPI_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_gpi, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPI_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< rcu_gpi, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_GPI_less_stdAlloc ;

        typedef cc::MichaelKVList< rcu_gpi, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPI_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_gpi, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPI_less_michaelAlloc ;

        //
        typedef cc::MichaelKVList< rcu_gpb, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_GPB_cmp_stdAlloc ;

        typedef cc::MichaelKVList< rcu_gpb, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPB_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_gpb, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPB_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< rcu_gpb, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_GPB_less_stdAlloc ;

        typedef cc::MichaelKVList< rcu_gpb, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPB_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_gpb, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPB_less_michaelAlloc ;

        //
        typedef cc::MichaelKVList< rcu_gpt, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_GPT_cmp_stdAlloc ;

        typedef cc::MichaelKVList< rcu_gpt, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPT_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_gpt, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPT_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< rcu_gpt, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_GPT_less_stdAlloc ;

        typedef cc::MichaelKVList< rcu_gpt, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPT_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_gpt, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPT_less_michaelAlloc ;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_SHB_cmp_stdAlloc ;

        typedef cc::MichaelKVList< rcu_shb, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_SHB_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_shb, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_SHB_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< rcu_shb, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_SHB_less_stdAlloc ;

        typedef cc::MichaelKVList< rcu_shb, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_SHB_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_shb, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_SHB_less_michaelAlloc ;

        //
        typedef cc::MichaelKVList< rcu_sht, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_SHT_cmp_stdAlloc ;

        typedef cc::MichaelKVList< rcu_sht, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_SHT_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_sht, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_SHT_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< rcu_sht, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_SHT_less_stdAlloc ;

        typedef cc::MichaelKVList< rcu_sht, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_SHT_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< rcu_sht, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_SHT_less_michaelAlloc ;
#endif

        // gc::nogc
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_NOGC_cmp_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_NOGC_cmp_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_NOGC_cmp_michaelAlloc ;

        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_NOGC_less_stdAlloc ;

        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_NOGC_less_stdAlloc_seqcst ;

        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_NOGC_less_michaelAlloc ;

        template <typename Base>
        class NogcMapWrapper: public Base
        {
            typedef Base    base_class ;
        public:
            NogcMapWrapper( size_t nMaxItemCount, size_t nLoadFactor )
                : base_class( nMaxItemCount, nLoadFactor )
            {}

            template <typename K>
            bool insert( const K& key )
            {
                return base_class::insert( key ) != base_class::end() ;
            }

            template <typename K, typename V>
            bool insert( K const& key, V const& val )
            {
                return base_class::insert( key, val ) != base_class::end() ;
            }

            template <typename K, typename Func>
            bool insert_key( const K& key, Func func )
            {
                return base_class::insert_key( key, func ) != base_class::end() ;
            }

            template <typename K>
            bool find( const K& key )
            {
                return base_class::find( key ) != base_class::end() ;
            }
        };

        template <typename Base>
        class NogcMapWrapper_dctor: public Base
        {
            typedef Base    base_class ;
        public:
            NogcMapWrapper_dctor()
            {}

            template <typename K>
            bool insert( const K& key )
            {
                return base_class::insert( key ) != base_class::end() ;
            }

            template <typename K, typename V>
            bool insert( K const& key, V const& val )
            {
                return base_class::insert( key, val ) != base_class::end() ;
            }

            template <typename K, typename Func>
            bool insert_key( const K& key, Func func )
            {
                return base_class::insert_key( key, func ) != base_class::end() ;
            }

            template <typename K>
            bool find( const K& key )
            {
                return base_class::find( key ) != base_class::end() ;
            }
        };

        // ***************************************************************************
        // MichaelHashMap based on MichaelKVList

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HP_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HP_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_HP_cmp_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HP_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HP_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_HP_less_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HRC_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HRC_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_HRC_cmp_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HRC_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_HRC_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, MichaelList_HRC_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_HRC_less_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_PTB_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_PTB_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_PTB_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_PTB_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_PTB_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, MichaelList_PTB_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_PTB_less_michaelAlloc  ;

        //RCU
        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPI_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPI_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_GPI_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPI_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPI_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_GPI_less_michaelAlloc  ;

        //
        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPB_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPB_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_GPB_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPB_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPB_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_GPB_less_michaelAlloc  ;

        //
        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPT_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPT_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_GPT_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPT_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_GPT_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_GPT_less_michaelAlloc  ;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_SHB_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_SHB_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_SHB_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_SHB_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_SHB_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_SHB_less_michaelAlloc  ;

        //
        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_SHT_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_SHT_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_SHT_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_SHT_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_RCU_SHT_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_RCU_SHT_less_michaelAlloc  ;
#endif

        // gc::nogc
        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        > >  MichaelMap_NOGC_cmp_stdAlloc  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        > >   MichaelMap_NOGC_cmp_stdAlloc_seqcst  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        > >  MichaelMap_NOGC_cmp_michaelAlloc  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        > >   MichaelMap_NOGC_less_stdAlloc  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        > >   MichaelMap_NOGC_less_stdAlloc_seqcst  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        > >  MichaelMap_NOGC_less_michaelAlloc  ;


        // ***************************************************************************
        // LazyKVList

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_HP_cmp_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HP_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HP_cmp_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_HP_less_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HP_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::HP, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HP_less_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_HRC_cmp_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HRC_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HRC_cmp_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_HRC_less_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HRC_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::HRC, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HRC_less_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_PTB_cmp_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_PTB_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_PTB_cmp_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_PTB_less_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_PTB_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::PTB, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_PTB_less_michaelAlloc ;

        // RCU
        typedef cc::LazyKVList< rcu_gpi, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_GPI_cmp_stdAlloc ;

        typedef cc::LazyKVList< rcu_gpi, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPI_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_gpi, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPI_cmp_michaelAlloc ;

        typedef cc::LazyKVList< rcu_gpi, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_GPI_less_stdAlloc ;

        typedef cc::LazyKVList< rcu_gpi, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPI_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_gpi, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPI_less_michaelAlloc ;

        //
        typedef cc::LazyKVList< rcu_gpb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_GPB_cmp_stdAlloc ;

        typedef cc::LazyKVList< rcu_gpb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPB_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_gpb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPB_cmp_michaelAlloc ;

        typedef cc::LazyKVList< rcu_gpb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_GPB_less_stdAlloc ;

        typedef cc::LazyKVList< rcu_gpb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPB_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_gpb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPB_less_michaelAlloc ;

        //
        typedef cc::LazyKVList< rcu_gpt, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_GPT_cmp_stdAlloc ;

        typedef cc::LazyKVList< rcu_gpt, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPT_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_gpt, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPT_cmp_michaelAlloc ;

        typedef cc::LazyKVList< rcu_gpt, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_GPT_less_stdAlloc ;

        typedef cc::LazyKVList< rcu_gpt, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPT_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_gpt, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPT_less_michaelAlloc ;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_SHB_cmp_stdAlloc ;

        typedef cc::LazyKVList< rcu_shb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_SHB_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_shb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_SHB_cmp_michaelAlloc ;

        typedef cc::LazyKVList< rcu_shb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_SHB_less_stdAlloc ;

        typedef cc::LazyKVList< rcu_shb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_SHB_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_shb, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_SHB_less_michaelAlloc ;

        //
        typedef cc::LazyKVList< rcu_sht, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_SHT_cmp_stdAlloc ;

        typedef cc::LazyKVList< rcu_sht, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_SHT_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_sht, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_SHT_cmp_michaelAlloc ;

        typedef cc::LazyKVList< rcu_sht, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_SHT_less_stdAlloc ;

        typedef cc::LazyKVList< rcu_sht, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_SHT_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< rcu_sht, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_SHT_less_michaelAlloc ;
#endif

        // gc::nogc
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_NOGC_cmp_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::nogc, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_NOGC_cmp_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::nogc, Key, Value,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_NOGC_cmp_michaelAlloc ;

        typedef cc::LazyKVList< cds::gc::nogc, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_NOGC_less_stdAlloc ;

        typedef cc::LazyKVList< cds::gc::nogc, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_NOGC_less_stdAlloc_seqcst ;

        typedef cc::LazyKVList< cds::gc::nogc, Key, Value,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_NOGC_less_michaelAlloc ;


        // ***************************************************************************
        // MichaelHashMap based on LazyKVList

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HP_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HP_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_HP_cmp_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HP_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HP_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_HP_less_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HRC_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HRC_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_HRC_cmp_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HRC_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_HRC_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::HRC, LazyList_HRC_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_HRC_less_michaelAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_PTB_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_PTB_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_PTB_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_PTB_less_stdAlloc  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_PTB_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< cds::gc::PTB, LazyList_PTB_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_PTB_less_michaelAlloc  ;

        // RCU
        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_GPI_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPI_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPI_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_GPI_less_michaelAlloc  ;

        //
        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_GPB_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPB_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPB_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_GPB_less_michaelAlloc  ;

        //
        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_GPT_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPT_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_GPT_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_GPT_less_michaelAlloc  ;


#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_SHB_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_SHB_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_SHB_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_SHB_less_michaelAlloc  ;

        //
        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_SHT_cmp_michaelAlloc ;

        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_SHT_less_stdAlloc  ;

        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        >   MichaelMap_Lazy_RCU_SHT_less_stdAlloc_seqcst  ;

        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelMap_Lazy_RCU_SHT_less_michaelAlloc  ;
#endif

        // gc::nogc
        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_cmp_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        > >   MichaelMap_Lazy_NOGC_cmp_stdAlloc  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_cmp_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        > >   MichaelMap_Lazy_NOGC_cmp_stdAlloc_seqcst  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_cmp_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        > >  MichaelMap_Lazy_NOGC_cmp_michaelAlloc  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_less_stdAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        > >  MichaelMap_Lazy_NOGC_less_stdAlloc  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_less_stdAlloc_seqcst,
            typename cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        > >  MichaelMap_Lazy_NOGC_less_stdAlloc_seqcst  ;

        typedef NogcMapWrapper< cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_less_michaelAlloc,
            typename cc::michael_map::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        > >  MichaelMap_Lazy_NOGC_less_michaelAlloc  ;


        // ***************************************************************************
        // SplitListMap based on MichaelList

        // HP
        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_cmp ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_cmp_seqcst ;

        //HP + less
        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_less ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_less ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_less_seqcst ;

        // HRC
        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_st_cmp ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_st_cmp_seqcst ;

        // HRC + less
        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_dyn_less ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_st_less ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_HRC_st_less_seqcst ;

        // PTB
        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_st_cmp ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_st_cmp_seqcst ;

        // PTB + less
        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_dyn_less ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_st_less ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_PTB_st_less_seqcst ;

        // RCU
        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_dyn_cmp ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_st_cmp ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_st_cmp_seqcst ;

        // RCU_GPI + less
        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_dyn_less ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_st_less ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_st_less_seqcst ;

        //
        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_dyn_cmp ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_st_cmp ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_st_cmp_seqcst ;

        // RCU_GPB + less
        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_dyn_less ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_st_less ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_st_less_seqcst ;

        //
        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_dyn_cmp ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_st_cmp ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_st_cmp_seqcst ;

        // RCU_GPT + less
        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_dyn_less ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_st_less ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_st_less_seqcst ;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_dyn_cmp ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_st_cmp ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_st_cmp_seqcst ;

        // RCU_SHB + less
        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_dyn_less ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_st_less ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_st_less_seqcst ;

        //
        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_dyn_cmp ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_st_cmp ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_st_cmp_seqcst ;

        // RCU_SHT + less
        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_dyn_less ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_st_less ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_st_less_seqcst ;
#endif

        // gc::nogc
        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > > SplitList_Michael_NOGC_dyn_cmp ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > > SplitList_Michael_NOGC_dyn_cmp_seqcst ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > > SplitList_Michael_NOGC_st_cmp ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > > SplitList_Michael_NOGC_st_cmp_seqcst ;

        //NOGC + less
        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > > SplitList_Michael_NOGC_dyn_less ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > > SplitList_Michael_NOGC_dyn_less_seqcst ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > > SplitList_Michael_NOGC_st_less ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > > SplitList_Michael_NOGC_st_less_seqcst ;


        // ***************************************************************************
        // SplitListMap based on LazyKVList

        // HP
        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_cmp ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_cmp_seqcst ;


        // HP + less
        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_less ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_less ;

        typedef cc::SplitListMap< cds::gc::HP, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_less_seqcst ;

        // HRC
        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_st_cmp ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_st_cmp_seqcst ;

        // HRC + less
        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_dyn_less ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_st_less ;

        typedef cc::SplitListMap< cds::gc::HRC, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_HRC_st_less_seqcst ;


        // PTB
        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_dyn_cmp ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_st_cmp ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_st_cmp_seqcst ;

        // PTB + less
        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_dyn_less ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_dyn_less_seqcst ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_st_less ;

        typedef cc::SplitListMap< cds::gc::PTB, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_PTB_st_less_seqcst ;

        // RCU
        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_dyn_cmp ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_st_cmp ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_st_cmp_seqcst ;

        // RCU_GPI + less
        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_dyn_less ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_st_less ;

        typedef cc::SplitListMap< rcu_gpi, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_st_less_seqcst ;

        //
        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_dyn_cmp ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_st_cmp ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_st_cmp_seqcst ;

        // RCU_GPB + less
        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_dyn_less ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_st_less ;

        typedef cc::SplitListMap< rcu_gpb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_st_less_seqcst ;

        //
        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_dyn_cmp ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_st_cmp ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_st_cmp_seqcst ;

        // RCU_GPT + less
        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_dyn_less ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_st_less ;

        typedef cc::SplitListMap< rcu_gpt, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_st_less_seqcst ;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_dyn_cmp ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_st_cmp ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_st_cmp_seqcst ;

        // RCU_SHB + less
        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_dyn_less ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_st_less ;

        typedef cc::SplitListMap< rcu_shb, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_st_less_seqcst ;

        //
        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_dyn_cmp ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_dyn_cmp_seqcst ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_st_cmp ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_st_cmp_seqcst ;

        // RCU_SHT + less
        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_dyn_less ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_dyn_less_seqcst ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_st_less ;

        typedef cc::SplitListMap< rcu_sht, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_st_less_seqcst ;
#endif

        // gc::nogc
        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > > SplitList_Lazy_NOGC_dyn_cmp ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > > SplitList_Lazy_NOGC_dyn_cmp_seqcst ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > > SplitList_Lazy_NOGC_st_cmp ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > > SplitList_Lazy_NOGC_st_cmp_seqcst ;


        // NOGC + less
        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > > SplitList_Lazy_NOGC_dyn_less ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > > SplitList_Lazy_NOGC_dyn_less_seqcst ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > > SplitList_Lazy_NOGC_st_less ;

        typedef NogcMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        > > SplitList_Lazy_NOGC_st_less_seqcst ;


        // ***************************************************************************
        // StripedHashMap

        // for sequential containers
        template <class BucketEntry, CDS_DECL_OPTIONS7>
        class StripedHashMap_seq:
            public cc::StripedMap< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , CDS_OPTIONS7
            >
        {
            typedef cc::StripedMap< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , CDS_OPTIONS7
            > base_class   ;
            typedef typename base_class::resizing_policy resizing_policy_t ;

            resizing_policy_t   m_placeHolder ;
        public:
            StripedHashMap_seq( size_t nCapacity, size_t nLoadFactor )
                : base_class( nCapacity / nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor )) )
            {}
        };

        // for non-sequential ordered containers
        template <class BucketEntry, CDS_DECL_OPTIONS7>
        class StripedHashMap_ord:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , CDS_OPTIONS7
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , CDS_OPTIONS7
            > base_class   ;
            typedef typename base_class::resizing_policy resizing_policy_t ;

            resizing_policy_t   m_placeHolder ;
        public:
            StripedHashMap_ord( size_t nCapacity, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}
        };


        typedef StripedHashMap_seq<
            std::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_list   ;

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION < 1600
        typedef StripedHashMap_ord<
            stdext::hash_map< Key, Value, stdext::hash_compare<Key, less > >
            , co::hash< hash2 >
        > StripedMap_hashmap    ;
#else
        typedef StripedHashMap_ord<
            std::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > StripedMap_hashmap    ;
#endif

        typedef StripedHashMap_ord<
            std::map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_map   ;

        typedef StripedHashMap_ord<
            boost::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > StripedMap_boost_unordered_map    ;

#if BOOST_VERSION >= 104800
        typedef StripedHashMap_seq<
            boost::container::slist< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_slist   ;

        typedef StripedHashMap_seq<
            boost::container::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_boost_list   ;

        typedef StripedHashMap_ord<
            boost::container::map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_boost_map   ;

//#   ifdef CDS_UNIT_MAP_TYPES_ENABLE_BOOST_FLAT_CONTAINERS
        typedef StripedHashMap_ord<
            boost::container::flat_map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_boost_flat_map   ;
//#   endif
#endif  // BOOST_VERSION >= 104800

        // ***************************************************************************
        // RefinableHashMap

        // for sequential containers
        template <class BucketEntry, CDS_DECL_OPTIONS7>
        class RefinableHashMap_seq:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , CDS_OPTIONS7
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , CDS_OPTIONS7
            > base_class   ;
            typedef typename base_class::resizing_policy resizing_policy_t ;

            resizing_policy_t   m_placeHolder ;
        public:
            RefinableHashMap_seq( size_t nCapacity, size_t nLoadFactor )
                : base_class( nCapacity / nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor )) )
            {}
        };

        // for non-sequential ordered containers
        template <class BucketEntry, CDS_DECL_OPTIONS7>
        class RefinableHashMap_ord:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , CDS_OPTIONS7
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , CDS_OPTIONS7
            > base_class   ;
            typedef typename base_class::resizing_policy resizing_policy_t ;

            resizing_policy_t   m_placeHolder ;
        public:
            RefinableHashMap_ord( size_t nCapacity, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}
        };


        typedef RefinableHashMap_seq<
            std::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_list   ;

#if BOOST_VERSION >= 104800
        typedef RefinableHashMap_seq<
            boost::container::slist< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_slist   ;
#endif

        typedef RefinableHashMap_ord<
            std::map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_map   ;

#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION < 1600
        typedef RefinableHashMap_ord<
            stdext::hash_map< Key, Value, stdext::hash_compare<Key, less > >
            , co::hash< hash2 >
        > RefinableMap_hashmap    ;
#else
        typedef RefinableHashMap_ord<
            std::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > RefinableMap_hashmap    ;
#endif
        typedef RefinableHashMap_ord<
            boost::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > RefinableMap_boost_unordered_map    ;

#if BOOST_VERSION >= 104800
        typedef RefinableHashMap_seq<
            boost::container::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_boost_list   ;

        typedef RefinableHashMap_ord<
            boost::container::map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_boost_map   ;

//#   ifdef CDS_UNIT_MAP_TYPES_ENABLE_BOOST_FLAT_CONTAINERS
        typedef RefinableHashMap_ord<
            boost::container::flat_map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_boost_flat_map   ;
//#   endif
#endif // #if BOOST_VERSION >= 104800


        // ***************************************************************************
        // CuckooMap

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooStripedMap_list_unord ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooStripedMap_list_unord_stat ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooStripedMap_list_unord_storehash ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooStripedMap_list_ord ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooStripedMap_list_ord_stat ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooStripedMap_list_ord_storehash ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooStripedMap_vector_unord ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooStripedMap_vector_unord_stat ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooStripedMap_vector_unord_storehash ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooStripedMap_vector_ord ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooStripedMap_vector_ord_stat ;

        typedef CuckooStripedMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooStripedMap_vector_ord_storehash ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooRefinableMap_list_unord ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooRefinableMap_list_unord_stat ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooRefinableMap_list_unord_storehash ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooRefinableMap_list_ord ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooRefinableMap_list_ord_stat ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooRefinableMap_list_ord_storehash ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooRefinableMap_vector_unord ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooRefinableMap_vector_unord_stat ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooRefinableMap_vector_unord_storehash ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooRefinableMap_vector_ord ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooRefinableMap_vector_ord_stat ;

        typedef CuckooRefinableMap< Key, Value,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooRefinableMap_vector_ord_storehash ;

        // ***************************************************************************
        // SkipListMap - HP

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_less_pascal ;

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_less_pascal_seqcst ;

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_less_pascal_stat ;

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_cmp_pascal ;

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_cmp_pascal_stat ;

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_less_xorshift ;

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_less_xorshift_stat ;

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_cmp_xorshift ;

        typedef cc::SkipListMap< cds::gc::HP, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hp_cmp_xorshift_stat ;

        // ***************************************************************************
        // SkipListMap - HRC

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_less_pascal ;

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_less_pascal_seqcst ;

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_less_pascal_stat ;

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_cmp_pascal ;

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_cmp_pascal_stat ;

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_less_xorshift ;

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_less_xorshift_stat ;

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_cmp_xorshift ;

        typedef cc::SkipListMap< cds::gc::HRC, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_hrc_cmp_xorshift_stat ;

        // ***************************************************************************
        // SkipListMap - PTB

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_ptb_less_pascal ;

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_ptb_less_pascal_seqcst ;

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_ptb_less_pascal_stat ;

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_ptb_cmp_pascal ;

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_ptb_cmp_pascal_stat ;

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_ptb_less_xorshift ;

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_ptb_less_xorshift_stat ;

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > SkipListMap_ptb_cmp_xorshift ;

        typedef cc::SkipListMap< cds::gc::PTB, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_ptb_cmp_xorshift_stat ;

        // ***************************************************************************
        // SkipListMap< gc::nogc >

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > > SkipListMap_nogc_less_pascal ;

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > > SkipListMap_nogc_less_pascal_seqcst ;

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > > SkipListMap_nogc_less_pascal_stat ;

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > > SkipListMap_nogc_cmp_pascal ;

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::skip_list::stat >
            >::type
        > > SkipListMap_nogc_cmp_pascal_stat ;

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > > SkipListMap_nogc_less_xorshift ;

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > > SkipListMap_nogc_less_xorshift_stat ;

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > > SkipListMap_nogc_cmp_xorshift ;

        typedef NogcMapWrapper_dctor< cc::SkipListMap< cds::gc::nogc, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
            >::type
        > > SkipListMap_nogc_cmp_xorshift_stat ;

        // ***************************************************************************
        // SkipListMap - RCU general_instant

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_gpi_less_pascal ;

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpi_less_pascal_seqcst ;

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpi_less_pascal_stat ;

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_gpi_cmp_pascal ;

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_gpi_cmp_pascal_stat ;

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpi_less_xorshift ;

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpi_less_xorshift_stat ;

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > SkipListMap_rcu_gpi_cmp_xorshift ;

        typedef cc::SkipListMap< rcu_gpi, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_gpi_cmp_xorshift_stat ;

        // ***************************************************************************
        // SkipListMap - RCU general_buffered

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_gpb_less_pascal ;

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpb_less_pascal_seqcst ;

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpb_less_pascal_stat ;

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_gpb_cmp_pascal ;

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_gpb_cmp_pascal_stat ;

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpb_less_xorshift ;

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpb_less_xorshift_stat ;

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > SkipListMap_rcu_gpb_cmp_xorshift ;

        typedef cc::SkipListMap< rcu_gpb, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_gpb_cmp_xorshift_stat ;

        // ***************************************************************************
        // SkipListMap - RCU general_threaded

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_gpt_less_pascal ;

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpt_less_pascal_seqcst ;

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpt_less_pascal_stat ;

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_gpt_cmp_pascal ;

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_gpt_cmp_pascal_stat ;

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpt_less_xorshift ;

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_gpt_less_xorshift_stat ;

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > SkipListMap_rcu_gpt_cmp_xorshift ;

        typedef cc::SkipListMap< rcu_gpt, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_gpt_cmp_xorshift_stat ;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // ***************************************************************************
        // SkipListMap - RCU signal_buffered

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_shb_less_pascal ;

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_shb_less_pascal_seqcst ;

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_shb_less_pascal_stat ;

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_shb_cmp_pascal ;

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_shb_cmp_pascal_stat ;

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_shb_less_xorshift ;

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_shb_less_xorshift_stat ;

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > SkipListMap_rcu_shb_cmp_xorshift ;

        typedef cc::SkipListMap< rcu_shb, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_shb_cmp_xorshift_stat ;

        // ***************************************************************************
        // SkipListMap - RCU signal_threaded

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_sht_less_pascal ;

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_sht_less_pascal_seqcst ;

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_sht_less_pascal_stat ;

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > SkipListMap_rcu_sht_cmp_pascal ;

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_sht_cmp_pascal_stat ;

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_sht_less_xorshift ;

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > SkipListMap_rcu_sht_less_xorshift_stat ;

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > SkipListMap_rcu_sht_cmp_xorshift ;

        typedef cc::SkipListMap< rcu_sht, Key, Value,
            typename cc::skip_list::make_traits <
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat >
            >::type
        > SkipListMap_rcu_sht_cmp_xorshift_stat ;
#endif

        // ***************************************************************************
        // Standard implementations

        typedef StdMap< Key, Value, cds::SpinLock >             StdMap_Spin             ;
        typedef StdMap< Key, Value, lock::NoLock>               StdMap_NoLock           ;

        typedef StdHashMap< Key, Value, cds::SpinLock >         StdHashMap_Spin         ;
        typedef StdHashMap< Key, Value, lock::NoLock >          StdHashMap_NoLock       ;

    };

    template <typename Map>
    static inline void print_stat( Map const& m )
    {}

    template <typename K, typename T, typename Traits >
    static inline void print_stat( cc::CuckooMap< K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() << m.mutex_policy_statistics() ) ;
    }

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( cc::SkipListMap< GC, K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() ) ;
    }

    template <typename K, typename V, CDS_SPEC_OPTIONS10>
    static inline void print_stat( CuckooStripedMap< K, V, CDS_OPTIONS10 > const& m )
    {
        typedef CuckooStripedMap< K, V, CDS_OPTIONS10 > map_type ;
        print_stat( static_cast<typename map_type::cuckoo_base_class const&>(m) )   ;
    }

    template <typename K, typename V, CDS_SPEC_OPTIONS10>
    static inline void print_stat( CuckooRefinableMap< K, V, CDS_OPTIONS10 > const& m )
    {
        typedef CuckooRefinableMap< K, V, CDS_OPTIONS10 > map_type ;
        print_stat( static_cast<typename map_type::cuckoo_base_class const&>(m) )   ;
    }

}   // namespace map2

#endif // ifndef _CDSUNIT_MAP2_MAP_TYPES_H
