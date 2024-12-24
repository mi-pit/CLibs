//
// Created by MacBook on 30.10.2024.
//
#include "errors.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined( __APPLE__ ) || defined( __FreeBSD__ )
#define get_prog_name() getprogname()
#elif defined( __FILE_NAME__ )
#define get_prog_name() __FILE_NAME__
#else
#define get_prog_name() "current-program"
#endif


ptrdiff_t WarnUniversal( const char *restrict FileName,
                         const char *restrict FunctionName,
                         int LineNumber,
                         int err_no,
                         ptrdiff_t ret_val,
                         const char *restrict format,
                         ... )
{
    fprintf( stderr, "%s", get_prog_name() );
    if ( FileName != NULL )
        fprintf( stderr, ": %s", FileName );
    if ( FunctionName != NULL )
        fprintf( stderr, ": %s", FunctionName );
    if ( LineNumber > 0 )
        fprintf( stderr, " @ %i", LineNumber );
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
