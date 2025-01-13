//
// Created by MacBook on 10.01.2025.
//

#ifndef CLIBS_ATTRIBUTES_H
#define CLIBS_ATTRIBUTES_H

#include <sys/cdefs.h>

#ifdef __has_attribute
#define HAS_ATTRIBUTE( TOK ) __has_attribute( TOK )
#else //!def __has_attribute
/* false */
#define HAS_ATTRIBUTE( TOK ) 0
#endif //__has_attribute

#if HAS_ATTRIBUTE( unused )
/// Stops compiler warnings when the function/variable is unused
#define UsageOptional __attribute__( ( __unused__ ) )
#else
#define UsageOptional
#endif

#if HAS_ATTRIBUTE( format )
/// Tells the compiler which varargs correspond to a format string
#define PrintfLike( FORMAT_STRING, FIRST_VAR_ARG ) \
    __attribute__( ( __format__( __printf__, FORMAT_STRING, FIRST_VAR_ARG ) ) )
#else
#define PrintfLike( FORMAT_STRING, FIRST_VAR_ARG )
#endif


#if HAS_ATTRIBUTE( warn_unused_result )
/// Function's return value must be used.
#define UseResult __attribute__( ( __warn_unused_result__ ) )

/**
 * Function allocates memory for an object.
 * Function's return value must be used.
 */
#define Constructor UseResult
#else
#define UseResult
#define Constructor
#endif


#define Private static UsageOptional

/// The function is static and it may go unused
#define LibraryDefined static UsageOptional


#endif //CLIBS_ATTRIBUTES_H
