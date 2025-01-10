//
// Created by MacBook on 08.01.2025.
//

#ifndef CLIBS_TEST_FOREACH_H
#define CLIBS_TEST_FOREACH_H

#include "../assert_that.h"
#include "../misc.h"
#include "../string_utils.h"
#include "../Structs/dynarr.h"
#include "../Structs/dynstring.h"
#include "../unit_tests.h"

// must be after dynarr.h
#include "../foreach.h"


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

static TEST( foreach )
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


#endif //CLIBS_TEST_FOREACH_H
