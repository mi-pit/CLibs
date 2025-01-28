//
// Created by MacBook on 09.01.2025.
//

#include "../Structs/bigint.h" /* also includes dynarr.h, misc.h */

#include "../Dev/assert_that.h" /* assert_that(), #include "../errors.h" */
#include "../Dev/unit_tests.h"  /* TEST, UNIT_TEST, RUN_TEST, FINISH_TESTING */
#include "../string_utils.h"    /* str types */

#include <assert.h>
#include <inttypes.h> /* PRIi64 */

/**
 * Sets the bi->numbers list to be
 * @code
 * size     := count;
 * contents := numbers;
 * @endcode
 */
Private void set_number_array( struct bigint *bi,
                               sign_t sign,
                               size_t count,
                               const uint64_t numbers[ count ] )
{
    bi->sign = sign;

    while ( list_size( bi->numbers ) > count )
        assert_that( list_pop( bi->numbers, NULL ) == RV_SUCCESS,
                     "list_pop at size %zu",
                     list_size( bi->numbers ) );

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

Private void reset_number_array( struct bigint *bi )
{
    uint64_t nums[ 1 ] = { 0 };
    set_number_array( bi, SIGN_POS, countof( nums ), nums );
}


#define UINT64_MAX_STRING "18446744073709551615"

Tester try_add_strings( string_t str_1, string_t str_2, string_t result )
{
    str_t sum = add_uint_strings( str_1, str_2 );
    assert( sum != NULL );
    bool rv = cmpeq( strcmp( sum, result ) );
    free( sum );
    return rv;
}

TEST( add_uint_strings )
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

TEST( mul_uint_strings )
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


Tester test_number_array( const struct bigint *bi,
                          size_t count,
                          const uint64_t arr[ count ] )
{
    if ( list_size( bi->numbers ) != count )
        return false;

    for ( size_t i = 0; i < count; ++i )
    {
        uint64_t in_num = list_access( bi->numbers, i, uint64_t );
        uint64_t arg    = arr[ i ];
        if ( arg != in_num )
            return false;
    }

    return true;
}

TEST( init )
{
    struct bigint bi;
    bigint_init_p( &bi );
    UNIT_TEST( test_number_array( &bi, 1, ( uint64_t[] ){ 0 } ) );

    bigint_destroy_l( bi );
}
END_TEST


Tester try_to_init_as_to_string( int64_t init )
{
    struct bigint *i = bigint_init_as( init );
    assert( i != NULL );

    char *str_got = bigint_to_string( i );
    assert( str_got != NULL );
    char *str_wanted;
    assert( asprintf( &str_wanted, "%" PRIi64, init ) != -1 );

    bool rv = cmpeq( strcmp( str_got, str_wanted ) );

    free( str_got );
    free( str_wanted );
    bigint_destroy( i );

    return rv;
}

TEST( init_and_string )
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
    assert( list_size( bi->numbers ) > 0 );
    UNIT_TEST( test_number_array( bi, 1, ( uint64_t[] ){ 0ULL } ) );

    bigint_add_i( bi, INT64_MAX );
    bigint_add_i( bi, INT64_MAX );
    bigint_add_i( bi, 1 );
    str_t str = bigint_to_string( bi );
    UNIT_TEST( cmpeq( strcmp( str, "18446744073709551615" ) ) );
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
    UNIT_TEST( cmpeq( strcmp( str = bigint_to_string( bi ), "36893488147419103231" ) ) );
    free( str );

    bigint_destroy( bi );
}
END_TEST


Tester test_to_string_bigger_( string_t desired, size_t count, uint64_t numbers[ count ] )
{
    struct bigint *bi = bigint_init();
    assert( bi != NULL );
    set_number_array( bi, SIGN_POS, count, numbers );

    str_t str = bigint_to_string( bi );
    assert( str != NULL );
    bool rv = cmpeq( strcmp( str, desired ) );

    free( str );
    bigint_destroy( bi );

    return rv;
}

#define test_to_string_bigger( STRING, COUNT, ... ) \
    test_to_string_bigger_( STRING, COUNT, ( uint64_t[ COUNT ] ){ __VA_ARGS__ } )

