//
// Created by MacBook on 06.01.2025.
//
#include "../assert_that.h"
#include "../dynarr.h"
#include "../dynstring.h"
#include "../foreach.h"
#include "../misc.h"
#include "../string_utils.h"
#include "../swexpr.h"
#include "../unit_tests.h"
#include "test_dynstr.h"
#include "test_string_utils.h"

#include <stdlib.h>


Tester test_one_foreach_arr( const int64_t arr[], const size_t count )
{
    foreach_arr( int64_t, num, arr, count )
    {
        if ( num != arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_ls( ConstList const numbers_ls,
                            const int64_t numbers_arr[],
                            const size_t count )
{
    assert_that( list_size( numbers_ls ) == count, "list size != array size" );

    foreach_ls( int64_t, num, numbers_ls )
    {
        if ( num != numbers_arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_uni( const int64_t numbers_arr[], const size_t count )
{
    foreach_uni( int64_t, num, numbers_arr[ foreach_index_num ], count )
    {
        if ( num != numbers_arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_str( string_t str )
{
    foreach_str( c, str )
    {
        if ( foreach_cap_c != strlen( str ) )
        {
            fflwarnx( "foreach_cap" );
            return false;
        }
        if ( c != str[ foreach_index_c ] )
        {
            fflwarnx( "c != str[ foreach_idx ]" );
            return false;
        }
    }
    return true;
}
Tester test_one_foreach_dynstr( string_t str )
{
    DynamicString dynstr = dynstr_init_as( str );
    assert_that( dynstr != NULL, "dynstr init" );
    assert_that( strlen( str ) == dynstr_len( dynstr ), "dynstr len" );

    foreach_str( c, dynstr_data( dynstr ) )
    {
        if ( foreach_cap_c != strlen( str ) )
        {
            fflwarnx( "foreach_cap" );
            return false;
        }
        if ( c != str[ foreach_index_c ] )
        {
            fflwarnx( "c != str[ foreach_idx ]" );
            return false;
        }
    }
    return true;
}

TEST( foreach )
{
    List numbers_ls = list_init_type( int64_t );
    assert_that( numbers_ls != NULL, "list init" );
    int64_t numbers_arr[] = {
        1, 2, 4, 6, 7, -1, 2323, 3195,
    };
    assert_that( list_extend( numbers_ls, numbers_arr, sizeof_arr( numbers_arr ) ) ==
                         RV_SUCCESS,
                 "list extend" );

    UNIT_TEST( test_one_foreach_arr( numbers_arr, sizeof_arr( numbers_arr ) ) );
    UNIT_TEST(
            test_one_foreach_ls( numbers_ls, numbers_arr, sizeof_arr( numbers_arr ) ) );
    UNIT_TEST( test_one_foreach_uni( numbers_arr, sizeof_arr( numbers_arr ) ) );

    UNIT_TEST( test_one_foreach_str( "" ) );
    UNIT_TEST( test_one_foreach_str( "HOVNO" ) );
    UNIT_TEST( test_one_foreach_str( "a\nb'\0'" ) );

    UNIT_TEST( test_one_foreach_dynstr( "" ) );
    UNIT_TEST( test_one_foreach_dynstr( "HOVNO" ) );
    UNIT_TEST( test_one_foreach_dynstr( "a\nb'\0'" ) );
}
END_TEST


static int get_swex_val( int branch_1, int branch_2, string_t branch_3 )
{
    swex_init_val( int, branch_1 ) as_new( int, rv )
    {
        swex_case_imm( int, 1 )
        {
            swex_init_val( int, branch_2 ) as( rv )
            {
                swex_case_imm( int, 1 )
                {
                    resolve( int, 10 );
                }
                swex_case_imm( int, 2 )
                {
                    swex_init_str( branch_3 ) as( rv )
                    {
                        swex_case_str( "1" )
                        {
                            resolve( int, 100 );
                        }
                        swex_default()
                        {
                            resolve( int, -100 );
                        }
                    }
                    swex_finish();
                }
                swex_default()
                {
                    resolve( int, -10 );
                }
            }
            swex_finish();
        }
        swex_case_imm( int, 2 )
        {
            resolve( int, 2 );
        }

        swex_default()
        {
            resolve( int, -1 );
        }
    }
    swex_finish();

    return rv;
}
Tester test_one_swex( int branch_1, int branch_2, string_t branch_3, int rv )
{
    return get_swex_val( branch_1, branch_2, branch_3 ) == rv;
}

TEST( swex )
{
    UNIT_TEST( test_one_swex( -1, 0, NULL, -1 ) );
    UNIT_TEST( test_one_swex( -1, 124, "hops", -1 ) );
    UNIT_TEST( test_one_swex( -100, 0, NULL, -1 ) );

    UNIT_TEST( test_one_swex( 2, 0, NULL, 2 ) );
    UNIT_TEST( test_one_swex( 2, 325, "NULL", 2 ) );

    UNIT_TEST( test_one_swex( 1, 1, NULL, 10 ) );
    UNIT_TEST( test_one_swex( 1, 1, "HOPSOPPO", 10 ) );

    UNIT_TEST( test_one_swex( 1, -11, NULL, -10 ) );

    UNIT_TEST( test_one_swex( 1, 2, "1", 100 ) );
    UNIT_TEST( test_one_swex( 1, 2, "Hovno", -100 ) );
}
END_TEST


Tester test_one_reverse_int( int64_t n, unsigned base, int64_t res )
{
    return reverse_integer( n, base ) == res;
}

TEST( misc_c )
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
        UNIT_TEST( sizeof_arr( arr ) == 10 );
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

int main( void )
{
    /*
    RUN_TEST( foreach );

    RUN_TEST( swex );

    RUN_TEST( misc_c );
    */

    //run_string_utils();
    run_dynstr();

    FINISH_TESTING();
}
