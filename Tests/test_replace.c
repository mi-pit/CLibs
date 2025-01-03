//
// Created by MacBook on 03.01.2025.
//

#include "../assert_that.h"
#include "../dynstring.h"
#include "../string_utils.h"

#define test_one( ORIGINAL, OLD_TOK, NEW_TOK, EXPECTED )            \
    do                                                              \
    {                                                               \
        str_t got_ = string_replaced( ORIGINAL, OLD_TOK, NEW_TOK ); \
        assert_that( strcmp( got_, EXPECTED ) == 0,                 \
                     "expected \"%s\", got \"%s\"",                 \
                     EXPECTED,                                      \
                     got_ );                                        \
        free( got_ );                                               \
    }                                                               \
    while ( 0 )

int main( void )
{
    test_one( "Hopspop", "p", "-", "Ho-s-o-" );
    test_one( "Hops Pop", "p", "-", "Ho-s Po-" );
    test_one( "Hopspop", "p", "12", "Ho12s12o12" );
    test_one( "HoHo a PiPi", "Ho", "Pi", "PiPi a PiPi" );
    test_one( "HoHo a PiPi", "Ho", "-", "-- a PiPi" );

    test_one( "Karel Vana, Karel Hoptysek a Karel \"Homosexual\" Hudlecka",
              "Karel",
              "0",
              "0 Vana, 0 Hoptysek a 0 \"Homosexual\" Hudlecka" );

    test_one( "Karel Vana, Karel Hoptysek a Karel \"Homosexual\" Hudlecka",
              "Homosexual",
              "Mistr v prevlekani",
              "Karel Vana, Karel Hoptysek a Karel \"Mistr v prevlekani\" Hudlecka" );

    test_one( "Hopspop", "p", "", "Hoso" );
    test_one( "Hopspop", "Hopspop", "Hopspop", "Hopspop" );
    test_one( "Hopspop", "Hopspop", "Hopcpop", "Hopcpop" );
    test_one( "Hopspop", "P", "_", "Hopspop" );

    assert_that( string_replaced( "To je jedno", "", "tohle taky" ) == NULL );
}
