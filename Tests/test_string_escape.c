//
// Created by MacBook on 25.12.2024.
//

#include "../run_test.h"
#include "../string_utils.h"

#include <stdlib.h>

static bool test_one( string_t old, string_t new )
{
    str_t esc = string_escaped( old );
    bool rv   = strcmp( esc, new ) == 0;
    free( esc );
    return rv;
}

TEST( escape )
{
    UNIT_TEST( test_one( "Hopspop", "Hopspop" ) );
    UNIT_TEST( test_one( "Hopspop\n", "Hopspop\\n" ) );
    UNIT_TEST( test_one( "Hopspop\nKokot\n", "Hopspop\\nKokot\\n" ) );
    UNIT_TEST( test_one( "\"Hopspop\"\n", "\\\"Hopspop\\\"\\n" ) );
    UNIT_TEST( test_one( "Hopspop\\nKokot\n", "Hopspop\\\\nKokot\\n" ) );
    UNIT_TEST( test_one( "Hopspop\0Kokot\n", "Hopspop" ) );
    UNIT_TEST( test_one( "\n\r\v\t\\\"", "\\n\\r\\v\\t\\\\\\\"" ) );
}
END_TEST

int main( void )
{
    RUN( escape );
}
