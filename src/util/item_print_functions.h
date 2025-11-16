/*
 * Functions for printing simple data.
 *
 * Comes with the most common types pre-defined, and macros to define more.
 */

#ifndef CLIBS_PRINT_FUNCTIONS_H
#define CLIBS_PRINT_FUNCTIONS_H

#include "../headers/core/attributes.h"
#include "../headers/errors.h"

#include <stdbool.h>


/// Interface for functions that print data
typedef void ( *PrintFunction )( const void *, size_t );

/// If true, the functions defined by `DEFINE_PRINT_FUNC` also print the type name
LibraryDefined bool PrintFunctions_DoPrintType = false;

#ifndef TYPE_SEPARATOR
/// String which separates the type name (see `PrintFunctions_DoPrintType`) from the value
#define TYPE_SEPARATOR ": "
#endif //TYPE_SEPARATOR


/// Creates a function name token from the `type` (must be a single token)
#define ITEM_PRINT_FUNCTION_NAME( type ) Print_##type

/// Declares a `PrintFunction`
#define DECLARE_PRINT_FUNC( name ) \
    void ITEM_PRINT_FUNCTION_NAME( name )( const void *, size_t )

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


/**
 * Defines a `PrintFunction`.
 *
 * This function checks whether data size equals `sizeof( type )`
 * and prints the data as `type`, according to the `format_str`ing.
 */
#define DEFINE_PRINT_FUNC( type, format_str )                                      \
    void ITEM_PRINT_FUNCTION_NAME( type )( const void *data, const size_t nbytes ) \
    {                                                                              \
        if ( nbytes != sizeof( type ) )                                            \
        {                                                                          \
            fflwarnx( "invalid data" );                                            \
            return;                                                                \
        }                                                                          \
                                                                                   \
        if ( PrintFunctions_DoPrintType )                                          \
        {                                                                          \
            printf( "‹" );                                                         \
            printf( #type TYPE_SEPARATOR );                                        \
        }                                                                          \
        printf( format_str, *( ( type * ) data ) );                                \
        if ( PrintFunctions_DoPrintType )                                          \
            printf( "›" );                                                         \
    }

#endif //CLIBS_PRINT_FUNCTIONS_H