TEST( to_string_bigger )
{
    UNIT_TEST( test_to_string_bigger( "10000000000000000123", 2, 123, 1 ) );

    UNIT_TEST( test_to_string_bigger(
            "100000000000000000020000000000000000123", 3, 123, 2, 1 ) );

    UNIT_TEST( test_to_string_bigger(
            "200000000009876543210000000000123456789", 3, 123456789, 987654321, 2 ) );

    UNIT_TEST( test_to_string_bigger(
            "1000000000000000000136000000000000000424100000000000000000029999999999999999"
            "999",
            5,
            BIGINT_LIST_MEMBER_MAX,
            2,
            4241,
            136,
            100 ) );

    UNIT_TEST( test_to_string_bigger( "99999999999999999999999999999999999999000000000000"
                                      "000424100000000000000000029999999999999999999",
                                      5,
                                      BIGINT_LIST_MEMBER_MAX,
                                      2,
                                      4241,
                                      BIGINT_LIST_MEMBER_MAX,
                                      BIGINT_LIST_MEMBER_MAX ) );

    UNIT_TEST(
            test_to_string_bigger( "90000000000000000008000000000000000000700000000000000"
                                   "00006000000000000000000500000000000000000040000000000"
                                   "00000000300000000000000000020000000000000000001",
                                   9,
                                   1,
                                   2,
                                   3,
                                   4,
                                   5,
                                   6,
                                   7,
                                   8,
                                   9 ) );

    UNIT_TEST( test_to_string_bigger(
            "1700000000000000000160000000000000000015000000000000000001400000000000000000"
            "1300000000000000000120000000000000000011000000000000000001000000000000000000"
            "0900000000000000000080000000000000000007000000000000000000600000000000000000"
            "0500000000000000000040000000000000000003000000000000000000200000000000000000"
            "010000000000000000000",
            18,
            0,
            1,
            2,
            3,
            4,
            5,
            6,
            7,
            8,
            9,
            10,
            11,
            12,
            13,
            14,
            15,
            16,
            17 ) );


    UNIT_TEST( test_to_string_bigger(
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001000000000000000000"
            "1000000000000000000100000000000000000010000000000000000001",
            100,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1,
            1 ) );
}
END_TEST


Tester test_metadata( const struct bigint *bi,
                      sign_t sign,
                      size_t count,
                      const uint64_t array[ count ] )
{
    if ( bi->sign != sign )
        return false;
    return test_number_array( bi, count, array );
}


