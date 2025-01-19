//
// Created by MacBook on 06.01.2025.
//

// tests

#include "test_dynstr.h"
#include "test_foreach.h"
#include "test_list.h"
#include "test_misc_c.h"
#include "test_string_utils.h"
#include "test_swex.h"


void run_list( void )
{
    RUN_TEST( list_init );
    RUN_TEST( list_basic );
}

void run_dynstr( void )
{
    RUN_TEST( init_as );
    RUN_TEST( slices );
    RUN_TEST( append );
    RUN_TEST( appendn );
    RUN_TEST( appendf );
}

void run_string_utils( void )
{
    RUN_TEST( replace );
    RUN_TEST( escape );
    RUN_TEST( strspl_str );
    RUN_TEST( strspl_regex );
    RUN_TEST( string_to_UL );
    RUN_TEST( string_as_UL );
    RUN_TEST( reverse_str );
    RUN_TEST( strip );
    RUN_TEST( strings_misc );
}

void run_foreach( void )
{
    RUN_TEST( foreach );
}

void run_swex( void )
{
    RUN_TEST( swex );
}

void run_misc_c( void )
{
    RUN_TEST( misc_c );
}

LibraryDefined TEST( array_sprintf )
{
    int array[ 10 ];
    for ( size_t i = 0; i < countof( array ); ++i )
        array[ i ] = ( int ) ( i * i ) * ( i % 2 == 0 ? -1 : 1 );

    void *items_gen = array;

    str_t str, snd;
    array_sprintf_d( str, array, countof( array ), int, "%i", ", " );
    array_sprintf( snd, array, countof( array ), int, "%i" );
    PrintInColor( stdout, FOREGROUND_RED, "%s\n", str );

    UNIT_TEST( cmpeq( strcmp( str, "[ 0, 1, -4, 9, -16, 25, -36, 49, -64, 81 ]" ) ) );
    UNIT_TEST( cmpeq( strcmp( str, snd ) ) );
    free( str );
    free( snd );

    array_sprintf_d( str, items_gen, countof( array ), int, "%+02i", " | " );
    PrintInColor( stdout, FOREGROUND_RED, "%s\n", str );

    UNIT_TEST( cmpeq( strcmp(
            str, "[ +0 | +1 | -4 | +9 | -16 | +25 | -36 | +49 | -64 | +81 ]" ) ) );
    free( str );
}
END_TEST

int main( void )
{
    run_misc_c();
    run_foreach();
    run_swex();
    run_string_utils();
    run_dynstr();
    run_list();

    RUN_TEST( array_sprintf );

    FINISH_TESTING();
}
