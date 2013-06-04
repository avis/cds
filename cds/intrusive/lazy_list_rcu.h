/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_INTRUSIVE_LAZY_LIST_RCU_H
#define __CDS_INTRUSIVE_LAZY_LIST_RCU_H

#include <cds/intrusive/lazy_list_base.h>
#include <cds/urcu/details/check_deadlock.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace intrusive {
    namespace lazy_list {
        /// Lazy list node for \ref cds_urcu_desc "RCU"
        /**
            Template parameters:
            - Tag - a tag used to distinguish between different implementation
        */
        template <class RCU, typename Lock, typename Tag>
        struct node<cds::urcu::gc<RCU>, Lock, Tag>
        {
            typedef cds::urcu::gc<RCU>  gc  ;   ///< RCU schema
            typedef Lock        lock_type   ;   ///< Lock type
            typedef Tag         tag         ;   ///< tag

            typedef cds::details::marked_ptr<node, 1>   marked_ptr          ;   ///< marked pointer
            typedef CDS_ATOMIC::atomic<marked_ptr>      atomic_marked_ptr   ;   ///< atomic marked pointer specific for GC

            atomic_marked_ptr   m_pNext ; ///< pointer to the next node in the list
            mutable lock_type   m_Lock  ; ///< Node lock

            /// Checks if node is marked
            bool is_marked() const
            {
                return m_pNext.load(CDS_ATOMIC::memory_order_relaxed).bits() != 0 ;
            }

            /// Default ctor
            node()
                : m_pNext( null_ptr<node *>())
            {}

            /// Clears internal fields
            void clear()
            {
                m_pNext.store( marked_ptr(), CDS_ATOMIC::memory_order_release ) ;
            }
        };
    }   // namespace lazy_list


    /// Lazy ordered single-linked list (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_LazyList_rcu

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p T - type to be stored in the list
        - \p Traits - type traits. See lazy_list::type_traits for explanation.

        It is possible to declare option-based list with \p %cds::intrusive::lazy_list::make_traits metafunction istead of \p Traits template
        argument. Template argument list \p Options of cds::intrusive::lazy_list::make_traits metafunction are:
        - opt::hook - hook used. Possible values are: lazy_list::base_hook, lazy_list::member_hook, lazy_list::traits_hook.
            If the option is not specified, <tt>lazy_list::base_hook<></tt> is used.
        - opt::compare - key comparison functor. No default functor is provided.
            If the option is not specified, the opt::less is used.
        - opt::less - specifies binary predicate used for key comparision. Default is \p std::less<T>.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used.
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer
        - opt::rcu_check_deadlock - a deadlock checking policy. Default is opt::v::rcu_throw_deadlock
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).

        \par Usage
            Before including <tt><cds/intrusive/lazy_list_rcu.h></tt> you should include appropriate RCU header file,
            see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
            For example, for \ref cds_urcu_general_buffered_gc "general-purpose buffered RCU" you should include:
            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/lazy_list_rcu.h>

            // Now, you can declare lazy list for type Foo and default traits:
            typedef cds::intrusive::LazyList<cds::urcu::gc< cds::urcu::general_buffered<> >, Foo > rcu_lazy_list ;
            \endcode

    */
    template <
        typename RCU
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
        ,class Traits = lazy_list::type_traits
#else
        ,class Traits
