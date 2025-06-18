#ifndef TEST_SETS_H
#define TEST_SETS_H

#include "../../src/headers/assert_that.h"
#include "../../src/headers/unit_tests.h"
#include "../../src/structs/set.h"

TEST( set_init )
{
    struct hash_set *set = set_init();
    UNIT_TEST( set != NULL );
    set_destroy( set );
}
END_TEST

TEST( set_insert )
{
    struct hash_set *set = set_init();
    assert_that( set != NULL, "init failed" );

    int number = 1;
    UNIT_TEST( set_insert( set, &number, sizeof number ) == SETINSERT_INSERTED );

    bool insert_SET_DEFAULT_CAP = true;
    for ( int i = 0; i < SET_DEFAULT_CAP; ++i )
    {
        if ( i == 1 )
            insert_SET_DEFAULT_CAP = insert_SET_DEFAULT_CAP &&
                                     set_insert( set, &i, sizeof i ) == SETINSERT_WAS_IN;
        else
            insert_SET_DEFAULT_CAP =
                    insert_SET_DEFAULT_CAP &&
                    set_insert( set, &i, sizeof i ) == SETINSERT_INSERTED;
    }
    UNIT_TEST( insert_SET_DEFAULT_CAP );

    bool insert_SET_4_DEFAULT_CAP = true;
    for ( int i = 0; i < SET_DEFAULT_CAP * 4; ++i )
    {
        if ( i < SET_DEFAULT_CAP )
            insert_SET_4_DEFAULT_CAP =
                    insert_SET_4_DEFAULT_CAP &&
                    set_insert( set, &i, sizeof i ) == SETINSERT_WAS_IN;
        else
            insert_SET_4_DEFAULT_CAP =
                    insert_SET_4_DEFAULT_CAP &&
                    set_insert( set, &i, sizeof i ) == SETINSERT_INSERTED;
    }
    UNIT_TEST( insert_SET_4_DEFAULT_CAP );

    bool search = true;
    for ( int i = 0; i < SET_DEFAULT_CAP * 4; ++i )
    {
        search = search && set_search( set, &i, sizeof i );
    }
    UNIT_TEST( search );

    number = -1;
    UNIT_TEST( !set_search( set, &number, sizeof number ) );

    set_destroy( set );
}
END_TEST


LibraryDefined void RUNALL_SETS( void )
{
    RUN_TEST( set_init );
    RUN_TEST( set_insert );
}

#endif
