/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_DETAILS_CXX11_FEATURES_H
#define __CDS_DETAILS_CXX11_FEATURES_H
//@cond

#ifndef __CDS_DEFS_H
#   error "<cds/details/cxx11_features.h> cannot be included directly, use <cds/details/defs.h> instead"
#endif

// =delete function specifier
#ifdef CDS_CXX11_DELETE_DEFINITION_SUPPORT
#   define CDS_DELETE_SPECIFIER     =delete
#else
#   define CDS_DELETE_SPECIFIER
#endif

// =default function specifier
#ifdef CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
#   define CDS_DEFAULT_SPECIFIER    =default
#else
#   define CDS_DEFAULT_SPECIFIER
#endif

//@endcond
#endif // #ifndef __CDS_DETAILS_CXX11_FEATURES_H