#endif
    >
    class LazyList<cds::urcu::gc<RCU>, T, Traits>
    {
    public:
        typedef T       value_type      ;   ///< type of value stored in the list
        typedef Traits  options         ;   ///< Traits template parameter

        typedef typename options::hook      hook        ;   ///< hook type
        typedef typename hook::node_type    node_type   ;   ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key compare functor based on opt::compare and opt::less option setter.
#   else
        typedef typename opt::details::make_comparator< value_type, options >::type key_comparator  ;
#   endif

        typedef typename options::disposer  disposer    ;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename lazy_list::get_link_checker< node_type, options::link_checker >::type link_checker   ;   ///< link checker

        typedef cds::urcu::gc<RCU>                      gc              ;   ///< RCU schema
        typedef typename options::back_off              back_off        ;   ///< back-off strategy (not used)
        typedef typename options::item_counter          item_counter    ;   ///< Item counting policy used
        typedef typename options::memory_model          memory_model    ;   ///< C++ memory ordering (see lazy_list::type_traits::memory_model)
        typedef typename options::rcu_check_deadlock    rcu_check_deadlock ; ///< Deadlock checking policy

        //@cond
        // Rebind options (split-list support)
        template <CDS_DECL_OPTIONS8>
        struct rebind_options {
            typedef LazyList<
                gc
                , value_type
                , typename cds::opt::make_options< options, CDS_OPTIONS8>::type
            >   type   ;
        };
        //@endcond

    protected:
        typedef typename node_type::marked_ptr  marked_node_ptr ;   ///< Node marked pointer
        typedef node_type *     auxiliary_head   ;   ///< Auxiliary head type (for split-list support)

    protected:
        node_type       m_Head ;            ///< List head (dummy node)
        node_type       m_Tail;             ///< List tail (dummy node)
        item_counter    m_ItemCounter   ;   ///< Item counter

        //@cond

        /// Position pointer for item search
        struct position {
            node_type *     pPred   ;    ///< Previous node
            node_type *     pCur    ;    ///< Current node

            /// Locks nodes \p pPred and \p pCur
            void lock()
            {
                pPred->m_Lock.lock()    ;
                pCur->m_Lock.lock()    ;
            }

            /// Unlocks nodes \p pPred and \p pCur
            void unlock()
            {
                pCur->m_Lock.unlock()    ;
                pPred->m_Lock.unlock()    ;
            }
        };

        class auto_lock_position {
            position&   m_pos   ;
        public:
            auto_lock_position( position& pos )
                : m_pos(pos)
            {
                pos.lock()      ;
            }
            ~auto_lock_position()
            {
                m_pos.unlock()  ;
            }
        };

        typedef typename gc::scoped_lock    rcu_lock ;
        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock>   check_deadlock_policy ;

#   ifndef CDS_CXX11_LAMBDA_SUPPORT
        struct empty_erase_functor {
            void operator()( value_type const & item )
            {}
        };
#   endif

        //@endcond

    protected:
        //@cond
        static void clear_links( node_type * pNode )
        {
            pNode->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed ) ;
        }

        struct clear_and_dispose {
            void operator()( value_type * p )
            {
                assert( p != null_ptr<value_type *>() ) ;
                clear_links( node_traits::to_node_ptr(p)) ;
                disposer()( p ) ;
            }
        };

        void dispose_node( node_type * pNode )
        {
            assert( pNode ) ;
            assert( !gc::is_locked() ) ;

            gc::template retire_ptr<clear_and_dispose>( node_traits::to_value_ptr( *pNode ) );
        }

        void link_node( node_type * pNode, node_type * pPred, node_type * pCur )
        {
            assert( pPred->m_pNext.load(memory_model::memory_order_relaxed).ptr() == pCur )  ;

            pNode->m_pNext.store( marked_node_ptr(pCur), memory_model::memory_order_release )  ;
            pPred->m_pNext.store( marked_node_ptr(pNode), memory_model::memory_order_release ) ;
        }

        void unlink_node( node_type * pPred, node_type * pCur )
        {
            assert( pPred->m_pNext.load(memory_model::memory_order_relaxed).ptr() == pCur )  ;
            assert( pCur != &m_Tail )   ;

            node_type * pNext = pCur->m_pNext.load(memory_model::memory_order_relaxed).ptr() ;
            pCur->m_pNext.store( marked_node_ptr( pNext, 1), memory_model::memory_order_relaxed) ;   // logically deleting
            pPred->m_pNext.store( marked_node_ptr( pNext ), memory_model::memory_order_relaxed); // physically deleting
            pCur->m_pNext.store( marked_node_ptr( &m_Head, 1 ), memory_model::memory_order_relaxed )    ; // back-link for search
        }

        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
        {
            friend class LazyList    ;

        protected:
            value_type * m_pNode        ;

            void next()
            {
                assert( m_pNode != null_ptr<value_type *>() )   ;

                node_type * pNode = node_traits::to_node_ptr( m_pNode ) ;
                node_type * pNext = pNode->m_pNext.load(memory_model::memory_order_relaxed).ptr() ;
                if ( pNext != null_ptr<node_type *>() )
                    m_pNode = node_traits::to_value_ptr( pNext ) ;
            }

            void skip_deleted()
            {
                if ( m_pNode != null_ptr<value_type *>() ) {
                    node_type * pNode = node_traits::to_node_ptr( m_pNode ) ;

                    // Dummy tail node could not be marked
                    while ( pNode->is_marked() )
                        pNode = pNode->m_pNext.load(memory_model::memory_order_relaxed).ptr()  ;

                    if ( pNode != node_traits::to_node_ptr( m_pNode ) )
                        m_pNode = node_traits::to_value_ptr( pNode ) ;
                }
            }

            iterator_type( node_type * pNode )
            {
                m_pNode = node_traits::to_value_ptr( pNode )    ;
                skip_deleted() ;
            }

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
                : m_pNode(null_ptr<value_type *>())
            {}

            iterator_type( const iterator_type& src )
                : m_pNode( src.m_pNode )
            {}

            value_ptr operator ->() const
            {
                return m_pNode ;
            }

            value_ref operator *() const
            {
                assert( m_pNode != null_ptr<value_type *>() )   ;
                return *m_pNode ;
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                next()  ;
                skip_deleted() ;
                return *this;
            }

            /// Post-increment
            iterator_type operator ++(int)
            {
                iterator_type i(*this)   ;
                next()  ;
                skip_deleted() ;
                return i;
            }

            iterator_type& operator = (const iterator_type& src)
            {
                m_pNode = src.m_pNode       ;
                return *this    ;
            }

            template <bool C>
            bool operator ==(iterator_type<C> const& i ) const
            {
                return m_pNode == i.m_pNode ;
            }
            template <bool C>
            bool operator !=(iterator_type<C> const& i ) const
            {
                return m_pNode != i.m_pNode ;
            }
        };
        //@endcond

    public:
        /// Forward iterator
        typedef iterator_type<false>    iterator        ;
        /// Const forward iterator
        typedef iterator_type<true>     const_iterator  ;

        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            iterator it( &m_Head )  ;
            ++it        ;   // skip dummy head
            return it   ;
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.

            The returned value can be used only to control reaching the end of the list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( &m_Tail )   ;
        }

        /// Returns a forward const iterator addressing the first element in a list
        //@{
        const_iterator begin() const
        {
            return get_const_begin() ;
        }
        const_iterator cbegin()
        {
            return get_const_begin() ;
        }
        //@}

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        //@{
        const_iterator end() const
        {
            return get_const_end() ;
        }
        const_iterator cend()
        {
            return get_const_end() ;
        }
        //@}

    private:
        //@cond
        const_iterator get_const_begin() const
        {
            const_iterator it( const_cast<node_type *>( &m_Head ))    ;
            ++it        ;   // skip dummy head
            return it   ;
        }
        const_iterator get_const_end() const
        {
            return const_iterator( const_cast<node_type *>( &m_Tail ))   ;
        }
        //@endcond

    public:
        /// Default constructor initializes empty list
        LazyList()
        {
            static_assert( (std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type" ) ;
            m_Head.m_pNext.store( marked_node_ptr( &m_Tail ), memory_model::memory_order_relaxed )  ;
        }

        /// Destroys the list object
        ~LazyList()
        {
            clear() ;

            assert( m_Head.m_pNext.load(memory_model::memory_order_relaxed).ptr() == &m_Tail ) ;
            m_Head.m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed ) ;
        }

        /// Inserts new node
        /**
            The function inserts \p val in the list if the list does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is linked into the list, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert_at( &m_Head, val )    ;
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split new item creating into two part:
            - create item with key only
            - insert new item into the list
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val ) ;
            \endcode
            where \p val is the item inserted.
            While the functor \p f is working the item \p val is locked.
            The user-defined functor is called only if the inserting is success and may be passed by reference
            using <tt>boost::ref</tt>.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            return insert_at( &m_Head, val, f )    ;
        }

        /// Ensures that the \p item exists in the list
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val not found in the list, then \p val is inserted into the list.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
            struct functor {
                void operator()( bool bNew, value_type& item, value_type& val ) ;
            };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list
            - \p val - argument \p val passed into the \p ensure function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refers to the same thing.

            The functor may change non-key fields of the \p item.
            While the functor \p f is calling the item \p item is locked.

            You may pass \p func argument by reference using <tt>boost::ref</tt> or cds::ref.

            Returns <tt> std::pair<bool, bool>  </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the list.
        */

        template <typename Func>
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return ensure_at( &m_Head, val, func )    ;
        }

        /// Unlinks the item \p val from the list
        /**
            The function searches the item \p val in the list and unlink it from the list
            if it is found and it is equal to \p val.

            Difference between \ref erase and \p unlink functions: \p erase finds <i>a key</i>
            and deletes the item found. \p unlink finds an item by key and deletes it
            only if \p val is an item of that list, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            The function returns \p true if success and \p false otherwise.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            The function can throw cds::urcu::rcu_deadlock exception if deadlock is encountered and
            deadlock checking policy is opt::v::rcu_throw_deadlock.
        */
        bool unlink( value_type& val )
        {
            return unlink_at( &m_Head, val )  ;
        }

        /// Deletes the item from the list
        /** \anchor cds_intrusive_LazyList_rcu_find_erase
            The function searches an item with key equal to \p val in the list,
            unlinks it from the list, and returns \p true.
            If the item with the key equal to \p val is not found the function return \p false.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            The function can throw \ref cds::urcu::rcu_deadlock exception if deadlock is encountered and
            deadlock checking policy is opt::v::rcu_throw_deadlock.
        */
        template <typename Q>
        bool erase( Q const& val )
        {
            return erase_at( &m_Head, val, key_comparator() )    ;
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_rcu_find_erase "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& val, Less pred )
        {
            return erase_at( &m_Head, val, cds::opt::details::make_comparator_from_less<Less>())    ;
        }


        /// Deletes the item from the list
        /** \anchor cds_intrusive_LazyList_rcu_find_erase_func
            The function searches an item with key equal to \p val in the list,
            call \p func functor with item found, unlinks it from the list, and returns \p true.
            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item ) ;
            } ;
            \endcode
            The functor may be passed by reference using <tt>boost:ref</tt>

            If the item with the key equal to \p val is not found the function return \p false.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            The function can throw \ref cds::urcu::rcu_deadlock exception if deadlock is encountered and
            deadlock checking policy is opt::v::rcu_throw_deadlock.
        */
        template <typename Q, typename Func>
        bool erase( Q const& val, Func func )
        {
            return erase_at( &m_Head, val, key_comparator(), func )    ;
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_rcu_find_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& val, Less pred, Func func )
        {
            return erase_at( &m_Head, val, cds::opt::details::make_comparator_from_less<Less>(), func )    ;
        }

        /// Extracts an item from the list
        /**
        \anchor cds_intrusive_LazyList_rcu_extract
            The function searches an item with key equal to \p val in the list,
            unlinks it from the list, and returns pointer to an item found.
            If the item with the key equal to \p val is not found the function returns \p NULL.

            @note The function does NOT call RCU read-side lock or synchronization,
            and does NOT dispose the item found. It just excludes the item from the list
            and returns a pointer to item found.
            You should lock RCU before calling of the function, and you should synchronize RCU
            outside the RCU lock region before reusing returned pointer.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/lazy_list_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu ;
            typedef cds::intrusive::LazyList< rcu, Foo > rcu_lazy_list ;

            rcu_lazy_list theList ;
            // ...

            value_type * p1;
            value_type * p2;
            {
                // first, we should lock RCU
                rcu::scoped_lock sl;

                // Now, you can apply extract function
                // Note that you must not delete the item found inside the RCU lock
                p1 = theList.extract( 10 ) ;
                if ( p1 ) {
                    // do something with p1
                    ...
                }

                // Extract another element
                p2 = theList.extract( 20 ) ;
                if ( p2 ) {
                    // do something with p2
                    ...
                }
            }

            if ( you_want_delete_item_extracted ) {
                // If you want delete the items p1 and p2, you can do it here
                // There are two way:

                // Way 1: Using RCU synchronization
                // You should call RCU synchronize primitive
                rcu::synchronize() ;

                // Now, it is safe to delete p1 and p2 after RCU synchronization done
                delete p1;
                delete p2;

                // Way 2: using RCU retire_ptr
                // rcu::retire_ptr calls rcu::synchronize internally
                rcu::retire_ptr( p1, []( value_ptr * p ){ delete p; } );
                rcu::retire_ptr( p2, []( value_ptr * p ){ delete p; } );
            }
            else {
                // If you want reuse extracted item, you should make RCU synchronization
                // and then call item's clear member function:
                rcu::synchronize() ;
                p1->clear() ;

                // Now you can reinsert the item
                theList.insert( *p1 )   ;
            }
            \endcode
        */
        template <typename Q>
        value_type * extract( Q const& val )
        {
            return extract_at( &m_Head, val, key_comparator() ) ;
        }

        /// Extracts an item from the list using \p pred predicate for searching
        /**
            This function is the analog for \ref cds_intrusive_LazyList_rcu_extract "extract(Q const&)".

            The \p pred is a predicate used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as \ref key_comparator.
        */
        template <typename Q, typename Less>
        value_type * extract_with( Q const& val, Less pred )
        {
            return extract_at( &m_Head, val, cds::opt::details::make_comparator_from_less<Less>() ) ;
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_LazyList_rcu_find_func
            The function searches the item with key equal to \p val
            and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& val ) ;
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You may pass \p f argument by reference using <tt>boost::ref</tt> or cds::ref.

            The functor may change non-key fields of \p item.
            While the functor \p f is calling the item found \p item is locked.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& val, Func f ) const
        {
            return find_at( const_cast<node_type *>( &m_Head ), val, key_comparator(), f )  ;
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& val, Less pred, Func f ) const
        {
            return find_at( const_cast<node_type *>( &m_Head ), val, cds::opt::details::make_comparator_from_less<Less>(), f )  ;
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_LazyList_rcu_find_cfunc
            The function searches the item with key equal to \p val
            and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q const& val ) ;
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You may pass \p f argument by reference using <tt>boost::ref</tt> or cds::ref.

            The functor may change non-key fields of \p item.
            While the functor \p f is calling the item found \p item is locked.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& val, Func f ) const
        {
            return find_at( const_cast<node_type *>( &m_Head ), val, key_comparator(), f )  ;
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_rcu_find_cfunc "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& val, Less pred, Func f ) const
        {
            return find_at( const_cast<node_type *>( &m_Head ), val, cds::opt::details::make_comparator_from_less<Less>(), f )  ;
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_LazyList_rcu_find_val
            The function searches the item with key equal to \p val
            and returns \p true if \p val found or \p false otherwise.
        */
        template <typename Q>
        bool find( Q const& val ) const
        {
            return find_at( const_cast<node_type *>( &m_Head ), val, key_comparator() )  ;
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_rcu_find_val "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool find_with( Q const& val, Less pred ) const
        {
            return find_at( const_cast<node_type *>( &m_Head ), val, cds::opt::details::make_comparator_from_less<Less>() ) ;
        }

        /// Clears the list using default disposer
        /**
            The function clears the list using default (provided in class template) disposer functor.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            The function can throw cds::urcu::rcu_deadlock exception if deadlock is encountered and
            deadlock checking policy is opt::v::rcu_throw_deadlock.
        */
        void clear()
        {
            if( !empty() ) {
                check_deadlock_policy::check() ;

                node_type * pHead ;
                for (;;) {
                    {
                        rcu_lock l ;
                        pHead = m_Head.m_pNext.load(memory_model::memory_order_acquire).ptr() ;
                        if ( pHead == &m_Tail )
                            break;

                        m_Head.m_Lock.lock()  ;
                        pHead->m_Lock.lock()   ;

                        if ( m_Head.m_pNext.load(memory_model::memory_order_relaxed).all() == pHead )
                            unlink_node( &m_Head, pHead ) ;

                        pHead->m_Lock.unlock()  ;
                        m_Head.m_Lock.unlock() ;
                    }

                    --m_ItemCounter ;
                    dispose_node( pHead )    ;
                }
            }
        }

        /// Checks if the list is empty
        bool empty() const
        {
            return m_Head.m_pNext.load(memory_model::memory_order_relaxed).ptr() == &m_Tail    ;
        }

        /// Returns list's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact is not mean that the list
            is empty. To check list emptyness use \ref empty() method.
        */
        size_t size() const
        {
            return m_ItemCounter.value()    ;
        }

    protected:
        //@cond
        // split-list support
        bool insert_aux_node( node_type * pNode )
        {
            return insert_aux_node( &m_Head, pNode )    ;
        }

        // split-list support
        bool insert_aux_node( node_type * pHead, node_type * pNode )
        {
            assert( pHead != null_ptr<node_type *>() ) ;
            assert( pNode != null_ptr<node_type *>() ) ;

            // Hack: convert node_type to value_type.
            // In principle, auxiliary node can be non-reducible to value_type
            // We assume that comparator can correctly distinguish aux and regular node.
            return insert_at( pHead, *node_traits::to_value_ptr( pNode ) )    ;
        }

        bool insert_at( node_type * pHead, value_type& val, bool bLock = true )
        {
            link_checker::is_empty( node_traits::to_node_ptr( val ) ) ;
            position pos    ;
            key_comparator  cmp ;

            rcu_lock l( bLock )   ;
            while ( true ) {
                search( pHead, val, pos )   ;
                {
                    auto_lock_position alp( pos )   ;
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // failed: key already in list
                            return false        ;
                        }
                        else {
                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur )   ;
                            ++m_ItemCounter     ;
                            return true         ;
                        }
                    }
                }
            }
        }

        template <typename Func>
        bool insert_at( node_type * pHead, value_type& val, Func f )
        {
            link_checker::is_empty( node_traits::to_node_ptr( val ) ) ;
            position pos    ;
            key_comparator  cmp ;

            rcu_lock l  ;
            while ( true ) {
                search( pHead, val, pos )   ;
                {
                    auto_lock_position alp( pos )   ;
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // failed: key already in list
                            return false        ;
                        }
                        else {
                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur )   ;
                            cds::unref(f)( val )    ;
                            ++m_ItemCounter         ;
                            return true             ;
                        }
                    }
                }
            }
        }

        iterator insert_at_( node_type * pHead, value_type& val, bool bLock = true )
        {
            rcu_lock l( bLock )  ;
            if ( insert_at( pHead, val, false ))
                return iterator( node_traits::to_node_ptr( val )) ;
            return end()    ;
        }


        template <typename Func>
        std::pair<iterator, bool> ensure_at_( node_type * pHead, value_type& val, Func func, bool bLock = true )
        {
            position pos    ;
            key_comparator  cmp ;

            rcu_lock l( bLock ) ;
            while ( true ) {
                search( pHead, val, pos )   ;
                {
                    auto_lock_position alp( pos )   ;
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // key already in the list

                            cds::unref(func)( false, *node_traits::to_value_ptr( *pos.pCur ) , val ) ;
                            return std::make_pair( iterator( pos.pCur ), false )    ;
                        }
                        else {
                            // new key
                            link_checker::is_empty( node_traits::to_node_ptr( val ) ) ;

                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur )   ;
                            cds::unref(func)( true, val, val )     ;
                            ++m_ItemCounter                     ;
                            return std::make_pair( iterator( node_traits::to_node_ptr( val )), true ) ;
                        }
                    }
                }
            }
        }

        template <typename Func>
        std::pair<bool, bool> ensure_at( node_type * pHead, value_type& val, Func func, bool bLock = true )
        {
            rcu_lock l( bLock ) ;
            std::pair<iterator, bool> ret = ensure_at_( pHead, val, func, false ) ;
            return std::make_pair( ret.first != end(), ret.second ) ;
        }

        bool unlink_at( node_type * pHead, value_type& val )
        {
            position pos    ;
            key_comparator  cmp ;
            check_deadlock_policy::check() ;

            while ( true ) {
                int nResult = 0 ;
                {
                    rcu_lock l ;
                    search( pHead, val, pos )   ;
                    {
                        auto_lock_position alp( pos )   ;
                        if ( validate( pos.pPred, pos.pCur ) ) {
                            if ( pos.pCur != &m_Tail
                                && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0
                                && node_traits::to_value_ptr( pos.pCur ) == &val )
                            {
                                // item found
                                unlink_node( pos.pPred, pos.pCur )  ;
                                --m_ItemCounter     ;
                                nResult = 1         ;
                            }
                            else
                                nResult = -1    ;
                        }
                    }
                }

                if ( nResult ) {
                    if ( nResult > 0 ) {
                        dispose_node( pos.pCur )     ;
                        return true     ;
                    }
                    return false        ;
                }
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( node_type * pHead, const Q& val, Compare cmp, Func f, position& pos )
        {
            check_deadlock_policy::check() ;

            while ( true ) {
                int nResult = 0 ;
                {
                    rcu_lock l ;
                    search( pHead, val, pos, cmp )   ;
                    {
                        auto_lock_position alp( pos )   ;
                        if ( validate( pos.pPred, pos.pCur )) {
                            if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                                // key found
                                unlink_node( pos.pPred, pos.pCur )  ;
                                cds::unref(f)( *node_traits::to_value_ptr( *pos.pCur ) )   ;
                                --m_ItemCounter     ;
                                nResult = 1         ;
                            }
                            else {
                                nResult = -1        ;
                            }
                        }
                    }
                }

                if ( nResult ) {
                    if ( nResult > 0 ) {
                        dispose_node( pos.pCur ) ;
                        return true     ;
                    }
                    return false    ;
                }
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( node_type * pHead, const Q& val, Compare cmp, Func f )
        {
            position pos    ;
            return erase_at( pHead, val, cmp, f, pos )    ;
        }

        template <typename Q, typename Compare>
        bool erase_at( node_type * pHead, const Q& val, Compare cmp )
        {
            position pos    ;
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return erase_at( pHead, val, cmp, [](value_type const &){}, pos )    ;
#       else
            return erase_at( pHead, val, cmp, empty_erase_functor(), pos )    ;
#       endif
        }

        template <typename Q, typename Compare>
        value_type * extract_at( node_type * pHead, const Q& val, Compare cmp )
        {
            position pos ;
            assert( gc::is_locked() ) ; // RCU must be locked!!!

            while ( true ) {
                search( pHead, val, pos, cmp )   ;
                int nResult = 0 ;
                {
                    auto_lock_position alp( pos )   ;
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // key found
                            unlink_node( pos.pPred, pos.pCur )  ;
                            --m_ItemCounter     ;
                            nResult = 1         ;
                        }
                        else {
                            nResult = -1        ;
                        }
                    }
                }

                if ( nResult ) {
                    if ( nResult > 0 )
                        return node_traits::to_value_ptr( pos.pCur ) ;
                    return null_ptr<value_type *>() ;
                }
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( node_type * pHead, Q& val, Compare cmp, Func f, bool bLock = true ) const
        {
            position pos    ;

            rcu_lock l( bLock ) ;
            search( pHead, val, pos, cmp )   ;
            if ( pos.pCur != &m_Tail ) {
                cds::lock::scoped_lock< typename node_type::lock_type> al( pos.pCur->m_Lock )  ;
                if ( cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 )
                {
                    cds::unref(f)( *node_traits::to_value_ptr( *pos.pCur ), val ) ;
                    return true ;
                }
            }
            return false    ;
        }

        template <typename Q, typename Compare>
        bool find_at( node_type * pHead, Q& val, Compare cmp ) const
        {
            rcu_lock l;
            return find_at_( pHead, val, cmp ) != end() ;
        }

        template <typename Q, typename Compare>
        const_iterator find_at_( node_type * pHead, Q& val, Compare cmp ) const
        {
            assert( gc::is_locked() ) ;

            position pos    ;

            search( pHead, val, pos, cmp )   ;
            if ( pos.pCur != &m_Tail ) {
                cds::lock::scoped_lock< typename node_type::lock_type> al( pos.pCur->m_Lock )  ;
                if ( cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 )
                {
                    return const_iterator( pos.pCur ) ;
                }
            }
            return end()    ;
        }
        //@endcond

    protected:
        //@cond
        template <typename Q>
        void search( node_type * pHead, const Q& key, position& pos ) const
        {
            search( pHead, key, pos, key_comparator() ) ;
        }

        template <typename Q, typename Compare>
        void search( node_type * pHead, const Q& key, position& pos, Compare cmp ) const
        {
            // RCU lock should be locked!!!
            assert( gc::is_locked() )   ;

            const node_type * pTail = &m_Tail   ;

            marked_node_ptr pCur(pHead)    ;
            marked_node_ptr pPrev(pHead)   ;

            while ( pCur.ptr() != pTail && ( pCur.ptr() == pHead || cmp( *node_traits::to_value_ptr( *pCur.ptr() ), key ) < 0 )) {
                pPrev = pCur ;
                pCur = pCur->m_pNext.load(memory_model::memory_order_relaxed)    ;
            }

            pos.pCur = pCur.ptr()   ;
            pos.pPred = pPrev.ptr() ;
        }

        static bool validate( node_type * pPred, node_type * pCur )
        {
            // RCU lock should be locked!!!
            assert( gc::is_locked() )   ;

            return !pPred->is_marked()
                && !pCur->is_marked()
                && pPred->m_pNext.load(memory_model::memory_order_relaxed) == pCur ;
        }

        //@endcond
    };

}}  // namespace cds::intrusive

#endif  // #ifndef __CDS_INTRUSIVE_LAZY_LIST_RCU_H
