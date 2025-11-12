#ifndef CLIBS_TEST_STRUCT_CONVERSIONS_H
#define CLIBS_TEST_STRUCT_CONVERSIONS_H


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

    const int v = GET_UNIT_TEST_VERBOSITY();
    SET_UNIT_TEST_VERBOSITY( UNIT_TESTS_YAP_NONE );
    for ( int i = 0; i < 123; ++i )
        UNIT_TEST( set_search( set, &i, sizeof i ) );

    int i = 124;
    UNIT_TEST( !set_search( set, &i, sizeof i ) );

    i = -1;
    UNIT_TEST( !set_search( set, &i, sizeof i ) );

    SET_UNIT_TEST_VERBOSITY( v );

    list_destroy( ls );
    set_destroy( set );
}
END_TEST

TEST( list_from_set )
{
    Set *set = set_init();
    assert_that( set, );

    for ( int i = 0; i < 123; ++i )
        assert_that( set_insert( set, &i, sizeof i ) == SETINSERT_INSERTED, );

    List *ls = list_from_set( set );
    CRITICAL_TEST( ls != NULL );
    UNIT_TEST( list_el_size( ls ) == sizeof( int ) );

    list_sort( ls, cmp_int );
    for ( int i = 0; i < 123; ++i )
        UNIT_TEST( list_fetch( ls, i, int ) == i );

    list_destroy( ls );
    set_destroy( set );
}
END_TEST

TEST( list_from_queue )
{
    Queue *q = queue_init( sizeof( int ) );
    CRITICAL_TEST( q );

    for ( int i = 0; i < 123; ++i )
        CRITICAL_TEST( queue_enqueue( q, &i ) == RV_SUCCESS );

    CRITICAL_TEST( queue_get_size( q ) == 123 );

    List *ls = list_from_queue( q );
    CRITICAL_TEST( ls != NULL );

    for ( int i = 0; i < 123; ++i )
        UNIT_TEST( list_fetch( ls, i, int ) == i );
}
END_TEST


LibraryDefined void RUNALL_STRUCT_CONVERSIONS( void )
{
    RUN_TEST( set_from_list );
    RUN_TEST( list_from_set );
    RUN_TEST( list_from_queue );
}


#endif //CLIBS_TEST_STRUCT_CONVERSIONS_H
