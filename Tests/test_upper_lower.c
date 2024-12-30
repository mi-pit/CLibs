//
// Created by MacBook on 30.12.2024.
//

#include "../assert_that.h"
#include "../string_utils.h"

typedef void( ToFunc )( str_t );


static void test_one_to( ToFunc func, string_t old, string_t new )
{
    str_t s = strdup( old );
    func( s );
    assert_that( strcmp( s, new ) == 0, "\"%s\" != \"%s\"", s, new );
    free( s );
}

int main( void )
{
    test_one_to( string_to_lower, "Hello, World!", "hello, world!" );
    test_one_to( string_to_lower, "hello, world!", "hello, world!" );
    test_one_to( string_to_lower, "HELLO, WORLD!", "hello, world!" );
    test_one_to( string_to_lower, "HELLO", "hello" );

    test_one_to( string_to_upper, "Hello, World!", "HELLO, WORLD!" );
    test_one_to( string_to_upper, "HELLO, WORLD!", "HELLO, WORLD!" );
    test_one_to( string_to_upper, "hello, world!", "HELLO, WORLD!" );
    test_one_to( string_to_upper, "hello", "HELLO" );
}
