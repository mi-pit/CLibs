//
// Created by MacBook on 03.01.2025.
//

#include "../src/headers/errors.h"
#include "../src/headers/misc.h"
#include "../src/headers/unit_tests.h"

#include <string.h>


TEST( all_fail )
{
    UNIT_TEST( false );
    UNIT_TEST( fopen( "nonexistent", "r" ) != NULL );
    UNIT_TEST( fopen( "another nonexistent", "r" ) != NULL );
}
END_TEST

TEST( test_example )
{
    UNIT_TEST( 1 == 1 );
    UNIT_TEST( 1 == 2 );
    UNIT_TEST( 2 == 1 );
    UNIT_TEST( 2 == 2 );
}
END_TEST

TEST( this_one_actually_passes )
{
    UNIT_TEST( true );

    UNIT_TEST( fwarnx_ret( 123, "Testing warn & stack trace" ) == 123 );
    UNIT_TEST( f_stack_trace( -1 ) == -1 );
}
END_TEST

TEST( loop )
{
    {
        const bool no_loop = true;
        UNIT_TEST( no_loop );
    }

    SET_UNIT_TEST_VERBOSITY( false );
    const bool this_one_should_show = false;
    UNIT_TEST( this_one_should_show );
    {
        const bool this_one_shouldnt_show = true;
        for ( int i = 0; i < 10; ++i )
            UNIT_TEST( this_one_shouldnt_show );
    }
    SET_UNIT_TEST_VERBOSITY( true );

    {
        const bool loop_finished = true;
        UNIT_TEST( loop_finished );
    }
}
END_TEST


int main( void )
{
    RUN_TEST( all_fail );
    RUN_TEST( test_example );
    RUN_TEST( this_one_actually_passes );
    RUN_TEST( loop );

    f_stack_trace( 0 );

    FINISH_TESTING();
}
