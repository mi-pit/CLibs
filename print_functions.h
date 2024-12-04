#ifndef CLIBS_PRINT_FUNCTIONS_H
#define CLIBS_PRINT_FUNCTIONS_H

#include "misc.h"

#include <stdarg.h>
#include <stddef.h>


#ifndef LINE_DELIM
#define LINE_DELIM "\n"
#endif


#define printfln( fmt, ... ) printf( fmt LINE_DELIM, __VA_ARGS__ )
#define println( fmt, ... )  printf( fmt LINE_DELIM )


typedef void ( *PrintFunction )( const void *, size_t );


#ifndef TYPE_SEPARATOR
#define TYPE_SEPARATOR ": "
#endif //TYPE_SEPARATOR


#define print( type ) print_##type

#define declare_print_func( name ) void print( name )( const void *, size_t )

/* Default */
declare_print_func( byte );

/* Basic types */
declare_print_func( int );
declare_print_func( size_t );
declare_print_func( ssize_t );
declare_print_func( int8_t );
declare_print_func( char );
declare_print_func( float );
declare_print_func( double );
declare_print_func( pointer );
declare_print_func( ptr );

/* Special */
declare_print_func( string ); /* These two      */
declare_print_func( str );    /* are equivalent */

declare_print_func( List );
declare_print_func( Set );
declare_print_func( Dict );

#ifndef REQUIRE_SEMICOLON
#define REQUIRE_SEMICOLON struct DECLARATION_MACRO_UNUSED_STRUCT
#endif//REQUIRE_SEMICOLON

#define define_print_func( type, format_str )                                    \
    void print( type )( const void *data, size_t nbytes )                        \
    {                                                                            \
        if ( nbytes != sizeof( type ) )                                          \
        {                                                                        \
            printf( "%s: invalid data", __func__ );                              \
            return;                                                              \
        }                                                                        \
        printf( "'" #type TYPE_SEPARATOR format_str "'", *( ( type * ) data ) ); \
    }                                                                            \
    REQUIRE_SEMICOLON

#endif //CLIBS_PRINT_FUNCTIONS_H
