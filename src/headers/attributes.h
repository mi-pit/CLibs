/**
 * @file attributes.h
 * @brief
 * Function and variable attributes.
 *
 * Whether the compiler supports attributes (GCC/Clang) or not,
 * this header creates platform-independent macros for certain attributes
 * a function or variable may have.
 *
 * All of the macros defined in this header are safe to use on any compiler
 * -- including ones that don't support attributes
 * (in that case they are preprocessed as empty macros).
 *
 * Some combinations may (definitely will) be missing; this isn't an exhaustive list.
 */

// Created by MacBook on 10.01.2025.

#ifndef CLIBS_ATTRIBUTES_H
#define CLIBS_ATTRIBUTES_H

#include "stdc_versions.h" /* STANDARD_C23_VERSION */


#ifdef __has_attribute
/** Evaluates as true if the compiler (clang/GCC) supports the attribute */
#define HAS_ATTRIBUTE( TOK ) __has_attribute( TOK )
#else // ndef __has_attribute
/** This compiler doesn't have any attributes */
#define HAS_ATTRIBUTE( TOK ) ( 0 )
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
/// Tells the compiler which varargs correspond to a format string
#define ScanfLike( FORMAT_STRING, FIRST_VAR_ARG ) \
    __attribute__( ( __format__( __scanf__, FORMAT_STRING, FIRST_VAR_ARG ) ) )
#else // format
#define PrintfLike( FORMAT_STRING, FIRST_VAR_ARG )
#define ScanfLike( FORMAT_STRING, FIRST_VAR_ARG )
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
/** Functions return value only depends on its parameters */
#define Const __attribute__( ( __const__ ) )
#else // const
#define Const
#endif // const


#if HAS_ATTRIBUTE( constructor )
/// Function runs before entering `main`
#define BeforeMain __attribute__( ( constructor ) )
#else
#define BeforeMain UsageOptional
#endif // constructor


#if HAS_ATTRIBUTE( noreturn )
/// Function always exits
#define NoReturn __attribute__( ( noreturn ) )
#else
#define NoReturn
#endif // constructor


#if HAS_ATTRIBUTE( deprecated )
/// Function is deprecated and shouldn't be used
#define Deprecated __attribute__( ( deprecated ) )
#else
#define Deprecated
#endif


/**
 * Function allocates memory for an object.
 * Not using function's return value results in a memory leak (or worse).
 */
#define Constructor UseResult

/** Function or variable is not visible outside the current file */
#define Private static

/** Function or variable is defined in a header file */
#define LibraryDefined UsageOptional static

/**
 * Function has no effect except for the return value.
 * Function's return value only depends on its parameters.
 */
#define Mathematical Const


#if HAS_ATTRIBUTE( nonnull )
/** Must not be NULL */
#define NonNullParams( ... ) __attribute( ( nonnull( __VA_ARGS__ ) ) )
#else
#define NonNullParams
#endif


#endif //CLIBS_ATTRIBUTES_H
