/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#include "cppunit/cppunit_proxy.h"
#include <cds/container/michael_list_base.h>

namespace ordlist {
    namespace cc = cds::container       ;
    namespace co = cds::container::opt  ;

    class MichaelListTestHeader: public CppUnitMini::TestCase
    {
    public:
        struct stat {
            int nEnsureExistsCall;
            int nEnsureNewCall  ;

            stat()
            {
                nEnsureExistsCall
                    = nEnsureNewCall
                    = 0 ;
            }
        };

        struct item {
            int     nKey    ;
            int     nVal    ;

            stat    s       ;

            item(int key)
                : nKey( key )
                , nVal( key * 2 )
                , s()
            {}

            item(int key, int val)
                : nKey( key )
                , nVal(val)
                , s()
            {}

            item( const item& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
                , s()
            {}

            int key() const
            {
                return nKey ;
            }
        };

        template <typename T>
        struct lt
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() < v2.key()  ;
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() < v2  ;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 < v2.key()  ;
            }
        };

        template <typename T>
        struct cmp {
            int operator ()(const T& v1, const T& v2 ) const
            {
                if ( v1.key() < v2.key() )
                    return -1   ;
                return v1.key() > v2.key() ? 1 : 0  ;
            }

            template <typename Q>
            int operator ()(const T& v1, const Q& v2 ) const
            {
                if ( v1.key() < v2 )
                    return -1   ;
                return v1.key() > v2 ? 1 : 0  ;
            }

            template <typename Q>
            int operator ()(const Q& v1, const T& v2 ) const
            {
                if ( v1 < v2.key() )
                    return -1   ;
                return v1 > v2.key() ? 1 : 0  ;
            }
        };

        struct insert_functor {
            void operator ()( item& i )
            {
                i.nVal = i.nKey * 1033  ;
            }
        };
        struct dummy_insert_functor {
            void operator ()( item& i )
            {
                // This functor should not be called
                TestCase::current_test()->error( "CPPUNIT_ASSERT", "dummy_insert_functor should not be called", __FILE__, __LINE__ ) ;
            }
        };


        struct erase_functor {
            unsigned int nEraseCall ;

            erase_functor()
                : nEraseCall(0)
            {}

            void operator()( item const& /*i*/)
            {
                ++nEraseCall    ;
            }
        };

        static void insert_function( item& i )
        {
            i.nVal = i.nKey * 1024  ;
        }
        static void dummy_insert_function( item& i )
        {
            // This function should not be called
            TestCase::current_test()->error( "CPPUNIT_ASSERT", "dummy_insert_function should not be called", __FILE__, __LINE__ ) ;
        }


        struct check_value {
            unsigned int m_nMultiplier    ;

            check_value( unsigned int nMultiplier )
                : m_nMultiplier( nMultiplier )
            {}

            check_value( const check_value& s )
                : m_nMultiplier( s.m_nMultiplier )
            {}

            void operator()( item& i, int )
            {
                CPPUNIT_ASSERT_CURRENT( int(i.nKey * m_nMultiplier) == i.nVal )  ;
            }
        };

        struct check_exact_value {
            int m_nExpected ;

            check_exact_value( int nExpected )
                : m_nExpected( nExpected )
            {}

            check_exact_value( check_exact_value const& s)
                : m_nExpected( s.m_nExpected )
            {}

            void operator()( item& i, int )
            {
                CPPUNIT_ASSERT_CURRENT( i.nVal == m_nExpected )  ;
            }
        };

        struct dummy_check_value {
            void operator()( item& i, int )
            {
                // This functor should not be called
                TestCase::current_test()->error( "CPPUNIT_ASSERT", "dummy_check_value should not be called", __FILE__, __LINE__ ) ;
            }
        };

        struct ensure_functor {
            void operator()( bool bNew, item& i, int n )
            {
                i.nVal = i.nKey * 1024 ;
            }
        };

        static void ensure_func( bool bNew, item& i, int n )
        {
            i.nVal = n * 1033 ;
        }

