//
// Created by MacBook on 25.12.2024.
//

#include "print_functions.h"

int PrintLNUniversal( FILE *__restrict file, const char *__restrict format, ... )
{
    va_list vaList;
    va_start( vaList, format );
    int rv = vfprintf( file, format, vaList );
    va_end( vaList );

    if ( rv < 0 )
        return rv;

    rv += fprintf( file, "\n" );

    return rv;
}
