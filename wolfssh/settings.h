/* settings.h
 *
 * Copyright (C) 2014-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSH.
 *
 * wolfSSH is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wolfSSH.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * The settings header provides a method for developers to provide the
 * internal preprocessor defines that the configure script provides to
 * environments that do not use the configure script.
 */


#pragma once

#include <wolfssh/visibility.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Uncomment next line if using FreeRTOS */
/* #define FREERTOS */

/* Uncomment next line if using FreeRTOS+ TCP */
/* #define FREERTOS_TCP */

/* Uncomment next line if using FreeRTOS Windows Simulator */
/* #define FREERTOS_WINSIM */


/* user settings here */
#ifndef WMALLOC_USER
    #define USE_WOLFSSH_MEMORY  /* default memory handlers */
#endif /* WMALLOC_USER */


#if defined(_WIN32) && !defined(FREERTOS) && !defined(FREERTOS_TCP)
    #define USE_WINDOWS_API
#endif


#ifdef FREERTOS_WINSIM
    #define FREERTOS
    #define USE_WINDOWS_API
#endif


#ifdef FREERTOS
    #include "FreeRTOS.h"

    /* FreeRTOS pvPortRealloc() only in AVR32_UC3 port */
    #if !defined(XMALLOC_USER) && !defined(NO_WOLFSSL_MEMORY)
        #define XMALLOC(s, h, type)  pvPortMalloc((s))
        #define XFREE(p, h, type)    vPortFree((p))
    #endif

    #ifndef NO_WRITEV
        #define NO_WRITEV
    #endif
    #ifndef HAVE_SHA512
        #ifndef NO_SHA512
            #define NO_SHA512
        #endif
    #endif
    #ifndef HAVE_DH
        #ifndef NO_DH
            #define NO_DH
        #endif
    #endif
    #ifndef NO_DSA
        #define NO_DSA
    #endif
    #ifndef NO_HC128
        #define NO_HC128
    #endif

    #ifndef SINGLE_THREADED
        #include "semphr.h"
    #endif
#endif

#ifdef FREERTOS_TCP
    #if !defined(NO_WOLFSSL_MEMORY) && !defined(XMALLOC_USER)
        #define WMALLOC(s, h, type)  pvPortMalloc((s))
        #define WFREE(p, h, type)    vPortFree((p))
    #endif

    #define WOLFSSL_GENSEED_FORTEST

    #define NO_WOLFSSL_DIR
    #define NO_WRITEV
    #define USE_FAST_MATH
    #define TFM_TIMING_RESISTANT
    #define NO_MAIN_DRIVER
#endif


#ifdef __cplusplus
}
#endif