TEST( add )
{
    struct bigint *bi = bigint_init();
    UNIT_TEST( test_number_array( bi, 1, ( uint64_t[] ){ 0 } ) );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_metadata( bi, SIGN_POS, 1, ( uint64_t[ 1 ] ){ INT64_MAX } ) );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_metadata(
            bi, SIGN_POS, 2, ( uint64_t[ 2 ] ){ 8446744073709551614, 1 } ) );

    bigint_add_i( bi, 2 );
    UNIT_TEST( test_metadata(
            bi, SIGN_POS, 2, ( uint64_t[ 2 ] ){ 8446744073709551616, 1 } ) );

    bigint_add_i( bi, 2 );
    UNIT_TEST( test_metadata(
            bi, SIGN_POS, 2, ( uint64_t[ 2 ] ){ 8446744073709551618, 1 } ) );

    for ( int i = 0; i < 3; ++i )
    {
        bigint_add_i( bi, UINT64_C( 5000000000000000000 ) );
        bigint_add_i( bi, UINT64_C( 5000000000000000000 ) );
        bool is_correct = list_size( bi->numbers ) == 2 && bi->sign == SIGN_POS;

        is_correct = is_correct
                     && list_access( bi->numbers, 0, uint64_t )
                                == UINT64_C( 8446744073709551618 );
        is_correct = is_correct && list_access( bi->numbers, 1, uint64_t ) == i + 2u;

        UNIT_TEST( is_correct );
    }

    set_number_array( bi,
                      SIGN_POS,
                      2,
                      ( uint64_t[] ){ BIGINT_LIST_MEMBER_MAX, BIGINT_LIST_MEMBER_MAX } );
    bigint_add_i( bi, 1 );
    UNIT_TEST( test_number_array( bi, 3, ( uint64_t[] ){ 0, 0, 1 } ) );

    set_number_array( bi,
                      SIGN_POS,
                      5,
                      ( uint64_t[] ){ BIGINT_LIST_MEMBER_MAX,
                                      BIGINT_LIST_MEMBER_MAX,
                                      BIGINT_LIST_MEMBER_MAX,
                                      BIGINT_LIST_MEMBER_MAX,
                                      1 } );
    bigint_add_i( bi, 1 );
    UNIT_TEST( test_number_array( bi, 5, ( uint64_t[] ){ 0, 0, 0, 0, 2 } ) );

    set_number_array( bi,
                      SIGN_POS,
                      5,
                      ( uint64_t[] ){ BIGINT_LIST_MEMBER_MAX,
                                      BIGINT_LIST_MEMBER_MAX,
                                      BIGINT_LIST_MEMBER_MAX - 1,
                                      BIGINT_LIST_MEMBER_MAX,
                                      1 } );
    bigint_add_i( bi, 1 );
    UNIT_TEST( test_number_array(
            bi,
            5,
            ( uint64_t[] ){ 0, 0, BIGINT_LIST_MEMBER_MAX, BIGINT_LIST_MEMBER_MAX, 1 } ) );

    set_number_array( bi,
                      SIGN_POS,
                      5,
                      ( uint64_t[] ){ INT64_MAX + 2ULL,
                                      BIGINT_LIST_MEMBER_MAX,
                                      BIGINT_LIST_MEMBER_MAX,
                                      BIGINT_LIST_MEMBER_MAX,
                                      1 } );
    assert( test_metadata( bi,
                           SIGN_POS,
                           5,
                           ( uint64_t[] ){ INT64_MAX + 2ULL,
                                           BIGINT_LIST_MEMBER_MAX,
                                           BIGINT_LIST_MEMBER_MAX,
                                           BIGINT_LIST_MEMBER_MAX,
                                           1 } ) );
    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_number_array( bi, 5, ( uint64_t[] ){ 0, 0, 0, 0, 2ULL } ) );

    bigint_destroy( bi );
}
END_TEST