    protected:
        template <class OrdList>
        void test()
        {
            typedef typename OrdList::value_type    value_type  ;

            {
                OrdList l   ;

                // The list should be empty
                CPPUNIT_ASSERT( l.empty() ) ;

                // insert test
                CPPUNIT_ASSERT( l.insert( 50 ) )     ;
                CPPUNIT_ASSERT( l.insert( item( 25 )) )     ;
                CPPUNIT_ASSERT( l.insert( item( 100 )) )    ;

                // insert failed - such key exists
                CPPUNIT_ASSERT( !l.insert( 50 ) )     ;
                CPPUNIT_ASSERT( !l.insert( item( 100 )) )    ;

                // clear test

                // The list should not be empty
                CPPUNIT_ASSERT( !l.empty() ) ;
                l.clear()   ;
                // and now the list is empty
                CPPUNIT_ASSERT( l.empty() ) ;

                // Test insert with functor

                CPPUNIT_ASSERT( l.insert( 100, insert_functor() ) )     ;
                // passed by ref
                {
                    insert_functor f    ;
                    CPPUNIT_ASSERT( l.insert( item(25), boost::ref(f)) )     ;
                    CPPUNIT_ASSERT( !l.insert( item(100), boost::ref(f)) )     ;
                }
                // Test insert with function
                CPPUNIT_ASSERT( l.insert( 50, insert_function ))      ;
                CPPUNIT_ASSERT( !l.insert( 25, dummy_insert_function ))      ;
                CPPUNIT_ASSERT( !l.insert( 100, dummy_insert_functor() ))      ;

                // The list should not be empty
                CPPUNIT_ASSERT( !l.empty() ) ;

                // Check inserted values
                {
                    int i ;
                    i = 100 ;
                    CPPUNIT_ASSERT( l.find( 100 ))   ;
                    CPPUNIT_ASSERT( l.find( i, check_value(1033) ))   ;
                    {
                        check_value f(1033) ;
                        i = 25 ;
                        CPPUNIT_ASSERT( l.find_with( 25, lt<value_type>() ))   ;
                        CPPUNIT_ASSERT( l.find_with( i, lt<value_type>(), boost::ref(f) ))   ;
                    }
                    i = 50 ;
                    CPPUNIT_ASSERT( l.find( 50 ))   ;
                    CPPUNIT_ASSERT( l.find( i, check_value(1024) ))   ;

                    i = 10 ;
                    CPPUNIT_ASSERT( !l.find_with( 10, lt<value_type>() ))  ;
                    CPPUNIT_ASSERT( !l.find_with( i, lt<value_type>(), dummy_check_value() ))  ;
                    i = 75 ;
                    CPPUNIT_ASSERT( !l.find( 75 ))  ;
                    CPPUNIT_ASSERT( !l.find( i, dummy_check_value() ))  ;
                    i = 150 ;
                    CPPUNIT_ASSERT( !l.find( 150 ))  ;
                    CPPUNIT_ASSERT( !l.find( i, dummy_check_value() ))  ;
                }

                // The list should not be empty
                CPPUNIT_ASSERT( !l.empty() ) ;
                l.clear()   ;
                // and now the list is empty
                CPPUNIT_ASSERT( l.empty() ) ;

                // Ensure test
                {
                    std::pair<bool, bool>   ensureResult ;
                    ensure_functor f    ;
                    ensureResult = l.ensure( 100, ensure_functor() )    ;
                    CPPUNIT_ASSERT( ensureResult.first )    ;
                    CPPUNIT_ASSERT( ensureResult.second )    ;

                    ensureResult = l.ensure( 200, boost::ref(f) )    ;
                    CPPUNIT_ASSERT( ensureResult.first )    ;
                    CPPUNIT_ASSERT( ensureResult.second )    ;

                    ensureResult = l.ensure( 50, ensure_func )    ;
                    CPPUNIT_ASSERT( ensureResult.first )    ;
                    CPPUNIT_ASSERT( ensureResult.second )    ;

                    int i   ;
                    i = 100 ;
                    CPPUNIT_ASSERT( l.find( i, check_value(1024) ))   ;
                    i = 50 ;
                    CPPUNIT_ASSERT( l.find( i, check_value(1033) ))   ;
                    i = 200 ;
                    CPPUNIT_ASSERT( l.find( i, check_value(1024) ))   ;

                    // ensure existing key
                    ensureResult = l.ensure( 200, ensure_func )    ;
                    CPPUNIT_ASSERT( ensureResult.first )    ;
                    CPPUNIT_ASSERT( !ensureResult.second )    ;
                    i = 200 ;
                    CPPUNIT_ASSERT( l.find( i, check_value(1033) ))   ;

                    ensureResult = l.ensure( 50, ensure_functor() )    ;
                    CPPUNIT_ASSERT( ensureResult.first )    ;
                    CPPUNIT_ASSERT( !ensureResult.second )    ;
                    i = 50 ;
                    CPPUNIT_ASSERT( l.find( i, check_value(1024) ))   ;
                }

                // erase test (list: 50, 100, 200)
                CPPUNIT_ASSERT( !l.empty() ) ;
                CPPUNIT_ASSERT( l.insert(160))  ;
                CPPUNIT_ASSERT( l.insert(250))  ;
                CPPUNIT_ASSERT( !l.empty() ) ;

                CPPUNIT_ASSERT( !l.erase( 150 ))   ;

                CPPUNIT_ASSERT( l.erase( 100 ))   ;
                CPPUNIT_ASSERT( !l.erase( 100 ))   ;

                CPPUNIT_ASSERT( l.erase_with( 200, lt<value_type>() ))   ;
                CPPUNIT_ASSERT( !l.erase_with( 200, lt<value_type>() ))   ;

                {
                    erase_functor ef ;
                    CPPUNIT_ASSERT( ef.nEraseCall == 0 )    ;
                    CPPUNIT_ASSERT( l.erase_with( 160, lt<value_type>(), cds::ref(ef) ))   ;
                    CPPUNIT_ASSERT( ef.nEraseCall == 1 )    ;
                    CPPUNIT_ASSERT( !l.erase_with( 160, lt<value_type>(), cds::ref(ef) ))   ;
                    CPPUNIT_ASSERT( ef.nEraseCall == 1 )    ;

                    CPPUNIT_ASSERT( l.erase( 250, cds::ref(ef) ))   ;
                    CPPUNIT_ASSERT( ef.nEraseCall == 2 )    ;
                    CPPUNIT_ASSERT( !l.erase( 250, cds::ref(ef) ))   ;
                    CPPUNIT_ASSERT( ef.nEraseCall == 2 )    ;
                }

                CPPUNIT_ASSERT( l.erase( 50 ))   ;
                CPPUNIT_ASSERT( !l.erase( 50 ))   ;

                CPPUNIT_ASSERT( l.empty() ) ;

                // clear empty list
                l.clear()   ;
                CPPUNIT_ASSERT( l.empty() ) ;

#ifdef CDS_EMPLACE_SUPPORT
                {
                    int i ;

                    // insert test
                    CPPUNIT_ASSERT( l.emplace( 501 ) )     ;
                    CPPUNIT_ASSERT( l.emplace( 251, 152 )) ;
                    CPPUNIT_ASSERT( l.emplace( item( 1001 )) )    ;

                    // insert failed - such key exists
                    CPPUNIT_ASSERT( !l.emplace( 501, 2 ) )     ;
                    CPPUNIT_ASSERT( !l.emplace( 251, 10) )    ;

                    i = 501 ;
                    CPPUNIT_ASSERT( l.find( i, check_exact_value(501*2) ))   ;
                    i = 251 ;
                    CPPUNIT_ASSERT( l.find( i, check_exact_value(152) ))   ;
                    i = 1001 ;
                    CPPUNIT_ASSERT( l.find( i, check_exact_value(1001*2) ))   ;

                    l.clear()   ;
                    CPPUNIT_ASSERT( l.empty() ) ;
                }
#endif

                // Iterator test
                {
                    int nCount = 100   ;
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i) )   ;

                    int i = 0   ;
                    for ( typename OrdList::iterator it = l.begin(), itEnd = l.end(); it != itEnd; ++it, ++i ) {
                        it->nVal = i * 2    ;
                        CPPUNIT_ASSERT( it->nKey == i ) ;
                    }

                    // Check that we have visited all items
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.find( i, check_value(2) ))   ;

