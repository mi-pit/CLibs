#ifndef CLIBS_PRINT_FUNCTIONS_H
#define CLIBS_PRINT_FUNCTIONS_H

#include "extra_types.h" /* byte */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>


#ifndef LINE_DELIM
#define LINE_DELIM "\n"
#endif


typedef void ( *PrintFunction )( const void *, size_t );

static bool PrintType = false;

#ifndef TYPE_SEPARATOR
#define TYPE_SEPARATOR ": "
#endif //TYPE_SEPARATOR

#define item_print_function( type ) print_##type
#define print_function( type )      item_print_function( type )

#define declare_print_func( name ) void print_function( name )( const void *, size_t )

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
#endif //REQUIRE_SEMICOLON

#define define_print_func( type, format_str )                      \
    void print_function( type )( const void *data, size_t nbytes ) \
    {                                                              \
        if ( nbytes != sizeof( type ) )                            \
        {                                                          \
            fprintf( stderr, "%s: invalid data", __func__ );       \
            return;                                                \
        }                                                          \
                                                                   \
        if ( PrintType )                                           \
        {                                                          \
            printf( "‹" );                                         \
            printf( #type TYPE_SEPARATOR );                        \
        }                                                          \
        printf( format_str, *( ( type * ) data ) );                \
        if ( PrintType )                                           \
            printf( "›" );                                         \
    }                                                              \
    REQUIRE_SEMICOLON

#endif //CLIBS_PRINT_FUNCTIONS_H
