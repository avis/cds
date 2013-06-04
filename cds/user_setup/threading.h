/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf.com) 2006-2013
    Distributed under the BSD license (see accompanying file license.txt)

    Version 1.4.0
*/


#ifndef __CDS_USER_SETUP_THREADING_MODEL_H
#define __CDS_USER_SETUP_THREADING_MODEL_H

/**
    CDS threading model

    CDS_THREADING_AUTODETECT - auto-detect appropriate threading model (default)

    CDS_THREADING_MSVC - use MS Visual C++ declspec( thread ) declaration to mantain thread-specific data

    CDS_THREADING_WIN_TLS - use Windows TLS API to mantain thread-specific data

    CDS_THREADING_GCC - use GCC __thread keyword to mantain thread-specific data

    CDS_THREADING_PTHREAD - use cds::Threading::Manager implementation based on pthread thread-specific
    data functions pthread_getspecific/pthread_setspecific

    CDS_THREADING_USER_DEFINED - use user-defined threading model
*/
#define CDS_THREADING_AUTODETECT

#endif    // #ifndef __CDS_USER_SETUP_THREADING_MODEL_H
