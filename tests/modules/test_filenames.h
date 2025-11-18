//
// Created by Michal Pitner on 08.07.2025.
//

#ifndef TEST_FILENAMES_H
#define TEST_FILENAMES_H

#include "../../src/headers/unit_tests.h"


#ifdef CLIBS_TESTS_MAIN
TEST( get_prog_name )
{
    UNIT_TEST( strcmp( get_prog_name(), "tests_sanitizers" ) == 0 );
}
END_TEST
#endif


TEST( get_file_name )
{
    UNIT_TEST( strcmp( get_file_name( "/Users/macbook/Hovno" ), "Hovno" ) == 0 );
    UNIT_TEST( strcmp( get_file_name( "Hovno" ), "Hovno" ) == 0 );
    UNIT_TEST( strcmp( get_file_name( "/a" ), "a" ) == 0 );
    UNIT_TEST( strcmp( get_file_name( "./a" ), "a" ) == 0 );
    UNIT_TEST( strcmp( get_file_name( "Directory/Subdir/" ), "Subdir/" ) == 0 );

    UNIT_TEST( strcmp( get_file_name( "/" ), "/" ) == 0 );
}
END_TEST


LibraryDefined void RUNALL_FILENAMES( void )
{
#ifdef CLIBS_TESTS_MAIN
    RUN_TEST( get_prog_name );
#endif
    RUN_TEST( get_file_name );
}


#endif //TEST_FILENAMES_H
