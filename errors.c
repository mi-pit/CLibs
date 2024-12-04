//
// Created by MacBook on 30.10.2024.
//
#include "errors.h"

#include <stdarg.h>

int warn_ret( int rv, const char *restrict fmt, ... )
{
    va_list vaList;
    va_start( vaList, fmt );
    vwarn( fmt, vaList );
    va_end( vaList );

    return rv;
}

int warnx_ret( int rv, const char *restrict fmt, ... )
{
    va_list vaList;
    va_start( vaList, fmt );
    vwarnx( fmt, vaList );
    va_end( vaList );

    return rv;
}


void *warn_ret_p( const char *restrict fmt, ... )
{
    va_list vaList;
    va_start( vaList, fmt );
    vwarn( fmt, vaList );
    va_end( vaList );

    return NULL;
}

void *warnx_ret_p( const char *restrict fmt, ... )
{
    va_list vaList;
    va_start( vaList, fmt );
    vwarnx( fmt, vaList );
    va_end( vaList );

    return NULL;
}
