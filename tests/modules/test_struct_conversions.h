#ifndef CLIBS_TEST_STRUCT_CONVERSIONS_H
#define CLIBS_TEST_STRUCT_CONVERSIONS_H

#define CLIBS_STRUCT_CONVERSIONS

#include "../../src/headers/assert_that.h"
#include "../../src/headers/unit_tests.h"
#include "../../src/structs/dynarr.h"
#include "../../src/structs/set.h"


TEST( set_from_list )
{
    List *ls = list_init_type( int );
    assert_that( ls != NULL, );

    for ( int i = 0; i < 123; ++i )
        assert_that( list_append( ls, &i ) == RV_SUCCESS, );

    Set *set = set_from_list( ls );
    CRITICAL_TEST( set != NULL );

    //set_print_as( set, Print_int );

    bool v = GET_UNIT_TEST_VERBOSITY();
    SET_UNIT_TEST_VERBOSITY( false );
    for ( int i = 0; i < 123; ++i )
        UNIT_TEST( set_search( set, &i, sizeof i ) );

    int i = 124;
    UNIT_TEST( !set_search( set, &i, sizeof i ) );

    i = -1;
    UNIT_TEST( !set_search( set, &i, sizeof i ) );

    SET_UNIT_TEST_VERBOSITY( v );
}
END_TEST


LibraryDefined void RUNALL_STRUCT_CONVERSIONS( void )
{
    RUN_TEST( set_from_list );
}


#endif //CLIBS_TEST_STRUCT_CONVERSIONS_H