TEST( sub )
{
    struct bigint *bi = bigint_init();
    assert_that( test_number_array( bi, 1, ( uint64_t[] ){ 0 } ),
                 "initialization fails" );

    bigint_add_i( bi, -1 );
    UNIT_TEST( test_metadata( bi, SIGN_NEG, 1, ( uint64_t[ 1 ] ){ 1 } ) );

    set_number_array( bi, SIGN_POS, 3, ( uint64_t[ 3 ] ){ 10uLL, 1234uLL, 1uLL } );
    bigint_add_i( bi, -11 );
    UNIT_TEST(
            test_metadata( bi,
                           SIGN_POS,
                           3,
                           ( uint64_t[ 3 ] ){ BIGINT_LIST_MEMBER_MAX, 1233uLL, 1uLL } ) );

    reset_number_array( bi );
    assert_that( test_number_array( bi, 1, ( uint64_t[ 1 ] ){ 0 } ),
                 "number array not reset" );
    bigint_add_i( bi, -INT64_MAX );
    UNIT_TEST( test_metadata( bi, SIGN_NEG, 1, ( uint64_t[] ){ INT64_MAX } ) );

    bigint_add_i( bi, -INT64_MAX );
    UNIT_TEST( test_metadata(
            bi, SIGN_NEG, 2, ( uint64_t[ 2 ] ){ 8446744073709551614, 1 } ) );

    bigint_add_i( bi, -1553255926290448386 );
    UNIT_TEST( test_metadata( bi, SIGN_NEG, 2, ( uint64_t[] ){ 0, 2 } ) );

    set_number_array(
            bi, SIGN_POS, 3, ( uint64_t[] ){ BIGINT_LIST_MEMBER_MAX, 1234, 1 } );
    assert_that(
            test_metadata(
                    bi, SIGN_POS, 3, ( uint64_t[] ){ BIGINT_LIST_MEMBER_MAX, 1234, 1 } ),
            "incorrect set: sign=%i, size=%zu",
            bi->sign,
            bigint_sizeof( bi ) );
    bigint_add_i( bi, -11 );
    UNIT_TEST( test_metadata(
            bi, SIGN_POS, 3, ( uint64_t[] ){ BIGINT_LIST_MEMBER_MAX - 11, 1234, 1 } ) );

    set_number_array( bi, SIGN_NEG, 3, ( uint64_t[ 3 ] ){ 50000, 1234, 1 } );
    bigint_add_i( bi, 50000 );
    UNIT_TEST( test_metadata( bi, SIGN_NEG, 3, ( uint64_t[ 3 ] ){ 0, 1234, 1 } ) );

    bigint_add_i( bi, -10 );
    UNIT_TEST( test_metadata( bi, SIGN_NEG, 3, ( uint64_t[ 3 ] ){ 10, 1234, 1 } ) );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_metadata(
            bi, SIGN_NEG, 3, ( uint64_t[ 3 ] ){ 776627963145224203, 1233, 1 } ) );

    reset_number_array( bi );
    assert_that( test_metadata( bi, SIGN_POS, 1, ( uint64_t[ 1 ] ){ 0 } ),
                 "sign=%s, size=%zu",
                 bi->sign == SIGN_POS ? "+" : "-",
                 list_size( bi->numbers ) );
    bigint_add_power( bi, -1, 1 );
    UNIT_TEST( test_metadata( bi, SIGN_NEG, 2, ( uint64_t[ 2 ] ){ 0, 1 } ) );

    bigint_add_power( bi, INT64_MIN, 0 );
    list_printf( bi->numbers, uint64_t, "%" PRIu64 );
    UNIT_TEST( test_metadata(
            bi, SIGN_NEG, 2, ( uint64_t[ 2 ] ){ 9223372036854775807uLL - 1, 1 } ) );

    bigint_add_power( bi, 1, 0 );
    UNIT_TEST( test_metadata(
            bi, SIGN_NEG, 2, ( uint64_t[ 3 ] ){ 9223372036854775807uLL - 2, 1 } ) );

    set_number_array( bi, SIGN_POS, 2, ( uint64_t[ 2 ] ){ 0, 1 } );
    bigint_add_i( bi, -1 );
    UNIT_TEST( test_metadata(
            bi, SIGN_POS, 1, ( uint64_t[ 1 ] ){ BIGINT_LIST_MEMBER_MAX } ) );

    set_number_array( bi, SIGN_NEG, 2, ( uint64_t[ 2 ] ){ 0, 1 } );
    bigint_add_i( bi, 1 );
    UNIT_TEST( test_metadata(
            bi, SIGN_NEG, 1, ( uint64_t[ 1 ] ){ BIGINT_LIST_MEMBER_MAX } ) );

    set_number_array( bi, SIGN_POS, 2, ( uint64_t[ 2 ] ){ 0, 1 } );
    bigint_add_i( bi, -10 );
    UNIT_TEST( test_metadata(
            bi, SIGN_POS, 1, ( uint64_t[ 1 ] ){ BIGINT_LIST_MEMBER_MAX - 9 } ) );

    set_number_array( bi, SIGN_NEG, 2, ( uint64_t[ 2 ] ){ 0, 1 } );
    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST(
            test_metadata( bi, SIGN_NEG, 1, ( uint64_t[ 1 ] ){ 776627963145224193 } ) );

    reset_number_array( bi );
    assert( bi->sign == SIGN_POS );
    bigint_add_power( bi, 1, 0x1000 );
    UNIT_TEST( bigint_sizeof( bi ) == 0x1000 + 1 );
#ifdef PRINT_BIG_ONE_
    SetTerminalColor( FOREGROUND_MAGENTA );
    list_printf( bi->numbers, uint64_t, "%" PRIu64 );
    SetTerminalColor( COLOR_DEFAULT );
