/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#include <cds/urcu/details/sh.h>

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
namespace cds { namespace urcu { namespace details {

    template<> CDS_EXPORT_API singleton_vtbl * sh_singleton_instance< signal_buffered_tag >::s_pRCU = null_ptr<singleton_vtbl *>() ;
    template<> CDS_EXPORT_API singleton_vtbl * sh_singleton_instance< signal_threaded_tag >::s_pRCU = null_ptr<singleton_vtbl *>() ;

}}} // namespace cds::urcu::details

#endif //#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
