//
// Created by Michal Pitner on 18.06.2025.
//

#ifndef TEST_ARRAYPRINTF_H
#define TEST_ARRAYPRINTF_H

#include "../../src/headers/array_printf.h"
#include "../../src/headers/misc.h"
#include "../../src/headers/unit_tests.h"
#include "../../src/string_utils.h"
#include "../../src/structs/dynstring.h"

TEST( array_sprintf )
{
    int array[ 10 ];
    for ( size_t i = 0; i < countof( array ); ++i )
        array[ i ] = ( int ) ( i * i ) * ( i % 2 == 0 ? -1 : 1 );

    str_t str, snd;
    array_sprintf_d( str, array, countof( array ), int, "%i", ", " );
    array_sprintf( snd, array, countof( array ), int, "%i" );
    PrintInColor( stdout, BACKGROUND_YELLOW, "%s\n", str );

    UNIT_TEST( cmpeq( strcmp( str, "[ 0, 1, -4, 9, -16, 25, -36, 49, -64, 81 ]" ) ) );
    UNIT_TEST( cmpeq( strcmp( str, snd ) ) );
    free( str );
    free( snd );

    void *items_gen = array;
    array_sprintf_d( str, items_gen, countof( array ), int, "%+02i", " | " );
    PrintInColor( stdout, BACKGROUND_YELLOW, "%s\n", str );

    UNIT_TEST( cmpeq( strcmp(
            str, "[ +0 | +1 | -4 | +9 | -16 | +25 | -36 | +49 | -64 | +81 ]" ) ) );
    free( str );
}
END_TEST

LibraryDefined void RUNALL_ARRAYPRINTF( void )
{
    RUN_TEST( array_sprintf );
}

#endif //TEST_ARRAYPRINTF_H
