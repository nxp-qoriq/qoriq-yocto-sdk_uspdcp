/* Copyright (c) 2011 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SEC_UTILS_H
#define SEC_UTILS_H

/*==============================================================================
                                INCLUDE FILES
==============================================================================*/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*==============================================================================
                              DEFINES AND MACROS
==============================================================================*/

#define TRUE    1
#define FALSE   0

#if SEC_DRIVER_LOGGING == OFF

/** No logging of info messages */
#define SEC_INFO(format, ...)
/** No logging of error messages */
#define SEC_ERROR(format, ...)

#elif SEC_DRIVER_LOGGING == ON

#if SEC_DRIVER_LOGGING_LEVEL == SEC_DRIVER_LOG_INFO

/** Log info messages to stdout */
#define SEC_INFO(format, ...) printf("%s() (%s@%d): " format "\n", __FUNCTION__, __FILE__, __LINE__ , ##__VA_ARGS__) 
/** No logging of error messages */
#define SEC_ERROR(format, ...) printf("%s() (%s@%d): " format "\n", __FUNCTION__, __FILE__, __LINE__ , ##__VA_ARGS__) 

#elif SEC_DRIVER_LOGGING_LEVEL == SEC_DRIVER_LOG_ERROR

/** Log info messages to stdout */
#define SEC_INFO(format, ...)
/** Log error messages to stdout */
#define SEC_ERROR(format, ...) printf("%s() (%s@%d): " format "\n", __FUNCTION__, __FILE__, __LINE__ , ##__VA_ARGS__) 

#else
#error "Invalid value for SEC_DRIVER_LOGGING_LEVEL!"
#endif // #if SEC_DRIVER_LOGGING_LEVEL == SEC_DRIVER_LOG_INFO

#else // #if SEC_DRIVER_LOGGING == OFF
#error "Invalid value for SEC_DRIVER_LOGGING!"
#endif

/**
 * Assert that cond is true. If !cond is true, display str and the vararg list in a printf-like syntax.
 * also, if !cond is true, return altRet.
 *
 * \param cond          A boolean expression to be asserted true
 * \param altRet        The value to be returned if cond doesn't hold true
 * \param str           A quoted char string
 *
 * E.g.:
 *      SEC_ASSERT(ret > 0, 0, "ERROR initializing app: code = %d\n", ret);
 */
#define SEC_ASSERT(cond, altRet, str, ...) \
    if (unlikely(!(cond))) {\
        SEC_ERROR(str ,## __VA_ARGS__); \
        return (altRet); \
    }

/**
 * Assert that cond is true. If !cond is true, display str and the vararg list in a printf-like syntax.
 * also, if !cond is true, return.
 *
 * \param cond          A boolean expression to be asserted true
 * \param str           A quoted char string
 *
 * E.g.:
 *      SEC_ASSERT(ret > 0, "ERROR initializing app: code = %d\n", ret);
 */
#define SEC_ASSERT_RET_VOID(cond, str, ...) \
    if (unlikely(!(cond))) {\
        SEC_ERROR(str ,## __VA_ARGS__); \
        return ; \
    }


/**
 * Check condition (possibly print error message) and continue execution.
 * Same as ##SEC_ASSERT(), only without the return instruction.
 */
#define SEC_ASSERT_CONT(cond, str, ...) \
    if (unlikely(!(cond))) {\
        SEC_ERROR(str ,## __VA_ARGS__); \
    }

/**
 * Check condition (possibly print error message) and stop execution.
 * Same as ##SEC_ASSERT(), only without the return instruction.
 */
#define SEC_ASSERT_STOP(cond, str, ...) \
    if (unlikely(!(cond))) {\
        SEC_ERROR(str ,## __VA_ARGS__); \
        ASSERT(cond); \
    }



/** shortcut for __attribute__ ((packed)) */
#define PACKED __attribute__ ((packed))

/** compute offset for structure member B in structure A */
#ifndef offsetof
#ifdef __GNUC__
#define offsetof(a,b)           __builtin_offsetof(a,b)
#else
#define offsetof(type,member)   ((size_t) &((type*)0)->member)
#endif
#endif

/** Get a pointer to beginning of structure which contains <member> */
#define container_of(ptr, type, member) ({            \
 const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
  (type *)( (char *)__mptr - offsetof(type,member) );})

/** counts the number of elements in an array */
#ifndef countof
 #define countof(A) ((sizeof(A)/sizeof(A[0])))
#endif

/** Compile time assert.
 * This doesn't compile if a is false. Good to compile-time check things like
 * struct sizes
 */
#define CTASSERT(a) extern char __dummy[(a)?1:-1];

/** ASSERT definition */
#define ASSERT(x)   assert(x)

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#define __always_unused __attribute__((unused))


/*==============================================================================
                                    ENUMS
==============================================================================*/

/*==============================================================================
                         STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/

/*==============================================================================
                                 CONSTANTS
==============================================================================*/


/*==============================================================================
                         GLOBAL VARIABLE DECLARATIONS
==============================================================================*/


/*==============================================================================
                            FUNCTION PROTOTYPES
==============================================================================*/

/** Write 32bit values */
static inline void out_be32(volatile uint32_t *addr, uint32_t val) {
	*addr = val;
}

/** Read 32bit values */
static inline uint32_t in_be32(const volatile uint32_t *addr) {
	return *addr;
}
/*============================================================================*/


#endif  /* SEC_UTILS_H */
