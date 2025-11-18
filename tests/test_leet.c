//
// Created by MacBook on 03.01.2025.
//
#define FOREACH_LS

#include "../src/headers.h"
#include "../src/structs/dictionary.h"


typedef size_t count_t;

#include <inttypes.h>

DEFINE_PRINT_FUNC( int64_t, "%" PRIi64 )


int64_t majority_element( const size_t arrlen, const int64_t array[ const arrlen ] )
{
    Dictionary *freq = dict_init();

    foreach_arr ( int64_t, num, array, arrlen )
    {
        const void *val     = dict_get_val( freq, &num, sizeof num );
        const count_t count = ( val == NULL ? 0 : deref_as( count_t, val ) ) + 1;
        if ( !dict_has_key( freq, &num, sizeof num ) )
            assert_that( dict_insert( freq, &num, sizeof num, &count, sizeof count )
                                 == DICTINSERT_INSERTED,
                         "insert" );
        else
            assert_that( dict_set_val( freq, &num, sizeof num, &count, sizeof count )
                                 == RV_SUCCESS,
                         "set_val" );
    }

    // dict_print_as( freq, print_int64_t, print_int64_t );

    foreach_arr ( int64_t, num, array, arrlen )
    {
        const void *val     = dict_get_val( freq, &num, sizeof num );
        const int64_t key   = num;
        const count_t value = deref_as( count_t, val );

        if ( value >= arrlen / 2 )
        {
            dict_destroy( freq );
            return key;
        }
    }

    dict_destroy( freq );

    return fwarnx_ret( -1, "couldn't find majority element" );
}


#define test_one_majelem( TYPE, DESIRED, ... )                                           \
    do                                                                                   \
    {                                                                                    \
        TYPE values_array[] = { __VA_ARGS__ };                                           \
        TYPE found          = majority_element( countof( values_array ), values_array ); \
        UNIT_TEST( found == DESIRED );                                                   \
    }                                                                                    \
    while ( 0 )


TEST( majelem )
{
    test_one_majelem( int64_t, 1, 1, 2, 1 );

    test_one_majelem( int64_t, 1, 3, 2, 2, 1, 1, 1 );

    test_one_majelem( int64_t,
                      /* desired */ 2,
                      2,
                      4,
                      2,
                      4,
                      2,
                      2,
                      2,
                      1,
                      5,
                      7 );

    test_one_majelem( int64_t,
                      /* desired */ 2,
                      100,
                      2,
                      2,
                      2,
                      500,
                      10000,
                      2,
                      2,
                      2,
                      2,
                      1000000,
                      60,
                      600,
                      2,
                      2,
                      1001000 );
}
END_TEST


int main( void )
{
    RUN_TEST( majelem );
    FINISH_TESTING();
}