#endif
    str_t s                = bigint_to_string( bi );
    bool is_correct_0x1000 = strlen( s ) == 77825 && s[ 0 ] == '1';
    for ( size_t i = 1; i < 77825; ++i )
        is_correct_0x1000 = is_correct_0x1000 && s[ i ] == '0';

    UNIT_TEST( is_correct_0x1000 );
#ifdef PRINT_BIG_ONE_
    PrintInColor( stdout, FOREGROUND_MAGENTA, ">\"%s\"\n", s );
#endif
    free( s );

    bigint_destroy( bi );
}
END_TEST

TEST( bigint_add_b )
{
    struct bigint *bi1 = bigint_init_as( INT64_MAX );
    struct bigint *bi2 = bigint_init();
    bigint_add_power( bi2, 123, 3 );
    UNIT_TEST( test_metadata( bi1, SIGN_POS, 1, ( uint64_t[ 1 ] ){ INT64_MAX } ) );
    UNIT_TEST( test_metadata( bi2, SIGN_POS, 4, ( uint64_t[ 4 ] ){ 0, 0, 0, 123 } ) );

    assert( bigint_add_b( bi1, bi2 ) == RV_SUCCESS );
    UNIT_TEST( test_metadata(
            bi1, SIGN_POS, 4, ( const uint64_t[ 4 ] ){ INT64_MAX, 0, 0, 123 } ) );

    set_number_array( bi1,
                      SIGN_POS,
                      3,
                      ( const uint64_t[ 3 ] ){ 5163779307421638327, 59287839, 1509730 } );
    set_number_array( bi2, SIGN_POS, 1, ( const uint64_t[ 1 ] ){ 23096907901937 } );
    bigint_add_b( bi1, bi2 );
    UNIT_TEST( test_metadata(
            bi1,
            SIGN_POS,
            3,
            ( const uint64_t[ 3 ] ){ 5163802404329540264, 59287839, 1509730 } ) );

    set_number_array( bi1,
                      SIGN_POS,
                      3,
                      ( const uint64_t[ 3 ] ){ 5163779307421638327, 59287839, 1509730 } );
    set_number_array( bi2, SIGN_NEG, 1, ( const uint64_t[ 1 ] ){ 5163779307421638327 } );
    bigint_add_b( bi1, bi2 );
    UNIT_TEST( test_metadata(
            bi1, SIGN_POS, 3, ( const uint64_t[ 3 ] ){ 0, 59287839, 1509730 } ) );

    set_number_array( bi1,
                      SIGN_POS,
                      3,
                      ( const uint64_t[ 3 ] ){
                              BIGINT_LIST_MEMBER_MAX, BIGINT_LIST_MEMBER_MAX, 1509730 } );
    set_number_array( bi2, SIGN_POS, 1, ( const uint64_t[ 1 ] ){ 1 } );
    bigint_add_b( bi1, bi2 );
    UNIT_TEST(
            test_metadata( bi1, SIGN_POS, 3, ( const uint64_t[ 3 ] ){ 0, 0, 1509731 } ) );

    set_number_array(
            bi1,
            SIGN_POS,
            2,
            ( const uint64_t[ 2 ] ){ BIGINT_LIST_MEMBER_MAX, BIGINT_LIST_MEMBER_MAX } );
    set_number_array( bi2, SIGN_POS, 3, ( const uint64_t[ 3 ] ){ 1, 1, 1 } );
    bigint_add_b( bi1, bi2 );
    UNIT_TEST( test_metadata( bi1, SIGN_POS, 3, ( const uint64_t[ 3 ] ){ 0, 1, 2 } ) );

    set_number_array(
            bi1,
            SIGN_NEG,
            2,
            ( const uint64_t[ 2 ] ){ BIGINT_LIST_MEMBER_MAX, BIGINT_LIST_MEMBER_MAX } );
    set_number_array(
            bi2,
            SIGN_POS,
            2,
            ( const uint64_t[ 2 ] ){ BIGINT_LIST_MEMBER_MAX, BIGINT_LIST_MEMBER_MAX } );
    bigint_add_b( bi1, bi2 );
    UNIT_TEST( test_number_array( bi1, 1, ( const uint64_t[ 3 ] ){ 0 } ) );

    bigint_destroy( bi1 );
    bigint_destroy( bi2 );
}
END_TEST

