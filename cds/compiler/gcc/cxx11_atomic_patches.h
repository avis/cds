/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_COMPILER_GCC_CXX11_ATOMIC_PATCHES_H
#define __CDS_COMPILER_GCC_CXX11_ATOMIC_PATCHES_H

#if CDS_CXX11_ATOMIC_SUPPORT == 1
#   if CDS_COMPILER_VERSION >= 40600 && CDS_COMPILER_VERSION < 40700
    // GCC 4.6.x has no atomic_thread_fence/atomic_signal_fence
    namespace std {
        inline void atomic_thread_fence(memory_order order)
        {
            switch(order)
            {
            case memory_order_relaxed:
            case memory_order_consume:
                break ;
            case memory_order_release:
            case memory_order_acquire:
            case memory_order_acq_rel:
            case memory_order_seq_cst:
                __sync_synchronize() ;
                break;
            default:;
            }

        }
        inline void atomic_signal_fence(memory_order order)
        {
            switch(order)
            {
            case memory_order_relaxed:
                break ;
            case memory_order_consume:
            case memory_order_release:
            case memory_order_acquire:
            case memory_order_acq_rel:
            case memory_order_seq_cst:
                CDS_COMPILER_RW_BARRIER ;
                break;
            default:;
            }
        }
    } // namespace std
#   endif
#endif

#endif // #ifndef __CDS_COMPILER_GCC_CXX11_ATOMIC_PATCHES_H
