/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#include "set/hdr_intrusive_set.h"

namespace set {

    class IntrusiveSkipListSetRCU: public IntrusiveHashSetHdrTest
    {
        typedef IntrusiveHashSetHdrTest base_class ;

        template <typename Set>
        struct extract_disposer {
            void operator()( typename Set::value_type * pVal ) const
            {
                pVal->nVal = 0 ;
            }
        };

    protected:

        template <class Set, typename PrintStat>
        void test_skiplist()
        {
            {
                Set s ;
                base_class::test_int_with( s )  ;
            }

            test_skiplist_<Set, PrintStat >() ;
        }

        template <class Set, typename PrintStat>
        void test_skiplist_()
        {
            Set s ;
            s.clear() ;
            CPPUNIT_ASSERT( s.empty() ) ;
            CPPUNIT_ASSERT( check_size( s, 0 )) ;

            typedef typename Set::gc::scoped_lock   rcu_lock ;
            typedef typename Set::value_type        value_type  ;
            typedef typename Set::iterator          set_iterator ;
            typedef typename Set::const_iterator    const_set_iterator ;

            value_type  v[1000]  ;
            int nCount = 0  ;
            int nPrevKey = 0;

            // Test iterator - ascending order
            for ( int i = 0; i < (int) (sizeof(v)/sizeof(v[0])); ++i ) {
                v[i].nKey = i       ;
                v[i].nVal = i * 2   ;

                CPPUNIT_ASSERT( s.insert( v[i] ))   ;
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;

            CPPUNIT_MSG( PrintStat()(s, "Iterator test, ascending insert order") ) ;

            nCount = 0  ;
            nPrevKey = 0;
            {
                rcu_lock l ;
                for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal )    ;
                    CPPUNIT_ASSERT( s.find( it->nKey )) ;
                    it->nVal = (*it).nKey   ;
                    ++nCount    ;
                    if ( it != s.begin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey ) ;
                    }
                    nPrevKey = it->nKey ;
                }
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0])) ;

            nCount = 0  ;
            {
                rcu_lock l ;
                for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal )    ;
                    ++nCount    ;
                    if ( it != s.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey ) ;
                    }
                    nPrevKey = it->nKey ;
                }
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0])) ;

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nKey == v[i].nVal )   ;
                CPPUNIT_ASSERT( s.find( v[i].nKey )) ;
            }

            s.clear() ;
            CPPUNIT_ASSERT( s.empty() ) ;
            CPPUNIT_ASSERT( check_size( s, 0)) ;
            Set::gc::force_dispose()  ;

            for ( size_t i = 0; i < (int) sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nDisposeCount == 1 )   ;
            }

            // Test iterator - descending order
            for ( int i = (int) sizeof(v)/sizeof(v[0]) - 1; i >= 0; --i ) {
                v[i].nKey = i       ;
                v[i].nVal = i * 2   ;

                CPPUNIT_ASSERT( s.insert( v[i] ))   ;
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;

            CPPUNIT_MSG( PrintStat()(s, "Iterator test, descending insert order") ) ;

            nCount = 0 ;
            {
                rcu_lock l ;
                for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal )    ;
                    it->nVal = (*it).nKey   ;
                    ++nCount    ;
                    if ( it != s.begin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey ) ;
                    }
                    nPrevKey = it->nKey ;
                }
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0])) ;

            nCount = 0  ;
            {
                rcu_lock l ;
                for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal )    ;
                    ++nCount    ;
                    if ( it != s.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey ) ;
                    }
                    nPrevKey = it->nKey ;
                }
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0])) ;

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nKey == v[i].nVal )   ;
            }

            s.clear() ;
            CPPUNIT_ASSERT( s.empty() ) ;
            CPPUNIT_ASSERT( check_size( s, 0 )) ;
            Set::gc::force_dispose()  ;

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nDisposeCount == 2 )   ;
            }

            // Test iterator - random order
            {
                int nRand[sizeof(v)/sizeof(v[0])] ;
                for ( int i = 0; i < (int) (sizeof(v)/sizeof(v[0])); ++i ) {
                    nRand[i] = i ;
                }
                std::random_shuffle( nRand, nRand + sizeof(v)/sizeof(v[0]) ) ;
                for ( int i = 0; i < (int)(sizeof(v)/sizeof(v[0])); ++i ) {
                    v[i].nKey = nRand[i]       ;
                    v[i].nVal = nRand[i] * 2   ;
                    CPPUNIT_ASSERT( s.insert( v[i] ))   ;
                }
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;
            CPPUNIT_MSG( PrintStat()(s, "Iterator test, random insert order") ) ;

            nCount = 0 ;
            {
                rcu_lock l ;
                for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal )    ;
                    it->nVal = (*it).nKey   ;
                    ++nCount    ;
                    if ( it != s.begin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey ) ;
                    }
                    nPrevKey = it->nKey ;
                }
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0])) ;

            nCount = 0  ;
            {
                rcu_lock l ;
                for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal )    ;
                    ++nCount    ;
                    if ( it != s.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey ) ;
                    }
                    nPrevKey = it->nKey ;
                }
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) )) ;
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0])) ;

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nKey == v[i].nVal )   ;
            }

            s.clear() ;
            CPPUNIT_ASSERT( s.empty() ) ;
            CPPUNIT_ASSERT( check_size( s, 0 )) ;
            Set::gc::force_dispose()  ;

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nDisposeCount == 3 )   ;
            }
        }


    public:

        // Skip-list RCU
        void skiplist_rcu_gpi_base_cmp() ;
        void skiplist_rcu_gpi_base_less() ;
        void skiplist_rcu_gpi_base_cmpmix() ;
        void skiplist_rcu_gpi_base_cmp_stat() ;
        void skiplist_rcu_gpi_base_less_stat() ;
        void skiplist_rcu_gpi_base_cmpmix_stat() ;
        void skiplist_rcu_gpi_base_cmp_xorshift() ;
        void skiplist_rcu_gpi_base_less_xorshift() ;
        void skiplist_rcu_gpi_base_cmpmix_xorshift() ;
        void skiplist_rcu_gpi_base_cmp_xorshift_stat() ;
        void skiplist_rcu_gpi_base_less_xorshift_stat() ;
        void skiplist_rcu_gpi_base_cmpmix_xorshift_stat() ;
        void skiplist_rcu_gpi_base_cmp_pascal() ;
        void skiplist_rcu_gpi_base_less_pascal() ;
        void skiplist_rcu_gpi_base_cmpmix_pascal() ;
        void skiplist_rcu_gpi_base_cmp_pascal_stat() ;
        void skiplist_rcu_gpi_base_less_pascal_stat() ;
        void skiplist_rcu_gpi_base_cmpmix_pascal_stat() ;

        void skiplist_rcu_gpi_member_cmp() ;
        void skiplist_rcu_gpi_member_less() ;
        void skiplist_rcu_gpi_member_cmpmix() ;
        void skiplist_rcu_gpi_member_cmp_stat() ;
        void skiplist_rcu_gpi_member_less_stat() ;
        void skiplist_rcu_gpi_member_cmpmix_stat() ;
        void skiplist_rcu_gpi_member_cmp_xorshift() ;
        void skiplist_rcu_gpi_member_less_xorshift() ;
        void skiplist_rcu_gpi_member_cmpmix_xorshift() ;
        void skiplist_rcu_gpi_member_cmp_xorshift_stat() ;
        void skiplist_rcu_gpi_member_less_xorshift_stat() ;
        void skiplist_rcu_gpi_member_cmpmix_xorshift_stat() ;
        void skiplist_rcu_gpi_member_cmp_pascal() ;
        void skiplist_rcu_gpi_member_less_pascal() ;
        void skiplist_rcu_gpi_member_cmpmix_pascal() ;
        void skiplist_rcu_gpi_member_cmp_pascal_stat() ;
        void skiplist_rcu_gpi_member_less_pascal_stat() ;
        void skiplist_rcu_gpi_member_cmpmix_pascal_stat() ;

        // general_buffered
        void skiplist_rcu_gpb_base_cmp() ;
        void skiplist_rcu_gpb_base_less() ;
        void skiplist_rcu_gpb_base_cmpmix() ;
        void skiplist_rcu_gpb_base_cmp_stat() ;
        void skiplist_rcu_gpb_base_less_stat() ;
        void skiplist_rcu_gpb_base_cmpmix_stat() ;
        void skiplist_rcu_gpb_base_cmp_xorshift() ;
        void skiplist_rcu_gpb_base_less_xorshift() ;
        void skiplist_rcu_gpb_base_cmpmix_xorshift() ;
        void skiplist_rcu_gpb_base_cmp_xorshift_stat() ;
        void skiplist_rcu_gpb_base_less_xorshift_stat() ;
        void skiplist_rcu_gpb_base_cmpmix_xorshift_stat() ;
        void skiplist_rcu_gpb_base_cmp_pascal() ;
        void skiplist_rcu_gpb_base_less_pascal() ;
        void skiplist_rcu_gpb_base_cmpmix_pascal() ;
        void skiplist_rcu_gpb_base_cmp_pascal_stat() ;
        void skiplist_rcu_gpb_base_less_pascal_stat() ;
        void skiplist_rcu_gpb_base_cmpmix_pascal_stat() ;

        void skiplist_rcu_gpb_member_cmp() ;
        void skiplist_rcu_gpb_member_less() ;
        void skiplist_rcu_gpb_member_cmpmix() ;
        void skiplist_rcu_gpb_member_cmp_stat() ;
        void skiplist_rcu_gpb_member_less_stat() ;
        void skiplist_rcu_gpb_member_cmpmix_stat() ;
        void skiplist_rcu_gpb_member_cmp_xorshift() ;
        void skiplist_rcu_gpb_member_less_xorshift() ;
        void skiplist_rcu_gpb_member_cmpmix_xorshift() ;
        void skiplist_rcu_gpb_member_cmp_xorshift_stat() ;
        void skiplist_rcu_gpb_member_less_xorshift_stat() ;
        void skiplist_rcu_gpb_member_cmpmix_xorshift_stat() ;
        void skiplist_rcu_gpb_member_cmp_pascal() ;
        void skiplist_rcu_gpb_member_less_pascal() ;
        void skiplist_rcu_gpb_member_cmpmix_pascal() ;
        void skiplist_rcu_gpb_member_cmp_pascal_stat() ;
        void skiplist_rcu_gpb_member_less_pascal_stat() ;
        void skiplist_rcu_gpb_member_cmpmix_pascal_stat() ;

        // general_threaded
        void skiplist_rcu_gpt_base_cmp() ;
        void skiplist_rcu_gpt_base_less() ;
        void skiplist_rcu_gpt_base_cmpmix() ;
        void skiplist_rcu_gpt_base_cmp_stat() ;
        void skiplist_rcu_gpt_base_less_stat() ;
        void skiplist_rcu_gpt_base_cmpmix_stat() ;
        void skiplist_rcu_gpt_base_cmp_xorshift() ;
        void skiplist_rcu_gpt_base_less_xorshift() ;
        void skiplist_rcu_gpt_base_cmpmix_xorshift() ;
        void skiplist_rcu_gpt_base_cmp_xorshift_stat() ;
        void skiplist_rcu_gpt_base_less_xorshift_stat() ;
        void skiplist_rcu_gpt_base_cmpmix_xorshift_stat() ;
        void skiplist_rcu_gpt_base_cmp_pascal() ;
        void skiplist_rcu_gpt_base_less_pascal() ;
        void skiplist_rcu_gpt_base_cmpmix_pascal() ;
        void skiplist_rcu_gpt_base_cmp_pascal_stat() ;
        void skiplist_rcu_gpt_base_less_pascal_stat() ;
        void skiplist_rcu_gpt_base_cmpmix_pascal_stat() ;

        void skiplist_rcu_gpt_member_cmp() ;
        void skiplist_rcu_gpt_member_less() ;
        void skiplist_rcu_gpt_member_cmpmix() ;
        void skiplist_rcu_gpt_member_cmp_stat() ;
        void skiplist_rcu_gpt_member_less_stat() ;
        void skiplist_rcu_gpt_member_cmpmix_stat() ;
        void skiplist_rcu_gpt_member_cmp_xorshift() ;
        void skiplist_rcu_gpt_member_less_xorshift() ;
        void skiplist_rcu_gpt_member_cmpmix_xorshift() ;
        void skiplist_rcu_gpt_member_cmp_xorshift_stat() ;
        void skiplist_rcu_gpt_member_less_xorshift_stat() ;
        void skiplist_rcu_gpt_member_cmpmix_xorshift_stat() ;
        void skiplist_rcu_gpt_member_cmp_pascal() ;
        void skiplist_rcu_gpt_member_less_pascal() ;
        void skiplist_rcu_gpt_member_cmpmix_pascal() ;
        void skiplist_rcu_gpt_member_cmp_pascal_stat() ;
        void skiplist_rcu_gpt_member_less_pascal_stat() ;
        void skiplist_rcu_gpt_member_cmpmix_pascal_stat() ;

        // signal_buffered
        void skiplist_rcu_shb_base_cmp() ;
        void skiplist_rcu_shb_base_less() ;
        void skiplist_rcu_shb_base_cmpmix() ;
        void skiplist_rcu_shb_base_cmp_stat() ;
        void skiplist_rcu_shb_base_less_stat() ;
        void skiplist_rcu_shb_base_cmpmix_stat() ;
        void skiplist_rcu_shb_base_cmp_xorshift() ;
        void skiplist_rcu_shb_base_less_xorshift() ;
        void skiplist_rcu_shb_base_cmpmix_xorshift() ;
        void skiplist_rcu_shb_base_cmp_xorshift_stat() ;
        void skiplist_rcu_shb_base_less_xorshift_stat() ;
        void skiplist_rcu_shb_base_cmpmix_xorshift_stat() ;
        void skiplist_rcu_shb_base_cmp_pascal() ;
        void skiplist_rcu_shb_base_less_pascal() ;
        void skiplist_rcu_shb_base_cmpmix_pascal() ;
        void skiplist_rcu_shb_base_cmp_pascal_stat() ;
        void skiplist_rcu_shb_base_less_pascal_stat() ;
        void skiplist_rcu_shb_base_cmpmix_pascal_stat() ;

        void skiplist_rcu_shb_member_cmp() ;
        void skiplist_rcu_shb_member_less() ;
        void skiplist_rcu_shb_member_cmpmix() ;
        void skiplist_rcu_shb_member_cmp_stat() ;
        void skiplist_rcu_shb_member_less_stat() ;
        void skiplist_rcu_shb_member_cmpmix_stat() ;
        void skiplist_rcu_shb_member_cmp_xorshift() ;
        void skiplist_rcu_shb_member_less_xorshift() ;
        void skiplist_rcu_shb_member_cmpmix_xorshift() ;
        void skiplist_rcu_shb_member_cmp_xorshift_stat() ;
        void skiplist_rcu_shb_member_less_xorshift_stat() ;
        void skiplist_rcu_shb_member_cmpmix_xorshift_stat() ;
        void skiplist_rcu_shb_member_cmp_pascal() ;
        void skiplist_rcu_shb_member_less_pascal() ;
        void skiplist_rcu_shb_member_cmpmix_pascal() ;
        void skiplist_rcu_shb_member_cmp_pascal_stat() ;
        void skiplist_rcu_shb_member_less_pascal_stat() ;
        void skiplist_rcu_shb_member_cmpmix_pascal_stat() ;

        // signal_threaded
        void skiplist_rcu_sht_base_cmp() ;
        void skiplist_rcu_sht_base_less() ;
        void skiplist_rcu_sht_base_cmpmix() ;
        void skiplist_rcu_sht_base_cmp_stat() ;
        void skiplist_rcu_sht_base_less_stat() ;
        void skiplist_rcu_sht_base_cmpmix_stat() ;
        void skiplist_rcu_sht_base_cmp_xorshift() ;
        void skiplist_rcu_sht_base_less_xorshift() ;
        void skiplist_rcu_sht_base_cmpmix_xorshift() ;
        void skiplist_rcu_sht_base_cmp_xorshift_stat() ;
        void skiplist_rcu_sht_base_less_xorshift_stat() ;
        void skiplist_rcu_sht_base_cmpmix_xorshift_stat() ;
        void skiplist_rcu_sht_base_cmp_pascal() ;
        void skiplist_rcu_sht_base_less_pascal() ;
        void skiplist_rcu_sht_base_cmpmix_pascal() ;
        void skiplist_rcu_sht_base_cmp_pascal_stat() ;
        void skiplist_rcu_sht_base_less_pascal_stat() ;
        void skiplist_rcu_sht_base_cmpmix_pascal_stat() ;

        void skiplist_rcu_sht_member_cmp() ;
        void skiplist_rcu_sht_member_less() ;
        void skiplist_rcu_sht_member_cmpmix() ;
        void skiplist_rcu_sht_member_cmp_stat() ;
        void skiplist_rcu_sht_member_less_stat() ;
        void skiplist_rcu_sht_member_cmpmix_stat() ;
        void skiplist_rcu_sht_member_cmp_xorshift() ;
        void skiplist_rcu_sht_member_less_xorshift() ;
        void skiplist_rcu_sht_member_cmpmix_xorshift() ;
        void skiplist_rcu_sht_member_cmp_xorshift_stat() ;
        void skiplist_rcu_sht_member_less_xorshift_stat() ;
        void skiplist_rcu_sht_member_cmpmix_xorshift_stat() ;
        void skiplist_rcu_sht_member_cmp_pascal() ;
        void skiplist_rcu_sht_member_less_pascal() ;
        void skiplist_rcu_sht_member_cmpmix_pascal() ;
        void skiplist_rcu_sht_member_cmp_pascal_stat() ;
        void skiplist_rcu_sht_member_less_pascal_stat() ;
        void skiplist_rcu_sht_member_cmpmix_pascal_stat() ;

        CPPUNIT_TEST_SUITE(IntrusiveSkipListSetRCU)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmp)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_less)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_less_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_base_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmp)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_less)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_less_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpi_member_cmpmix_pascal_stat)

            //
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmp)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_less)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_less_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_base_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmp)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_less)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_less_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpb_member_cmpmix_pascal_stat)

            //
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmp)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_less)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_less_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_base_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmp)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_less)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_less_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_gpt_member_cmpmix_pascal_stat)

            //
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmp)
            CPPUNIT_TEST(skiplist_rcu_shb_base_less)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_base_less_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_shb_base_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_base_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_shb_base_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_base_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_base_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_rcu_shb_member_cmp)
            CPPUNIT_TEST(skiplist_rcu_shb_member_less)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_member_less_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_shb_member_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_member_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_shb_member_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_member_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_shb_member_cmpmix_pascal_stat)

            //
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmp)
            CPPUNIT_TEST(skiplist_rcu_sht_base_less)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_base_less_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_sht_base_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_base_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_sht_base_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_base_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_base_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_rcu_sht_member_cmp)
            CPPUNIT_TEST(skiplist_rcu_sht_member_less)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmpmix)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmp_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_member_less_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmpmix_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmp_xorshift)
            CPPUNIT_TEST(skiplist_rcu_sht_member_less_xorshift)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_member_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmp_pascal)
            CPPUNIT_TEST(skiplist_rcu_sht_member_less_pascal)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_member_less_pascal_stat)
            CPPUNIT_TEST(skiplist_rcu_sht_member_cmpmix_pascal_stat)

        CPPUNIT_TEST_SUITE_END()
    };
} // namespace set
