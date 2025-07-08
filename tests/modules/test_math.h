//
// Created by Michal Pitner on 21.06.2025.
//

#ifndef TEST_MATH_H
#define TEST_MATH_H

#include "../../src/headers/simple_math.h"
#include "../../src/headers/unit_tests.h"

TEST( powers_of_two )
{
    UNIT_TEST( is_power_of_two( 1 ) );
    UNIT_TEST( is_power_of_two( 2 ) );
    UNIT_TEST( !is_power_of_two( 3 ) );
    UNIT_TEST( is_power_of_two( 4 ) );
    UNIT_TEST( is_power_of_two( 8 ) );
    UNIT_TEST( !is_power_of_two( 9 ) );

    UNIT_TEST( !is_power_of_two( 0 ) );
}
END_TEST

LibraryDefined void RUNALL_MATH( void )
{
    RUN_TEST( powers_of_two );
}

#endif //TEST_MATH_H
