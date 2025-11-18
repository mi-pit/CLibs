#include "item_print_functions.h"

#include "headers/errors.h"
#include "headers/pointer_utils.h"
#include "headers/types.h"

#include <err.h>
#include <stdio.h>
#include <string.h>


void ITEM_PRINT_FUNCTION_NAME( byte )( const void *data, const size_t nbytes )
{
    printf( "'" );
    for ( size_t i = 0; i < nbytes; ++i )
    {
        if ( i != 0 )
            printf( " " );
        const byte n = deref_as_offset( byte, data, i );
        printf( "%02x", n );
    }
    printf( "'" );
}

void ITEM_PRINT_FUNCTION_NAME( bool )( const void *data, const size_t nbytes )
{
    if ( nbytes != sizeof( bool ) )
    {
        fwarnx( "invalid data" );
        return;
    }

    printf( "%s", deref_as( bool, data ) ? "true" : "false" );
}

DEFINE_PRINT_FUNC( int, "%-d" )

DEFINE_PRINT_FUNC( size_t, "%-zu" )

DEFINE_PRINT_FUNC( ssize_t, "%-zd" )

DEFINE_PRINT_FUNC( char, "'%c'" )

DEFINE_PRINT_FUNC( float, "%.2f" )

DEFINE_PRINT_FUNC( double, "%.4f" )

DEFINE_PRINT_FUNC( int8_t, "%3d" )


void ITEM_PRINT_FUNCTION_NAME( pointer )( const void *data, const size_t nbytes )
{
    if ( nbytes != sizeof( void * ) )
    {
        fflwarn( "invalid data size" );
        return;
    }

    printf( "\"%p\"", *( void ** ) data );
}

void ITEM_PRINT_FUNCTION_NAME( string )( const void *data, const size_t nbytes )
{
    if ( nbytes != strlen( data ) )
    {
        fwarnx( "strlen != nbytes" );
        return;
    }
    printf( "\"%s\"", deref_as( const char *, data ) );
}

void ITEM_PRINT_FUNCTION_NAME( string_direct )( const void *data, const size_t nbytes )
{
    printf( "\"%.*s\"", ( int ) nbytes, ( const char * ) data );
}


#ifdef CLIBS_SETS_H
void print_Set( const void *data, size_t nbytes )
{
    if ( nbytes != sizeof( Set ) )
    {
        printf( "%s: invalid data", __func__ );
        return;
    }
    ConstSet s = *( Set * ) data;
    set_print( s );
}
#endif //CLIBS_SETS_H

#ifdef CLIBS_DICTIONARY_H
void print_Dict( const void *data, size_t nbytes )
{
    if ( nbytes != sizeof( Dict ) )
    {
        printf( "%s: invalid data", __func__ );
        return;
    }
    ConstDict dict = *( Dict * ) data;
    dict_print( dict );
}
#endif //CLIBS_DICTIONARY_H
