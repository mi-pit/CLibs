//
// Created by Michal Pitner on 23.06.2025.
//

#include "../src/headers/ctrlflow/assert_that.h"

int main( void )
{
    assert( true );
    assert_that( true, "testing success, not exitting or printing" );
    assert_that( 1 == 2, "testing failure, exitting :)" );
    assert( false );
}
