//
// Created by MacBook on 01.11.2024.
//

#include "pointer_utils.h"

#include <stdarg.h>
#include <stdlib.h>

void free_all( int n, ... )
{
    va_list vaList;
    va_start( vaList, n );
    while ( n-- > 0 )
        free( va_arg( vaList, void * ) );

    va_end( vaList );
}

void free_and_null( void **n )
{
    free( *n );
    *n = NULL;
}
