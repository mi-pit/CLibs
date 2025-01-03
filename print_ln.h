//
// Created by MacBook on 25.12.2024.
//

#ifndef CLIBS_PRINT_LN_H
#define CLIBS_PRINT_LN_H

#include <stdarg.h>
#include <stdio.h>

static int PrintLNUniversal( FILE *__restrict, const char *__restrict format, ... )
        __printflike( 2, 3 );

static int PrintLNUniversal( FILE *__restrict file, const char *__restrict format, ... )
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

#define printf_ln( ... )                 PrintLNUniversal( stdout, __VA_ARGS__ )
#define fprintf_ln( FILE_VAR_NAME, ... ) PrintLNUniversal( FILE_VAR_NAME, __VA_ARGS__ )

#endif //CLIBS_PRINT_LN_H
