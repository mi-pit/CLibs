/*
 * Utility functions and macros for working with pointers.
 *
 *
 * Created by MacBook on 01.11.2024.
 */

#ifndef CLIBS_ALLOC_H
#define CLIBS_ALLOC_H

#include "../../core/attributes.h" /* LibraryDefined */

#include <stdarg.h> /* va_* */
#include <stdlib.h> /* free(), NULL */


/// Allocates memory for one (1) object of the specified type
#define new( TYPE ) calloc( 1, sizeof( TYPE ) )

/// Frees the pointer and sets `ptr` to `NULL`
#define free_n( ptr ) free_and_null( ( void ** ) &( ptr ) )


/**
 * Frees `n` pointers by calling `free` on each.
 *
 * @param n     number of pointers
 * @param ...   pointers
 */
LibraryDefined void free_all( int n, ... )
{
    va_list vaList;
    va_start( vaList, n );
    while ( n-- > 0 )
        free( va_arg( vaList, void * ) );

    va_end( vaList );
}

/**
 * Does `free(*n)` and sets `*n = NULL`
 *
 * @param n pointer to pointer
 */
LibraryDefined inline void free_and_null( void **n )
{
    free( *n );
    *n = NULL;
}


#endif //CLIBS_ALLOC_H
