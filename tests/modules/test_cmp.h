//
// Created by Michal Pitner on 18.11.2025.
//

#ifndef CLIBS_TEST_CMP_H
#define CLIBS_TEST_CMP_H

#include "../../src/headers/unit_tests.h"
#include "../../src/headers/util/cmp.h"
#include "../../src/headers/util/hash.h"

#define TEST_CMP_CURRENT_TYPE unsigned int

DEFINE_CMP_FUNCTION( unsigned )

static TEST( intcmp )
{
    TEST_CMP_CURRENT_TYPE array[ 256 ];
    for ( size_t i = 0; i < countof( array ); ++i )
        array[ i ] = hash_func( &i, sizeof i );

    qsort( array, countof( array ), sizeof( TEST_CMP_CURRENT_TYPE ), cmp_unsigned );
    // array_printf( array, countof( array ), TEST_CMP_CURRENT_TYPE, "%u" );

    for ( size_t i = 0; i < countof( array ) - 1; ++i )
        CRITICAL_TEST( array[ i ] <= array[ i + 1 ] );
}
END_TEST

void RUNALL_CMP( void )
{
    RUN_TEST( intcmp );
}


#endif //CLIBS_TEST_CMP_H
