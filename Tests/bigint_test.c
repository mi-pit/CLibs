//
// Created by MacBook on 09.01.2025.
//

#include "../Structs/bigint.h" /* also includes dynarr.h */

#include "../assert_that.h"  /* assert_that(), #include "errors.h" */
#include "../misc.h"         /* cmpeq */
#include "../string_utils.h" /* str types */
#include "../unit_tests.h"   /* TEST, UNIT_TEST, RUN_TEST, FINISH_TESTING */

#include <assert.h>
#include <inttypes.h>

/**
 * Sets the bi->numbers list to be
 * @code
 * size := count;
 * contents := numbers;
 * @endcode
 */
Private void set_number_array( struct bigint *bi,
                               size_t count,
                               const uint64_t numbers[ count ] )
{
    for ( size_t i = 0; i < list_size( bi->numbers ); ++i )
    {
        uint64_t num = numbers[ i ];
        assert_that(
                list_set_at( bi->numbers, i, &num ) == RV_SUCCESS, "list_set_at %zu", i );
    }
    for ( size_t i = list_size( bi->numbers ); i < count; ++i )
    {
        uint64_t num = numbers[ i ];
        assert_that(
                list_append( bi->numbers, &num ) == RV_SUCCESS, "list_append @%zu", i );
    }
}


str_t add_uint_strings( string_t str_1, string_t str_2 );
str_t mul_uint_strings( string_t str_1, string_t str_2 );

#define UINT64_MAX_STRING "18446744073709551615"

Tester try_add_strings( string_t str_1, string_t str_2, string_t result )
{
    str_t sum = add_uint_strings( str_1, str_2 );
    assert( sum != NULL );
    bool rv = cmpeq( strcmp( sum, result ) );
    free( sum );
    return rv;
}

static TEST( add_uint_strings )
{
    UNIT_TEST( try_add_strings( "0", "0", "0" ) );
    UNIT_TEST( try_add_strings( "1", "1", "2" ) );
    UNIT_TEST( try_add_strings( "1", "10", "11" ) );
    UNIT_TEST( try_add_strings( "0", "10", "10" ) );
    UNIT_TEST( try_add_strings( "10", "10", "20" ) );

    UNIT_TEST( try_add_strings( "0", UINT64_MAX_STRING, UINT64_MAX_STRING ) );
    UNIT_TEST( try_add_strings( "1", UINT64_MAX_STRING, "18446744073709551616" ) );
    UNIT_TEST( try_add_strings( "1000", UINT64_MAX_STRING, "18446744073709552615" ) );
    UNIT_TEST( try_add_strings( "5", UINT64_MAX_STRING, "18446744073709551620" ) );
    UNIT_TEST( try_add_strings(
            UINT64_MAX_STRING, UINT64_MAX_STRING, "36893488147419103230" ) );
}
END_TEST

Tester try_mul_strings( string_t str_1, string_t str_2, string_t result )
{
    str_t sum = mul_uint_strings( str_1, str_2 );
    assert( sum != NULL );
    bool rv = cmpeq( strcmp( sum, result ) );
    free( sum );
    return rv;
}

static TEST( mul_uint_strings )
{
    UNIT_TEST( try_mul_strings( "0", "0", "0" ) );
    UNIT_TEST( try_mul_strings( "0", "1", "0" ) );
    UNIT_TEST( try_mul_strings( "1", "1", "1" ) );
    UNIT_TEST( try_mul_strings( "1", "10", "10" ) );

    UNIT_TEST( try_mul_strings( "2", "1", "2" ) );
    UNIT_TEST( try_mul_strings( "2", "10", "20" ) );
    UNIT_TEST( try_mul_strings( "20", "10", "200" ) );

    UNIT_TEST( try_mul_strings( "8", "8", "64" ) );

    UNIT_TEST( try_mul_strings( "18446744073709551616", "0", "0" ) );
    UNIT_TEST( try_mul_strings( "18446744073709551616", "1", "18446744073709551616" ) );
    UNIT_TEST( try_mul_strings( "18446744073709551616",
                                "18446744073709551616",
                                "340282366920938463463374607431768211456" ) );
}
END_TEST


