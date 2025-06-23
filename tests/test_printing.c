//
// Created by Michal Pitner on 23.06.2025.
//

#include "../src/headers/assert_that.h"
#include "../src/headers/misc.h"
#include "../src/headers/unit_tests.h"
#include "../src/string_utils.h"
#include "../src/structs/dynstring.h"

// dynstr.h
#include "../src/headers/array_printf.h"


static void TEST_array_print( void )
{
    const int array[ 10 ] = { 1, 4, 9, 16, 0, -0, -1, INT_MAX, INT_MIN, 123 };
    assert_that( array[ countof( array ) - 1 ] == 123,
                 "the last element should be \"%d\", but it's \"%d\"", 123,
                 array[ countof( array ) - 1 ] );
    array_printf_sde( array, countof( array ), int, "%d",
                      FOREGROUND_BLUE "[" COLOR_DEFAULT, " and ",
                      FOREGROUND_CYAN "]" COLOR_DEFAULT "\n" );

    char *str;
    array_sprintf_sde( str, array, countof( array ), int, "%d", "(", ",", ")" );
    printf( "%s\n", str );

    str_t repl = string_replaced( str, "(", FOREGROUND_BLUE "(" );
    repl       = string_replaced( repl, ")", ")" COLOR_DEFAULT );
    printf( "%s\n", repl );

    printf( "str: %zu, repl: %zu\n", strlen( str ), strlen( repl ) );
    printf( "%zu\n", strlen( COLOR_DEFAULT ) );

    free( str );
}

int main( void )
{
    TEST_array_print();
}