TEST( bigint_cmp )
{
    struct bigint *bi_low  = bigint_init_as( 12345 );
    struct bigint *bi_high = bigint_init_as( 1234567890 );
    UNIT_TEST( bigint_cmp_i( bi_low, 12345 ) == BIGINT_EQ );
    UNIT_TEST( bigint_cmp_i( bi_low, 1234 ) == BIGINT_GT );
    UNIT_TEST( bigint_cmp_i( bi_low, 123456 ) == BIGINT_LT );

    set_number_array( bi_low, SIGN_NEG, 1, ( uint64_t[ 1 ] ){ 12345 } );
    UNIT_TEST( bigint_cmp_i( bi_low, 12345 ) == BIGINT_LT );
    set_number_array( bi_low, SIGN_NEG, 1, ( uint64_t[ 1 ] ){ 1234567 } );
    UNIT_TEST( bigint_cmp_i( bi_low, 12345 ) == BIGINT_LT );
    set_number_array( bi_low, SIGN_NEG, 1, ( uint64_t[ 1 ] ){ 0 } );
    UNIT_TEST( bigint_cmp_i( bi_low, 12345 ) == BIGINT_LT );

    UNIT_TEST( bigint_cmp_b( bi_low, bi_high ) == BIGINT_LT );
    UNIT_TEST( bigint_cmp_b( bi_high, bi_low ) == BIGINT_GT );
    UNIT_TEST( bigint_cmp_b( bi_high, bi_high ) == BIGINT_EQ );

    bigint_destroy( bi_low );
    bigint_destroy( bi_high );
}
END_TEST

Tester test_one_from_string( string_t str,
                             sign_t sign,
                             size_t count,
                             uint64_t array[ count ] )
{
    struct bigint *bi;
    int rv = bigint_from_string( str, &bi );
    assert_that( rv == RV_SUCCESS, "bigint from string: %s", rv_to_string( rv ) );
    if ( bigint_sizeof( bi ) != count )
        return false;
    if ( !test_metadata( bi, sign, count, array ) )
        return false;

    bigint_destroy( bi );
    return true;
}

TEST( from_string )
{
    UNIT_TEST(
            test_one_from_string( "234098", SIGN_POS, 1, ( uint64_t[ 1 ] ){ 234098 } ) );
    UNIT_TEST( test_one_from_string( "0", SIGN_POS, 1, ( uint64_t[ 1 ] ){ 0 } ) );
    UNIT_TEST( test_one_from_string(
            "10000000000000000000", SIGN_POS, 2, ( uint64_t[ 2 ] ){ 0, 1 } ) );
    UNIT_TEST( test_one_from_string( "+123", SIGN_POS, 1, ( uint64_t[ 1 ] ){ 123 } ) );
    UNIT_TEST( test_one_from_string( "-123", SIGN_NEG, 1, ( uint64_t[ 1 ] ){ 123 } ) );
    UNIT_TEST( test_one_from_string(
            "-10000000000000000000", SIGN_NEG, 2, ( uint64_t[ 2 ] ){ 0, 1 } ) );
    UNIT_TEST( test_one_from_string(
            "9350000000000000123123000000000012312312399999999999999999999999999999999999"
            "999",
            SIGN_POS,
            5,
            ( uint64_t[ 5 ] ){ BIGINT_LIST_MEMBER_MAX,
                               BIGINT_LIST_MEMBER_MAX,
                               123123123,
                               123123,
                               935 } ) );
    UNIT_TEST( test_one_from_string(
            "-100000000000000000000000000000000000000000000000000000093500000000000001231"
            "23000000000012312312300000000000010905800000000157539609843",
            SIGN_NEG,
            8,
            ( uint64_t[ 8 ] ){
                    157539609843, 1090580, 123123123, 123123, 935, 0, 0, 1 } ) );
    UNIT_TEST( test_one_from_string(
            "+100000000000000000100000000000000000000000000000000000093500000000000001231"
            "23000000000012312312300000000000010905800000000157539609843",
            SIGN_POS,
            8,
            ( uint64_t[ 8 ] ){
                    157539609843, 1090580, 123123123, 123123, 935, 0, 10, 1 } ) );

    struct bigint *bi;
    UNIT_TEST( bigint_from_string( "Hovenko", &bi ) == RV_EXCEPTION );
    UNIT_TEST( bigint_from_string( "", &bi ) == RV_EXCEPTION );
    UNIT_TEST( bigint_from_string( "+1h", &bi ) == RV_EXCEPTION );
    UNIT_TEST( bigint_from_string( "1+", &bi ) == RV_EXCEPTION );
    UNIT_TEST( bigint_from_string( "+", &bi ) == RV_EXCEPTION );
    UNIT_TEST( bigint_from_string( "+-123", &bi ) == RV_EXCEPTION );
}
END_TEST


