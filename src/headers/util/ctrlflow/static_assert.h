/**
 * @file static_assert.h
 * @brief Static assert macro for C99 compatibility
 *
 * Makes it so that the you can use static asserts in some way in any std C version.
 *
 * If the version of C is below C11 and the compiler supports function attributes,
 * the macro expands to a function which asserts the condition
 * at the start of the execution of the program (not actually static).
 */

// Created by Michal Pitner on 07.10.2025.


#ifndef STATIC_ASSERT_H
#define STATIC_ASSERT_H

#include "../../core/stdc_versions.h"

#if __STDC_VERSION__ >= STANDARD_C11_VERSION
#define STATIC_ASSERT( CONDITION, MESSAGE ) _Static_assert( CONDITION, MESSAGE )

#elif HAS_ATTRIBUTE( constructor )
#include "../../core/attributes.h" /* BeforeMain */
#include "../../core/errors.h"     /* xwarnx */

#define CONCAT2( a, b ) a##b
#define CONCAT( a, b )  CONCAT2( a, b )

#define UNIQUE_NAME( base ) CONCAT( base, __COUNTER__ )

#define FOREGROUND_BWHITE_BOLD COLOR_CREATOR( "1;97" )

/// Asserts condition before entering `main`
#define STATIC_ASSERT( CONDITION, MESSAGE )                                             \
    static /* :D */ BeforeMain void UNIQUE_NAME( STATIC_ASSERT__ )( void )              \
    {                                                                                   \
        if ( !( CONDITION ) )                                                           \
        {                                                                               \
            xwarnx( "'static' assertion failed\n"                                       \
                    "\toffending expression: " COLOR_DEFAULT " `%s`" COLOR_WARNING "\n" \
                    "\twith message " FOREGROUND_BWHITE_BOLD "\"%s\"",                  \
                    #CONDITION, ( MESSAGE ) );                                          \
            abort();                                                                    \
        }                                                                               \
    }                                                                                   \
    /* The `struct` is there so that there is no */                                     \
    /* trailing semicolon after use. */                                                 \
    struct UNUSED_STATIC_ASSERT_STRUCT

#else // C level < 11
/// Does nothing
#define STATIC_ASSERT( CONDITION, MESSAGE ) struct UNUSED_STATIC_ASSERT_STRUCT

#endif //C11

STATIC_ASSERT( 1 == 1, "Example" );

#endif //STATIC_ASSERT_H
