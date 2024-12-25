//
// Created by MacBook on 25.12.2024.
//

#include "string_utils.h"

#include <assert.h>

void test_one( string_t test_str, string_t result )
{
    str_t esc = string_escaped( test_str );
    assert( strcmp( esc, result ) == 0 );
}

int main( void )
{
    test_one( "Hopspop", "Hopspop" );
    test_one( "Hopspop\n", "Hopspop\\n" );
    test_one( "Hopspop\nKokot\n", "Hopspop\\nKokot\\n" );
    test_one( "\"Hopspop\"\n", "\\\"Hopspop\\\"\\n" );
    test_one( "Hopspop\\nKokot\n", "Hopspop\\\\nKokot\\n" );
    test_one( "Hopspop\0Kokot\n", "Hopspop" );
    test_one( "\n\r\v\t\\\"", "\\n\\r\\v\\t\\\\\\\"" );
}
