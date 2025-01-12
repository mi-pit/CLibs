//
// Created by MacBook on 01.11.2024.
//

#ifndef CLIBS_POINTER_UTILS_H
#define CLIBS_POINTER_UTILS_H

#include "attributes.h" /* UsageOptional */

#include <stdarg.h> /* va_* */
#include <stdlib.h> /* free(), NULL */


#ifndef __cplusplus
#define new( TYPE ) ( calloc( 1, sizeof( TYPE ) ) )
#endif

#define free_n( ptr ) free_and_null( ( void ** ) &( ptr ) )


LibraryDefined void free_all( int n, ... )
{
    va_list vaList;
    va_start( vaList, n );
    while ( n-- > 0 )
        free( va_arg( vaList, void * ) );

    va_end( vaList );
}

LibraryDefined inline void free_and_null( void **n )
{
    free( *n );
    *n = NULL;
}


#define deref_as( type, var )                ( *( type * ) ( var ) )
#define deref_as_offset( type, var, offset ) ( *( ( type * ) ( var ) ) + offset )


#endif //CLIBS_POINTER_UTILS_H
