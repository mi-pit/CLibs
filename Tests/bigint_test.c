//
// Created by MacBook on 09.01.2025.
//

#include "../Structs/bigint.h"

#include "../errors.h"
#include "../misc.h"
#include "../string_utils.h"
#include "../unit_tests.h"

#include <assert.h>
#include <inttypes.h>


Tester try_to_init_as_to_string( int64_t init )
{
    struct bigint *i = bigint_init_as( init );
    assert( i != NULL );

    char *str_got = bigint_to_string( i );
    char *str_wanted;
    assert( str_got != NULL );
    assert( asprintf( &str_wanted, "%" PRIi64, init ) != -1 );

    bool rv = cmpeq( strcmp( str_got, str_wanted ) );

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
}
END_TEST


Tester try_to_string_bigger( int64_t num, int n, string_t desired )
{
    struct bigint *bi = bigint_init();
    assert( bi != NULL );

    for ( int i = 0; i < n; ++i )
        assert( list_append( bi->numbers, &num ) == RV_SUCCESS );

    str_t str_got = bigint_to_string( bi );
    assert( str_got != NULL );

    bool rv = cmpeq( strcmp( str_got, desired ) );

    bigint_destroy( bi );

    return rv;
}


static TEST( to_string_bigger )
{
    UNIT_TEST( try_to_string_bigger( 1, 1, "1" ) );
    UNIT_TEST( try_to_string_bigger( 123, 1, "123" ) );

    struct bigint *bi = bigint_init();
    uint64_t ap       = UINT64_MAX;
    list_append( bi->numbers, &ap );
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
    UNIT_TEST( cmpeq( strcmp( str = bigint_to_string( bi ), "83010348331692982266" ) ) );
    free( str );
}
END_TEST


str_t add_uint_strings( string_t str_1, string_t str_2 );

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


Tester test_number_array( List nums, size_t count, ... )
{
    if ( list_size( nums ) != count )
        return false;

    va_list args;
    va_start( args, count );
    for ( size_t i = 0; i < count; ++i )
        if ( va_arg( args, uint64_t ) != list_access( nums, i, uint64_t ) )
            return false;
    va_end( args );

    return true;
}

TEST( add )
{
    struct bigint *bi = bigint_init();
    UNIT_TEST( test_number_array( bi->numbers, 1, 0 ) );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_number_array( bi->numbers, 1, INT64_MAX ) );

    bigint_add_i( bi, INT64_MAX );
    UNIT_TEST( test_number_array( bi->numbers, 1, 2UL * INT64_MAX ) );

    bigint_add_i( bi, 2 );
    UNIT_TEST( test_number_array( bi->numbers, 2, 0, 1 ) );

    bigint_add_i( bi, 2 );
    UNIT_TEST( test_number_array( bi->numbers, 2, 2, 1 ) );

    for ( int i = 0; i < 3; ++i )
    {
        bigint_add_i( bi, INT64_MAX );
        bigint_add_i( bi, INT64_MAX );
        bigint_add_i( bi, 2 );
        bool is_correct = list_size( bi->numbers ) == 2;

        is_correct = is_correct && list_access( bi->numbers, 0, uint64_t ) == 2;
        is_correct = is_correct && list_access( bi->numbers, 1, uint64_t ) == i + 2;

        UNIT_TEST( is_correct );
    }
}
END_TEST

int main( void )
{
    RUN_TEST( add );
    RUN_TEST( init_and_string );
    RUN_TEST( to_string_bigger );

    RUN_TEST( add_uint_strings );

    FINISH_TESTING();
}
