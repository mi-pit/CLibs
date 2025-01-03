#include "item_print_functions.h"

#include "errors.h"
#include "extra_types.h"
#include "pointer_utils.h"

#include <err.h>
#include <stdio.h>
#include <string.h>


void print_byte( const void *data, size_t nbytes )
{
    printf( "'" );
    for ( size_t i = 0; i < nbytes; ++i )
    {
        if ( i != 0 )
            printf( " " );
        byte n = deref_as_offset( byte, data, i );
        printf( "%02x", n );
    }
    printf( "'" );
}

define_print_func( int, "%-d" );

define_print_func( size_t, "%-zu" );

define_print_func( ssize_t, "%- zd" );

define_print_func( char, "'%c'" );

define_print_func( float, "%.2f" );

define_print_func( double, "%.4f" );

define_print_func( int8_t, "%3d" );


void print_pointer( const void *data, size_t nbytes )
{
    if ( nbytes != sizeof( void * ) )
    {
        fflwarn( "invalid data size" );
        return;
    }

    printf( "'pointer: \"%p\"'", *( ( void ** ) data ) );
}

void print_string( const void *data, size_t nbytes )
{
    if ( nbytes != strlen( data ) )
    {
        warnx( "%s: strlen != nbytes", __func__ );
        return;
    }
    printf( "\"%s\"", ( char * ) data );
}

void print_str( const void *data, size_t nbytes )
{
    print_string( data, nbytes );
}


#ifdef CLIBS_DYNAMIC_ARRAY_H
void print_List( const void *data, size_t nbytes )
{
    if ( nbytes != sizeof( List ) )
    {
        printf( "%s: invalid data", __func__ );
        return;
    }
    ConstList ls = *( List * ) data;
    list_print( ls );
}
#endif //CLIBS_DYNAMIC_ARRAY_H

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