Tester test_number_array( struct bigint *bi, size_t count, const uint64_t arr[ count ] )
{
    if ( list_size( bi->numbers ) != count )
        return false;

    printf( "\n" );
    // array_printf( arr, count, uint64_t, "%llu" );

    for ( size_t i = 0; i < count; ++i )
    {
        uint64_t in_num = list_access( bi->numbers, i, uint64_t );
        uint64_t arg    = arr[ i ];
        if ( arg != in_num )
            return false;
    }

    return true;
}

static TEST( init )
{
    struct bigint bi;
    bigint_init_p( &bi );
    UNIT_TEST( test_number_array( &bi, 1, ( uint64_t[] ){ 0 } ) );

    bigint_destroy_l( bi );
}
END_TEST

static TEST( add )
{
    struct bigint *bi = bigint_init();
    UNIT_TEST( test_number_array( bi, 1, ( uint64_t[] ){ 0 } ) );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_number_array( bi, 1, ( uint64_t[] ){ INT64_MAX } ) );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_number_array( bi, 1, ( uint64_t[] ){ 2UL * INT64_MAX } ) );

    bigint_add_i( bi, 2 );
    UNIT_TEST( test_number_array( bi, 2, ( uint64_t[] ){ 0, 1 } ) );

    bigint_add_i( bi, 2 );
    UNIT_TEST( test_number_array( bi, 2, ( uint64_t[] ){ 2, 1 } ) );

    for ( int i = 0; i < 3; ++i )
    {
        bigint_add_i( bi, INT64_MAX );
        bigint_add_i( bi, INT64_MAX );
        bigint_add_i( bi, 2 );
        bool is_correct = list_size( bi->numbers ) == 2;

        is_correct = is_correct && list_access( bi->numbers, 0, uint64_t ) == 2u;
        is_correct = is_correct && list_access( bi->numbers, 1, uint64_t ) == i + 2u;

        UNIT_TEST( is_correct );
    }

    set_number_array( bi, 2, ( uint64_t[] ){ UINT64_MAX, UINT64_MAX } );
    bigint_add_i( bi, 1 );
    UNIT_TEST( test_number_array( bi, 3, ( uint64_t[] ){ 0, 0, 1 } ) );

    set_number_array(
            bi, 5, ( uint64_t[] ){ UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX, 1 } );
    bigint_add_i( bi, 1 );
    UNIT_TEST( test_number_array( bi, 5, ( uint64_t[] ){ 0, 0, 0, 0, 2 } ) );

    set_number_array(
            bi,
            5,
            ( uint64_t[] ){ UINT64_MAX, UINT64_MAX, UINT64_MAX - 1, UINT64_MAX, 1 } );
    bigint_add_i( bi, 1 );
    UNIT_TEST( test_number_array(
            bi, 5, ( uint64_t[] ){ 0, 0, UINT64_MAX, UINT64_MAX, 1 } ) );

    set_number_array(
            bi,
            5,
            ( uint64_t[] ){ INT64_MAX + 2ULL, UINT64_MAX, UINT64_MAX, UINT64_MAX, 1 } );
    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_number_array( bi, 5, ( uint64_t[] ){ 0, 0, 0, 0, 2ULL } ) );

    bigint_destroy( bi );
}
END_TEST


Tester try_to_init_as_to_string( int64_t init )
{
    struct bigint *i = bigint_init_as( init );
    assert( i != NULL );

    char *str_got = bigint_to_string( i );
    char *str_wanted;
    assert( str_got != NULL );
    assert( asprintf( &str_wanted, "%" PRIi64, init ) != -1 );

    bool rv = cmpeq( strcmp( str_got, str_wanted ) );

    free( str_got );
    free( str_wanted );
    bigint_destroy( i );

    return rv;
}

