//
// Created by MacBook on 08.01.2025.
//

#ifndef CLIBS_TEST_SWEX_H
#define CLIBS_TEST_SWEX_H

#include "../src/Dev/unit_tests.h"
#include "../src/string_utils.h"
#include "../src/swexpr.h"

static int get_swex_val( int branch_1, int branch_2, string_t branch_3 )
{
    swex_init_val( int, branch_1 ) as_new( int, rv )
    {
        swex_case_imm( int, 1 )
        {
            swex_init_val( int, branch_2 ) as( rv )
            {
                swex_case_imm( int, 1 )
                {
                    resolve( int, 10 );
                }
                swex_case_imm( int, 2 )
                {
                    swex_init_str( branch_3 ) as( rv )
                    {
                        swex_case_str( "1" )
                        {
                            resolve( int, 100 );
                        }
                        swex_default()
                        {
                            resolve( int, -100 );
                        }
                    }
                    swex_finish();
                }
                swex_default()
                {
                    resolve( int, -10 );
                }
            }
            swex_finish();
        }
        swex_case_imm( int, 2 )
        {
            resolve( int, 2 );
        }

        swex_default()
        {
            resolve( int, -1 );
        }
    }
    swex_finish();

    return rv;
}
Tester test_one_swex( int branch_1, int branch_2, string_t branch_3, int rv )
{
    return get_swex_val( branch_1, branch_2, branch_3 ) == rv;
}

TEST( swex )
{
    UNIT_TEST( test_one_swex( -1, 0, NULL, -1 ) );
    UNIT_TEST( test_one_swex( -1, 124, "hops", -1 ) );
    UNIT_TEST( test_one_swex( -100, 0, NULL, -1 ) );

    UNIT_TEST( test_one_swex( 2, 0, NULL, 2 ) );
    UNIT_TEST( test_one_swex( 2, 325, "NULL", 2 ) );

    UNIT_TEST( test_one_swex( 1, 1, NULL, 10 ) );
    UNIT_TEST( test_one_swex( 1, 1, "HOPSOPPO", 10 ) );

    UNIT_TEST( test_one_swex( 1, -11, NULL, -10 ) );

    UNIT_TEST( test_one_swex( 1, 2, "1", 100 ) );
    UNIT_TEST( test_one_swex( 1, 2, "Hovno", -100 ) );
}
END_TEST

#endif //CLIBS_TEST_SWEX_H
