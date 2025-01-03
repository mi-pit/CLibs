//
// Created by MacBook on 30.12.2024.
//

#include "../assert_that.h"
#include "../string_utils.h"
#include "run_test.h"

typedef void( ToFunc )( str_t );
typedef str_t( AsFunc )( string_t );


static bool test_one_to( ToFunc func, string_t old, string_t new )
{
    str_t s = strdup( old );
    func( s );
    bool rv = strcmp( s, new ) == 0;
    free( s );
    return rv;
}

static bool test_one_as( AsFunc func, string_t old, string_t res )
{
    str_t got = func( old );
    bool rv   = strcmp( got, res ) == 0;
    free( got );
    return rv;
}

TEST( to )
{
    UNIT_TEST( test_one_to( string_to_lower, "Hello, World!", "hello, world!" ) );
    UNIT_TEST( test_one_to( string_to_lower, "hello, world!", "hello, world!" ) );
    UNIT_TEST( test_one_to( string_to_lower, "HELLO, WORLD!", "hello, world!" ) );
    UNIT_TEST( test_one_to( string_to_lower, "HELLO", "hello" ) );

    UNIT_TEST( test_one_to( string_to_upper, "Hello, World!", "HELLO, WORLD!" ) );
    UNIT_TEST( test_one_to( string_to_upper, "HELLO, WORLD!", "HELLO, WORLD!" ) );
    UNIT_TEST( test_one_to( string_to_upper, "hello, world!", "HELLO, WORLD!" ) );
    UNIT_TEST( test_one_to( string_to_upper, "hello", "HELLO" ) );
}
END_TEST

TEST( as )
{
    UNIT_TEST( test_one_as( string_as_lower, "Hello, World!", "hello, world!" ) );
    UNIT_TEST( test_one_as( string_as_lower, "hello, world!", "hello, world!" ) );
    UNIT_TEST( test_one_as( string_as_lower, "HELLO, WORLD!", "hello, world!" ) );
    UNIT_TEST( test_one_as( string_as_lower, "HELLO", "hello" ) );

    UNIT_TEST( test_one_as( string_as_upper, "Hello, World!", "HELLO, WORLD!" ) );
    UNIT_TEST( test_one_as( string_as_upper, "HELLO, WORLD!", "HELLO, WORLD!" ) );
    UNIT_TEST( test_one_as( string_as_upper, "hello, world!", "HELLO, WORLD!" ) );
    UNIT_TEST( test_one_as( string_as_upper, "hello", "HELLO" ) );
}
END_TEST

int main( void )
{
    RUN( to );
    RUN( as );
}
