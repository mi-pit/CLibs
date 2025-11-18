/**
 * @file asprintf.h
 * @brief `sprintf(3)` which also `malloc`s the memory for the string.
 */

// Created by Michal Pitner on 16.11.2025.


#ifndef CLIBS_ASPRINTF_H
#define CLIBS_ASPRINTF_H

#include "../../core/attributes.h"


#if ( !defined( _GNU_SOURCE ) && !defined( __APPLE__ ) ) || defined( _POSIX_C_SOURCE )
// non-standard

/*
 * Define
 * asprintf && vasprintf
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef vsnprintf
/// `va_list` version of `snprintf()`
int vsnprintf( char *str, size_t size, const char *restrict format, va_list ap );
#endif

/**
 * Like `vsprintf()`, except it heap-allocates memory for the resulting string.
 * `*strp` may be passed to `free(3)`
 *
 * @param strp container for the new string
 * @param fmt  printf-like format string
 * @param args va_list of printf args
 * @return `vsnprintf(3)` return value or -1 if `malloc(3)` fails
 */
LibraryDefined int vasprintf( char **strp, const char *const fmt, va_list args )
{
    va_list vaList;
    va_copy( vaList, args );

    int size = vsnprintf( NULL, ( size_t ) 0, fmt, args );

    if ( size < 0 )
        return size;

    *strp = malloc( size + 1 );
    if ( !*strp )
        return -1;

    const int result = vsnprintf( *strp, ( size_t ) size + 1, fmt, vaList );
    va_end( args );

    return result;
}

/**
 * Like `sprintf()`, except it heap-allocates memory for the resulting string.
 * `*strp` may be passed to `free(3)` *
 */
LibraryDefined PrintfLike( 2, 3 ) int asprintf( char **strp, const char *const fmt, ... )
{
    va_list va;
    va_start( va, fmt );
    const int rv = vasprintf( strp, fmt, va );
    va_end( va );
    return rv;
}
#endif


#endif //CLIBS_ASPRINTF_H
