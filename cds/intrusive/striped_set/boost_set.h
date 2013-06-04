/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_SET_ADAPTER_H
#define __CDS_INTRUSIVE_STRIPED_SET_BOOST_SET_ADAPTER_H

#include <boost/intrusive/set.hpp>
#include <cds/intrusive/striped_set/adapter.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

    template <typename T, CDS_BOOST_INTRUSIVE_DECL_OPTIONS4, CDS_SPEC_OPTIONS>
    class adapt< boost::intrusive::set< T, CDS_BOOST_INTRUSIVE_OPTIONS4 >, CDS_OPTIONS >
    {
    public:
        typedef boost::intrusive::set< T, CDS_BOOST_INTRUSIVE_OPTIONS4 >  container_type  ;   ///< underlying intrusive container type

    public:
        typedef details::boost_intrusive_set_adapter<container_type>   type ;  ///< Result of the metafunction

    };
}}} // namespace cds::intrusive::striped_set
//@endcond

#endif // #ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_SET_ADAPTER_H