                    l.clear()   ;
                    CPPUNIT_ASSERT( l.empty() ) ;

                    // Const iterator
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i) )   ;

                    i = 0   ;
                    const OrdList& rl = l ;
                    for ( typename OrdList::const_iterator it = rl.begin(), itEnd = rl.end(); it != itEnd; ++it, ++i ) {
                        // it->nVal = i * 2    ;    // not!
                        CPPUNIT_ASSERT( it->nKey == i ) ;
                    }

                    // Check that we have visited all items
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.find( i, check_value(2) ))   ;

                    l.clear()   ;
                    CPPUNIT_ASSERT( l.empty() ) ;
                }
            }
        }

        template <class OrdList>
        void nogc_test()
        {
            typedef OrdList list    ;
            typedef typename list::value_type    value_type  ;
            typedef std::pair<typename list::iterator, bool> ensure_result ;

            typename list::iterator it  ;

            list l  ;
            CPPUNIT_ASSERT( l.empty() ) ;
            CPPUNIT_ASSERT( l.insert(50) != l.end() )   ;
            CPPUNIT_ASSERT( !l.empty() ) ;

            ensure_result eres = l.ensure( item(100, 33) ) ;
            CPPUNIT_ASSERT( eres.second )   ;
            CPPUNIT_ASSERT( eres.first != l.end() ) ;
            CPPUNIT_ASSERT( l.insert( item(150) ) != l.end() )   ;

            CPPUNIT_ASSERT( l.insert(100) == l.end() )   ;
            eres = l.ensure( item(50, 33) ) ;
            CPPUNIT_ASSERT( !eres.second )   ;
            CPPUNIT_ASSERT( eres.first->nVal == eres.first->nKey * 2 )   ;
            eres.first->nVal = 63   ;

            it = l.find( 33 )       ;
            CPPUNIT_ASSERT( it == l.end() ) ;

            it = l.find( 50 )   ;
            CPPUNIT_ASSERT( it != l.end() ) ;
            CPPUNIT_ASSERT( it->nKey == 50 ) ;
            CPPUNIT_ASSERT( it->nVal == 63 ) ;

            it = l.find_with( 100, lt<value_type>() )   ;
            CPPUNIT_ASSERT( it != l.end() ) ;
            CPPUNIT_ASSERT( it->nKey == 100 ) ;
            CPPUNIT_ASSERT( it->nVal == 33 ) ;

            it = l.find( 150 )   ;
            CPPUNIT_ASSERT( it != l.end() ) ;
            CPPUNIT_ASSERT( it->nKey == 150 ) ;
            CPPUNIT_ASSERT( it->nVal == it->nKey * 2 ) ;

            CPPUNIT_ASSERT( !l.empty() ) ;
            l.clear()   ;
            CPPUNIT_ASSERT( l.empty() ) ;

#ifdef CDS_EMPLACE_SUPPORT
            // insert test
            CPPUNIT_ASSERT( l.emplace( 501 ) != l.end() )     ;
            CPPUNIT_ASSERT( l.emplace( 251, 152 ) != l.end()) ;
            CPPUNIT_ASSERT( l.emplace( item( 1001 )) != l.end() )    ;

            // insert failed - such key exists
            CPPUNIT_ASSERT( l.emplace( 501, 2 ) == l.end() ) ;
            CPPUNIT_ASSERT( l.emplace( 251, 10) == l.end() ) ;

            it = l.find( 501 ) ;
            CPPUNIT_ASSERT( it != l.end() ) ;
            CPPUNIT_ASSERT( it->nKey == 501 ) ;
            CPPUNIT_ASSERT( it->nVal == 501 * 2 ) ;

            it = l.find( 251 ) ;
            CPPUNIT_ASSERT( it != l.end() ) ;
            CPPUNIT_ASSERT( it->nKey == 251 ) ;
            CPPUNIT_ASSERT( it->nVal == 152 ) ;

            it = l.find( 1001 ) ;
            CPPUNIT_ASSERT( it != l.end() ) ;
            CPPUNIT_ASSERT( it->nKey == 1001 ) ;
            CPPUNIT_ASSERT( it->nVal == 1001 * 2 ) ;

            l.clear()   ;
            CPPUNIT_ASSERT( l.empty() ) ;
#endif
        }

        void HP_cmp()           ;
        void HP_less()          ;
        void HP_cmpmix()        ;
        void HP_ic()            ;

        void PTB_cmp()          ;
        void PTB_less()         ;
        void PTB_cmpmix()       ;
        void PTB_ic()           ;

        void HRC_cmp()          ;
        void HRC_less()         ;
        void HRC_cmpmix()       ;
        void HRC_ic()           ;

        void RCU_GPI_cmp()          ;
        void RCU_GPI_less()         ;
        void RCU_GPI_cmpmix()       ;
        void RCU_GPI_ic()           ;

        void RCU_GPB_cmp()          ;
        void RCU_GPB_less()         ;
        void RCU_GPB_cmpmix()       ;
        void RCU_GPB_ic()           ;

        void RCU_GPT_cmp()          ;
        void RCU_GPT_less()         ;
        void RCU_GPT_cmpmix()       ;
        void RCU_GPT_ic()           ;

        void RCU_SHB_cmp()          ;
        void RCU_SHB_less()         ;
        void RCU_SHB_cmpmix()       ;
        void RCU_SHB_ic()           ;

        void RCU_SHT_cmp()          ;
        void RCU_SHT_less()         ;
        void RCU_SHT_cmpmix()       ;
        void RCU_SHT_ic()           ;

        void NOGC_cmp()         ;
        void NOGC_less()        ;
        void NOGC_cmpmix()      ;
        void NOGC_ic()          ;

        CPPUNIT_TEST_SUITE(MichaelListTestHeader)
            CPPUNIT_TEST(HP_cmp)
            CPPUNIT_TEST(HP_less)
            CPPUNIT_TEST(HP_cmpmix)
            CPPUNIT_TEST(HP_ic)

            CPPUNIT_TEST(PTB_cmp)
            CPPUNIT_TEST(PTB_less)
            CPPUNIT_TEST(PTB_cmpmix)
            CPPUNIT_TEST(PTB_ic)

            CPPUNIT_TEST(HRC_cmp)
            CPPUNIT_TEST(HRC_less)
            CPPUNIT_TEST(HRC_cmpmix)
            CPPUNIT_TEST(HRC_ic)

            CPPUNIT_TEST(RCU_GPI_cmp)
            CPPUNIT_TEST(RCU_GPI_less)
            CPPUNIT_TEST(RCU_GPI_cmpmix)
            CPPUNIT_TEST(RCU_GPI_ic)

            CPPUNIT_TEST(RCU_GPB_cmp)
            CPPUNIT_TEST(RCU_GPB_less)
            CPPUNIT_TEST(RCU_GPB_cmpmix)
            CPPUNIT_TEST(RCU_GPB_ic)

            CPPUNIT_TEST(RCU_GPT_cmp)
            CPPUNIT_TEST(RCU_GPT_less)
            CPPUNIT_TEST(RCU_GPT_cmpmix)
            CPPUNIT_TEST(RCU_GPT_ic)

            CPPUNIT_TEST(RCU_SHB_cmp)
            CPPUNIT_TEST(RCU_SHB_less)
            CPPUNIT_TEST(RCU_SHB_cmpmix)
            CPPUNIT_TEST(RCU_SHB_ic)

            CPPUNIT_TEST(RCU_SHT_cmp)
            CPPUNIT_TEST(RCU_SHT_less)
            CPPUNIT_TEST(RCU_SHT_cmpmix)
            CPPUNIT_TEST(RCU_SHT_ic)

            CPPUNIT_TEST(NOGC_cmp)
            CPPUNIT_TEST(NOGC_less)
            CPPUNIT_TEST(NOGC_cmpmix)
            CPPUNIT_TEST(NOGC_ic)
        CPPUNIT_TEST_SUITE_END()
    };

}   // namespace ordlist
