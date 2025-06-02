//
// Created by MacBook on 03.01.2025.
//

#include "../Dev/errors.h"
#include "../Dev/unit_tests.h"
#include "../misc.h"

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
    UNIT_TEST( f_stack_trace( -1 ) == ( int ) -1 );
}
END_TEST


int main( void )
{
    int a, b, c, d;
    UNUSED( a );
    UNUSED( b );
    UNUSED( c );
    UNUSED( d );

    RUN_TEST( all_fail );
    RUN_TEST( test_example );
    RUN_TEST( this_one_actually_passes );

    f_stack_trace( 0 );

    FINISH_TESTING();
}
