//
// Created by Michal Pitner on 16.11.2025.
//

#ifndef CLIBS_STRDUP_H
#define CLIBS_STRDUP_H

#if __STDC_VERSION__ < 202311L && !defined( _POSIX_C_SOURCE )
/*
 * Define
 * strdup && strndup
 */

#include <stdlib.h> /* malloc */

#define strndup string_duplicate_l
#define strdup  string_duplicate

/// `strndup` implementation (standard in POSIX and C23+)
LibraryDefined UseResult str_t string_duplicate_l( string_t s, size_t l )
{
    const str_t n = malloc( l + 1 );
    if ( n == NULL )
        return NULL;

    strncpy( n, s, l );
    n[ l ] = '\0';
    return n;
}

/// `strdup` implementation (standard in POSIX and C23+)
LibraryDefined UseResult str_t string_duplicate( string_t s )
{
    const size_t l = strlen( s );
    const str_t n  = malloc( l + 1 );
    if ( n == NULL )
        return NULL;

    n[ l ] = '\0';
    strcpy( n, s );
    return n;
}
#else // C23 or posix

#include <string.h> /* string.h has strdup here */

#endif

#endif //CLIBS_STRDUP_H