static TEST( init_and_string )
{
    UNIT_TEST( try_to_init_as_to_string( 0 ) );

    UNIT_TEST( try_to_init_as_to_string( 1 ) );
    UNIT_TEST( try_to_init_as_to_string( 100 ) );
    UNIT_TEST( try_to_init_as_to_string( INT32_MAX ) );
    UNIT_TEST( try_to_init_as_to_string( INT64_MAX ) );

    UNIT_TEST( try_to_init_as_to_string( -1 ) );
    UNIT_TEST( try_to_init_as_to_string( -123 ) );
    UNIT_TEST( try_to_init_as_to_string( INT32_MIN ) );
    UNIT_TEST( try_to_init_as_to_string( INT64_MIN ) );

    struct bigint *bi = bigint_init();
    assert( bi != NULL );
    assert( list_size( bi->numbers ) > 0 );
    UNIT_TEST( test_number_array( bi, 1, ( uint64_t[] ){ 0 } ) );
    uint64_t ap = UINT64_MAX;
    list_set_at( bi->numbers, 0, &ap );

    str_t str = bigint_to_string( bi );
    UNIT_TEST( strcmp( str, "18446744073709551615" ) == 0 );
    free( str );

    bigint_add_i( bi, 1 );
    UNIT_TEST( cmpeq( strcmp( str = bigint_to_string( bi ), "18446744073709551616" ) ) );
    free( str );

    bigint_add_i( bi, 1 );
    UNIT_TEST( cmpeq( strcmp( str = bigint_to_string( bi ), "18446744073709551617" ) ) );
    free( str );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( cmpeq( strcmp( str = bigint_to_string( bi ), "27670116110564327424" ) ) );
    free( str );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( cmpeq( strcmp( str = bigint_to_string( bi ), "55340232221128654847" ) ) );
    free( str );

    bigint_destroy( bi );
}
END_TEST


Tester test_to_string_bigger( string_t desired, size_t count, uint64_t numbers[ count ] )
{
    struct bigint *bi = bigint_init();
    assert( bi != NULL );
    set_number_array( bi, count, numbers );

    str_t str = bigint_to_string( bi );
    assert( str != NULL );
    bool rv = cmpeq( strcmp( str, desired ) );

    free( str );
    bigint_destroy( bi );

    return rv;
}

static TEST( to_string_bigger )
{
    UNIT_TEST( test_to_string_bigger(
            "18446744073709551739", 2, ( uint64_t[] ){ 123, 1 } ) );

    UNIT_TEST( test_to_string_bigger(
            "340282366920938463500268095579187314811", 3, ( uint64_t[] ){ 123, 2, 1 } ) );

    UNIT_TEST( test_to_string_bigger( "680564733860095933419529596016174812437",
                                      3,
                                      ( uint64_t[] ){ 123456789, 987654321, 2 } ) );

    UNIT_TEST( test_to_string_bigger(
            "1157920892373161954321078433688137937065180334623199906008390579158877177118"
            "7199",
            5,
            ( uint64_t[] ){ 18446744073709551615ULL, 2, 4241, 136, 100 } ) );

    UNIT_TEST( test_to_string_bigger(
            "853685836012588585324804879667942656138147390797458807193599",
            4,
            ( uint64_t[] ){ 18446744073709551615ULL, 2, 4241, 136 } ) );

    UNIT_TEST( test_to_string_bigger(
            "1206702713694833739019809347782174702719846968918103380288399530819109515100"
            "4435165023844500874971148011566008467096842002484009659777893013109168704743"
            "0145",
            9,
            ( uint64_t[] ){ 1, 2, 3, 4, 5, 6, 7, 8, 9 } ) );

    UNIT_TEST( test_to_string_bigger(
            "5637469480917861191136013294670630515041198256094041550171248147422583527368"
            "1937743251361084371853355349747373629843502003878571735840022685338567201623"
            "5941806060615633599916733952652630150544744365992134102634434934383516232829"
            "5088898679737927720408360514324618364596738089792512480162902435255047530934"
            "3940862395667494498992128",
            18,
            ( uint64_t[] ){
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 } ) );
}
END_TEST


int main( void )
{
    RUN_TEST( add_uint_strings );
    RUN_TEST( mul_uint_strings );

    RUN_TEST( init );
    RUN_TEST( add );
    RUN_TEST( init_and_string );
    RUN_TEST( to_string_bigger );

    FINISH_TESTING();
}
