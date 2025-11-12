//
// Created by MacBook on 03.01.2025.
//

#include "../src/headers/errors.h"
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

TEST( loop_setting_verbosity )
{
    {
        const bool no_loop = true;
        UNIT_TEST( no_loop );
    }

    int old_v = GET_UNIT_TEST_VERBOSITY();
    SET_UNIT_TEST_VERBOSITY( UNIT_TESTS_YAP_NONE );
    const bool this_one_should_show = false;
    UNIT_TEST( this_one_should_show );
    {
        const bool this_one_shouldnt_show = true;
        for ( int i = 0; i < 10; ++i )
            UNIT_TEST( this_one_shouldnt_show );
    }
    SET_UNIT_TEST_VERBOSITY( old_v );

    {
        const bool loop_finished = true;
        UNIT_TEST( loop_finished );
    }
}
END_TEST

TEST( critical )
{
    UNIT_TEST( true );
    CRITICAL_TEST( true );
    CRITICAL_TEST( false );

    UNIT_TEST( !"This one shouldn't run" );
}
END_TEST

TEST( really_long_messages_test_hopshops_test_test_kunker_lager )
{
    UNIT_TEST( !"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam in augue "
                "sed diam cursus pulvinar eu id massa. Vestibulum dignissim aliquet "
                "feugiat. Cras feugiat velit a elit sollicitudin, ac molestie." );
}
END_TEST


bool run_all( void )
{
    RUN_TEST( all_fail );
    RUN_TEST( test_example );
    RUN_TEST( this_one_actually_passes );
    RUN_TEST( loop_setting_verbosity );
    RUN_TEST( critical );
    RUN_TEST( really_long_messages_test_hopshops_test_test_kunker_lager );

    ( void ) f_stack_trace( 0 );

    return false;
}


int main( void )
{
    if ( !SET_UNIT_TEST_VERBOSITY( -1 ) )
        ( void ) f_stack_trace( 0 );

    SetTerminalColor( stdout, BACKGROUND_YELLOW );
    for ( int i = 0; i < LINE_PREF_WIDTH; ++i )
        printf( "%d", i % 10 );
    SetTerminalColor( stdout, COLOR_DEFAULT );
    printf( "\n" );

    run_all();
    ( void ) f_stack_trace( 0 );

    FINISH_TESTING();
}
