//
// Created by MacBook on 03.01.2025.
//

#include "../assert_that.h"
#include "../dynstring.h"
#include "../run_test.h"
#include "../string_utils.h"

bool test_one( string_t orig, string_t old, string_t new, string_t expected )
{
    str_t got = string_replaced( orig, old, new );
    bool rv   = strcmp( got, expected ) == 0;
    free( got );

    return rv;
}

TEST( replace )
{
    UNIT_TEST( test_one( "Hopspop", "p", "-", "Ho-s-o-" ) );
    UNIT_TEST( test_one( "Hops Pop", "p", "-", "Ho-s Po-" ) );
    UNIT_TEST( test_one( "Hopspop", "p", "12", "Ho12s12o12" ) );
    UNIT_TEST( test_one( "HoHo a PiPi", "Ho", "Pi", "PiPi a PiPi" ) );

    UNIT_TEST( test_one( "HoHo a PiPi", "Ho", "-", "-- a PiPi" ) );

    UNIT_TEST( test_one( "Karel Vana, Karel Hoptysek a Karel \"Homosexual\" Hudlecka",
                         "Karel",
                         "0",
                         "0 Vana, 0 Hoptysek a 0 \"Homosexual\" Hudlecka" ) );

    UNIT_TEST( test_one(
            "Karel Vana, Karel Hoptysek a Karel \"Homosexual\" Hudlecka",
            "Homosexual",
            "Mistr v prevlekani",
            "Karel Vana, Karel Hoptysek a Karel \"Mistr v prevlekani\" Hudlecka" ) );


    UNIT_TEST( test_one( "Hopspop", "p", "", "Hoso" ) );
    UNIT_TEST( test_one( "Hopspop", "Hopspop", "Hopspop", "Hopspop" ) );
    UNIT_TEST( test_one( "Hopspop", "Hopspop", "Hopcpop", "Hopcpop" ) );
    UNIT_TEST( test_one( "Hopspop", "P", "_", "Hopspop" ) );

    UNIT_TEST( string_replaced( "To je jedno", "", "tohle taky" ) == NULL );
}
END_TEST

int main( void )
{
    RUN( replace );
}
