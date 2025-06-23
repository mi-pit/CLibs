/*
 * Functions for printing simple data.
 *
 * Comes with the most common types pre-defined, and macros to define more.
 */

#ifndef CLIBS_PRINT_FUNCTIONS_H
#define CLIBS_PRINT_FUNCTIONS_H

#include "headers/attributes.h"
#include "headers/errors.h"

#include <stdbool.h>


typedef void ( *PrintFunction )( const void *, size_t );


LibraryDefined bool PrintType = false;

#ifndef TYPE_SEPARATOR
#define TYPE_SEPARATOR ": "
#endif //TYPE_SEPARATOR


#define item_print_function( type ) print_##type
#define print_function( type )      item_print_function( type )

#define DECLARE_PRINT_FUNC( name ) void print_function( name )( const void *, size_t )

/* Default */
DECLARE_PRINT_FUNC( byte );

/* Basic types */
DECLARE_PRINT_FUNC( bool );
DECLARE_PRINT_FUNC( int );
DECLARE_PRINT_FUNC( size_t );
DECLARE_PRINT_FUNC( ssize_t );
DECLARE_PRINT_FUNC( int8_t );
DECLARE_PRINT_FUNC( char );
DECLARE_PRINT_FUNC( float );
DECLARE_PRINT_FUNC( double );
DECLARE_PRINT_FUNC( pointer );

/* Special */
DECLARE_PRINT_FUNC( string );
DECLARE_PRINT_FUNC( string_direct );

DECLARE_PRINT_FUNC( Set );
DECLARE_PRINT_FUNC( Dict );

#ifndef REQUIRE_SEMICOLON
#define REQUIRE_SEMICOLON struct DECLARATION_MACRO_UNUSED_STRUCT
#endif //REQUIRE_SEMICOLON

#define DEFINE_PRINT_FUNC( type, format_str )                            \
    void print_function( type )( const void *data, const size_t nbytes ) \
    {                                                                    \
        if ( nbytes != sizeof( type ) )                                  \
        {                                                                \
            fwarnx( "invalid data" );                                    \
            return;                                                      \
        }                                                                \
                                                                         \
        if ( PrintType )                                                 \
        {                                                                \
            printf( "‹" );                                               \
            printf( #type TYPE_SEPARATOR );                              \
        }                                                                \
        printf( format_str, *( ( type * ) data ) );                      \
        if ( PrintType )                                                 \
            printf( "›" );                                               \
    }                                                                    \
    REQUIRE_SEMICOLON

#endif //CLIBS_PRINT_FUNCTIONS_H
