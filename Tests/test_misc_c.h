//
// Created by MacBook on 08.01.2025.
//

#ifndef CLIBS_TEST_MISC_C_H
#define CLIBS_TEST_MISC_C_H

#include "../Dev/unit_tests.h" /* ... */
#include "../misc.h"           /* ... */

Tester test_one_reverse_int( int64_t n, unsigned base, int64_t res )
{
    return reverse_integer( n, base ) == res;
}

static TEST( misc_c )
{
    UNIT_TEST( max_64( 1, 2 ) == 2 );
    UNIT_TEST( max_64( 3, 2 ) == 3 );
    UNIT_TEST( max_64( 3, -2 ) == 3 );
    {
        int a = 1;
        int b = 1;
        UNIT_TEST( max_64( a, ++b ) == 2 );
    }
    {
        int a = 1;
        int b = 1;
        UNIT_TEST( max_64( a, b++ ) == 1 );
    }
    UNIT_TEST( min_64( 1, 2 ) == 1 );
    UNIT_TEST( min_64( 10, 2 ) == 2 );
    UNIT_TEST( min_64( -10, 2 ) == -10 );

    UNIT_TEST( sgn_64( 1204 ) > 0 );
    UNIT_TEST( sgn_64( -11 ) < 0 );
    UNIT_TEST( sgn_64( 0 ) == 0 );

    UNIT_TEST( is_within( 0, 1, 2 ) );
    UNIT_TEST( is_within( -10, 20, 20 ) );
    UNIT_TEST( is_within( -10, -10, 20 ) );
    UNIT_TEST( !is_within( -10, -11, 20 ) );
    UNIT_TEST( !is_within( 10, 5, 20 ) );

    {
        int a = 1, b = 2;
        UNIT_TEST( cmp_int( &a, &b ) != 0 );
    }
    {
        int arr[ 10 ];
        UNIT_TEST( countof( arr ) == 10 );
    }

    UNIT_TEST( digitsof( 1, 10 ) == 1 );
    UNIT_TEST( digitsof( 10, 10 ) == 2 );
    UNIT_TEST( digitsof( 7, 2 ) == 3 );

    UNIT_TEST( power( 2, 5 ) == 32 );
    UNIT_TEST( power( 2, 31 ) - 1 == INT32_MAX );
    UNIT_TEST( power( 10, 3 ) == 1000 );
    UNIT_TEST( power( 100, 0 ) == 1 );

    UNIT_TEST( test_one_reverse_int( 123, 10, 321 ) );
    UNIT_TEST( test_one_reverse_int( 120, 10, 21 ) );
    UNIT_TEST( test_one_reverse_int( 7, 2, 7 ) );
    UNIT_TEST( test_one_reverse_int( 8, 2, 1 ) );
    UNIT_TEST( test_one_reverse_int( 1, 2, 1 ) );
}
END_TEST

#endif //CLIBS_TEST_MISC_C_H
