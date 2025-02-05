//
// Created by MacBook on 10.01.2025.
//

#ifndef CLIBS_ATTRIBUTES_H
#define CLIBS_ATTRIBUTES_H

#include <sys/cdefs.h>

#ifdef __has_attribute
#define HAS_ATTRIBUTE( TOK ) __has_attribute( TOK )
#else // ndef __has_attribute
/* false */
#define HAS_ATTRIBUTE( TOK ) 0
#endif // __has_attribute


#if HAS_ATTRIBUTE( unused )
/// Stops compiler warnings when the function/variable is unused
#define UsageOptional __attribute__( ( __unused__ ) )
#else // unused
#define UsageOptional
#endif // unused

#if HAS_ATTRIBUTE( format )
/// Tells the compiler which varargs correspond to a format string
#define PrintfLike( FORMAT_STRING, FIRST_VAR_ARG ) \
    __attribute__( ( __format__( __printf__, FORMAT_STRING, FIRST_VAR_ARG ) ) )
#else // format
#define PrintfLike( FORMAT_STRING, FIRST_VAR_ARG )
#endif // format


#if HAS_ATTRIBUTE( warn_unused_result )
/// Function's return value must be used.
#define UseResult __attribute__( ( __warn_unused_result__ ) )
#else // warn_unused_result
#define UseResult
#endif // warn_unused_result


#if HAS_ATTRIBUTE( cold )
#define Cold __attribute__( ( __cold__ ) )
#else // cold
#define Cold
#endif // cold

#if HAS_ATTRIBUTE( const )
#define Const __attribute__( ( __const__ ) )
#else // const
#define Const
#endif // const

#if HAS_ATTRIBUTE( constructor )
#define BeforeMain __attribute__( ( constructor ) )
#else
#define BeforeMain UsageOptional
#endif // constructor

#if HAS_ATTRIBUTE( noreturn )
#define NoReturn __attribute__( ( noreturn ) )
#else
#define NoReturn
#endif // constructor

/**
 * Function allocates memory for an object.
 * Not using function's return value results in a memory leak (or worse).
 */
#define Constructor UseResult

/** Function is not visible outside of the current file */
#define Private static

/** Function is defined in a header file */
#define LibraryDefined static UsageOptional

/**
 * Function has no effect except for the return value.
 * Function's return value only depends on its parameters.
 */
#define Mathematical Const

#endif //CLIBS_ATTRIBUTES_H