Tester test_one_multi_cmp( string_t s1, string_t s2, int cmp_rv )
{
    struct bigint *bi1, *bi2;
    assert( bigint_from_string( s1, &bi1 ) == RV_SUCCESS );
    assert( bigint_from_string( s2, &bi2 ) == RV_SUCCESS );

    bool rv = bigint_cmp_b( bi1, bi2 ) == cmp_rv;

    bigint_destroy( bi1 );
    bigint_destroy( bi2 );

    return rv;
}

Tester test_one_multi_add( string_t s1, string_t s2, string_t result )
{
    struct bigint *bi1, *bi2, *res;
    assert( bigint_from_string( s1, &bi1 ) == RV_SUCCESS );
    assert( bigint_from_string( s2, &bi2 ) == RV_SUCCESS );

    assert( bigint_from_string( result, &res ) == RV_SUCCESS );

    assert( bigint_add_b( bi1, bi2 ) == RV_SUCCESS );

    SetTerminalColor( stdout, FOREGROUND_BLUE );
    list_printf_sde( bi1->numbers, uint64_t, "%llu", "\nbi1: [\n\t", "\n\t", "\n]\n" );
    list_printf_sde( res->numbers, uint64_t, "%llu", "\nres: [\n\t", "\n\t", "\n]\n" );
    SetTerminalColor( stdout, COLOR_DEFAULT );

    bool rv = cmpeq( bigint_cmp_b( bi1, res ) );

    bigint_destroy( bi1 );
    bigint_destroy( bi2 );
    bigint_destroy( res );

    return rv;
}

TEST( multi )
{
    UNIT_TEST( test_one_multi_cmp(
            "29370597230597209723059809895032098350297076902390976097539040",
            "29370597230597209723059809895032098350297076902390976097539041",
            BIGINT_LT ) );
    UNIT_TEST( test_one_multi_cmp(
            "29370597230597209723059809895032098350297076902390976097539041",
            "29370597230597209723059809895032098350297076902390976097539041",
            BIGINT_EQ ) );
    UNIT_TEST( test_one_multi_cmp(
            "29370597230597209723059809895032098350297016902390976097539042",
            "29370597230597209723059809895032098350297076902390976097539041",
            BIGINT_LT ) );
    UNIT_TEST( test_one_multi_cmp(
            "29370597230597209723059809895032098350297006902390976097539042",
            "29370597230597209723059809895032098350297001902390976097539042",
            BIGINT_GT ) );

    UNIT_TEST( test_one_multi_add( "+395203950928293509809835029",
                                   "2739428037492848670349687096872653812",
                                   "2739428037888052621277980606682488841" ) );
    UNIT_TEST( test_one_multi_add( "+395203950928293509809835029",
                                   "-2739428037492848670349687096872653812",
                                   "-2739428037097644719421393587062818783" ) );
}
END_TEST


int main( void )
{
    RUN_TEST( add_uint_strings );
    RUN_TEST( mul_uint_strings );

    RUN_TEST( init );
    RUN_TEST( init_and_string );
    RUN_TEST( to_string_bigger );

    RUN_TEST( add );
    RUN_TEST( sub );
    RUN_TEST( bigint_add_b );

    RUN_TEST( bigint_cmp );

    RUN_TEST( from_string );

    RUN_TEST( multi );

    FINISH_TESTING();
}
