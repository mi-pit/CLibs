//
// Created by MacBook on 08.01.2025.
//

#ifndef CLIBS_TEST_MISC_C_H
#define CLIBS_TEST_MISC_C_H

#include "../src/Dev/misc.h"
#include "../src/Dev/unit_tests.h" /* ... */
#include "../src/math.h"

Tester test_one_reverse_int( int64_t n, unsigned base, uint64_t res )
{
    return reverse_integer( n, base ) == res;
}

TEST( misc_c )
{
    UNIT_TEST( max_i64( 1, 2 ) == 2 );
    UNIT_TEST( max_i64( 3, 2 ) == 3 );
    UNIT_TEST( max_i64( 3, -2 ) == 3 );
    {
        int a = 1;
        int b = 1;
        UNIT_TEST( max_i64( a, ++b ) == 2 );
    }
    {
        int a = 1;
        int b = 1;
        UNIT_TEST( max_i64( a, b++ ) == 1 );
    }
    {
        int a = 1;
        int b = 1;
        PrintInColor( stdout, FOREGROUND_CYAN, "%i\n", min_m( a, b++ ) );
    }
    {
        double a = 1;
        double b;
        PrintInColor( stdout, FOREGROUND_CYAN, "%f\n", min_m( a, b = .9 ) );
    }
    UNIT_TEST( min_i64( 1, 2 ) == 1 );
    UNIT_TEST( min_i64( 10, 2 ) == 2 );
    UNIT_TEST( min_i64( -10, 2 ) == -10 );

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
    {
        str_t *arr[ 1 ];
        UNIT_TEST( countof( arr ) == 1 );
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
    UNIT_TEST( test_one_reverse_int( 0x1AB, 16, 0xBA1 ) );

    UNIT_TEST( get_next_power_of_two( 1 ) == 2 );
    UNIT_TEST( get_next_power_of_two( 2 ) == 4 );
    UNIT_TEST( get_next_power_of_two( 3 ) == 4 );
    UNIT_TEST( get_next_power_of_two( 4 ) == 8 );
    UNIT_TEST( get_next_power_of_two( 1 << 24 ) == ( 1 << 25 ) );
    UNIT_TEST( get_next_power_of_two( ( 1 << 24 ) - 1 ) == ( 1 << 24 ) );

    UNIT_TEST( get_prev_power_of_two( 2 ) == 1 );
    UNIT_TEST( get_prev_power_of_two( 3 ) == 2 );
    UNIT_TEST( get_prev_power_of_two( 4 ) == 2 );
    UNIT_TEST( get_prev_power_of_two( 5 ) == 4 );
    UNIT_TEST( get_prev_power_of_two( 1 << 24 ) == ( 1 << 23 ) );
    UNIT_TEST( get_prev_power_of_two( ( 1 << 24 ) + 1 ) == ( 1 << 24 ) );
}
END_TEST

#endif //CLIBS_TEST_MISC_C_H
