//
// Created by MacBook on 30.12.2024.
//

#include "../assert_that.h"
#include "../string_utils.h"

typedef void( ToFunc )( str_t );
typedef str_t( AsFunc )( string_t );


static void test_one_to( ToFunc func, string_t old, string_t new )
{
    str_t s = strdup( old );
    func( s );
    assert_that( strcmp( s, new ) == 0, "\"%s\" != \"%s\"", s, new );
    free( s );
}

static void test_one_as( AsFunc func, string_t old, string_t res )
{
    str_t got = func( old );
    assert_that( strcmp( got, res ) == 0, "\"%s\" != \"%s\"", got, res );
    free( got );
}

#define START( STR ) printf( "starting %s\n", #STR )
#define END( STR )   printf( "finished %s\n", #STR )

int main( void )
{
    START( "TO" );
    test_one_to( string_to_lower, "Hello, World!", "hello, world!" );
    test_one_to( string_to_lower, "hello, world!", "hello, world!" );
    test_one_to( string_to_lower, "HELLO, WORLD!", "hello, world!" );
    test_one_to( string_to_lower, "HELLO", "hello" );

    test_one_to( string_to_upper, "Hello, World!", "HELLO, WORLD!" );
    test_one_to( string_to_upper, "HELLO, WORLD!", "HELLO, WORLD!" );
    test_one_to( string_to_upper, "hello, world!", "HELLO, WORLD!" );
    test_one_to( string_to_upper, "hello", "HELLO" );
    END( "TO" );

    START( "AS" );
    test_one_as( string_as_lower, "Hello, World!", "hello, world!" );
    test_one_as( string_as_lower, "hello, world!", "hello, world!" );
    test_one_as( string_as_lower, "HELLO, WORLD!", "hello, world!" );
    test_one_as( string_as_lower, "HELLO", "hello" );

    test_one_as( string_as_upper, "Hello, World!", "HELLO, WORLD!" );
    test_one_as( string_as_upper, "HELLO, WORLD!", "HELLO, WORLD!" );
    test_one_as( string_as_upper, "hello, world!", "HELLO, WORLD!" );
    test_one_as( string_as_upper, "hello", "HELLO" );
    END( "AS" );
}
