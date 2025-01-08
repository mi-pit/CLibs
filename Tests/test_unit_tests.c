//
// Created by MacBook on 03.01.2025.
//

#include "../misc.h"
#include "../unit_tests.h"

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
}
END_TEST


int main( void )
{
    int a, b, c, d;
    UNUSED( a, b, c, d );

    RUN( all_fail );
    RUN( test_example );
    RUN( this_one_actually_passes );

    FINISH_TESTING();
}
