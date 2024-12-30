//
// Created by MacBook on 25.12.2024.
//

#include "string_utils.h"

#include <assert.h>

void test_one_to_lower( string_t old, string_t new )
{
    str_t esc = string_escaped( old );
    assert( strcmp( esc, new ) == 0 );
}

int main( void )
{
    test_one_to_lower( "Hopspop", "Hopspop" );
    test_one_to_lower( "Hopspop\n", "Hopspop\\n" );
    test_one_to_lower( "Hopspop\nKokot\n", "Hopspop\\nKokot\\n" );
    test_one_to_lower( "\"Hopspop\"\n", "\\\"Hopspop\\\"\\n" );
    test_one_to_lower( "Hopspop\\nKokot\n", "Hopspop\\\\nKokot\\n" );
    test_one_to_lower( "Hopspop\0Kokot\n", "Hopspop" );
    test_one_to_lower( "\n\r\v\t\\\"", "\\n\\r\\v\\t\\\\\\\"" );
}
