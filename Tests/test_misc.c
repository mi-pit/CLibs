//
// Created by MacBook on 03.01.2025.
//

#include "../run_test.h"


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

int main( void )
{
    RUN( all_fail );
    RUN( test_example );

    FINISH_TESTING();
}
