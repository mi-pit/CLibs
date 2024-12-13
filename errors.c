//
// Created by MacBook on 30.10.2024.
//
#include "errors.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


int WarnUniversal( const char *restrict FileName,
                   const char *restrict FunctionName,
                   int LineNumber,
                   int err_no,
                   int ret_val,
                   const char *restrict format,
                   ... )
{
    fprintf( stderr, "%s", getprogname() );
    if ( FileName != NULL )
        fprintf( stderr, ": %s", FileName );
    if ( FunctionName != NULL )
        fprintf( stderr, ": %s", FunctionName );
    if ( LineNumber > 0 )
        fprintf( stderr, "@ %i", LineNumber );
    fprintf( stderr, ": " );

    va_list vaList;
    va_start( vaList, format );
    vfprintf( stderr, format, vaList );
    va_end( vaList );

    if ( err_no >= 0 )
        fprintf( stderr, ": %s", strerror( err_no ) );
    fprintf( stderr, "\n" );

    return ret_val;
}
