//
// Created by MacBook on 08.01.2025.
//

#ifndef CLIBS_TEST_FOREACH_H
#define CLIBS_TEST_FOREACH_H

#include "../src/headers/assert_that.h"
#include "../src/headers/misc.h"
#include "../src/headers/unit_tests.h"
#include "../src/string_utils.h"
#include "../src/structs/dynarr.h"
#include "../src/structs/dynstring.h"
#include "../src/structs/queue.h"
#include "../src/structs/sets.h"

// must be after dynarr.h and sets.h
#include "../src/headers/foreach.h"
#include "../src/headers/pointer_utils.h"


Tester test_one_foreach_arr( const int64_t arr[], const size_t count )
{
    foreach_arr ( int64_t, num, arr, count )
    {
        if ( num != arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_ls( const struct dynamic_array *const numbers_ls,
                            const int64_t numbers_arr[],
                            const size_t count )
{
    assert_that( list_size( numbers_ls ) == count, "list size != array size" );

    foreach_ls ( int64_t, num, numbers_ls )
    {
        if ( num != numbers_arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_uni( const int64_t numbers_arr[], const size_t count )
{
    foreach_uni ( const int64_t,
                  num,
                  numbers_arr[ foreach_index_num ],
                  numbers_arr[ foreach_index_num ],
                  count )
    {
        if ( num != numbers_arr[ foreach_index_num ] )
            return false;
    }
    return true;
}
Tester test_one_foreach_str( const string_t str )
{
    foreach_str ( c, str )
    {
        if ( foreach_cap_c != strlen( str ) )
        {
            fflwarnx( "foreach_cap" );
            return false;
        }
        if ( c != str[ foreach_index_c ] )
        {
            fflwarnx( "c != str[ foreach_idx ]" );
            return false;
        }
    }
    return true;
}
Tester test_one_foreach_dynstr( string_t str )
{
    struct dynamic_string *dynstr = dynstr_init_as( str );
    assert_that( dynstr != NULL, "dynstr init" );
    assert_that( strlen( str ) == dynstr_len( dynstr ), "dynstr len" );

    foreach_str ( c, dynstr_data( dynstr ) )
    {
        if ( foreach_cap_c != strlen( str ) )
        {
            fflwarnx( "foreach_cap" );
            dynstr_destroy( dynstr );
            return false;
        }
        if ( c != str[ foreach_index_c ] )
        {
            fflwarnx( "c != str[ foreach_idx ]" );
            dynstr_destroy( dynstr );
            return false;
        }
    }
    dynstr_destroy( dynstr );
    return true;
}

TEST( foreach )
{
    struct dynamic_array *numbers_ls = list_init_type( int64_t );
    assert_that( numbers_ls != NULL, "list init" );
    const int64_t numbers_arr[] = {
        1, 2, 4, 6, 7, -1, 2323, 3195,
    };
    assert_that( list_extend( numbers_ls, numbers_arr, countof( numbers_arr ) ) ==
                         RV_SUCCESS,
                 "list extend" );

    UNIT_TEST( test_one_foreach_arr( numbers_arr, countof( numbers_arr ) ) );
    UNIT_TEST( test_one_foreach_ls( numbers_ls, numbers_arr, countof( numbers_arr ) ) );
    list_destroy( numbers_ls );
    UNIT_TEST( test_one_foreach_uni( numbers_arr, countof( numbers_arr ) ) );

    UNIT_TEST( test_one_foreach_str( "" ) );
    UNIT_TEST( test_one_foreach_str( "HOVNO" ) );
    UNIT_TEST( test_one_foreach_str( "a\nb'\0'" ) );

    UNIT_TEST( test_one_foreach_dynstr( "" ) );
    UNIT_TEST( test_one_foreach_dynstr( "HOVNO" ) );
    UNIT_TEST( test_one_foreach_dynstr( "a\nb'\0'" ) );
}
END_TEST


TEST( foreach_set )
{
    Set *const set = set_init();
    assert_that( set != NULL, "set init" );

    assert_that( set_insert( set, &set, sizeof set ) == SETINSERT_INSERTED,
                 "couldn't insert set" );
    const int num = 1423;
    assert_that( set_insert( set, &num, sizeof num ) == SETINSERT_INSERTED,
                 "couldn't insert num" );

    // check sanity
    assert_that( set_insert( set, &set, sizeof set ) == SETINSERT_WAS_IN,
                 "collision blocking not working" );
    assert_that( set_size( set ) == 2, "two items must be present" );
    assert_that( sizeof( Set * ) != sizeof( int ), "test assumes this" );

    foreach_set ( set )
    {
        UNIT_TEST( entry.index >= 0 );
        const struct set_item *item = entry.item;
        assert_that( item != NULL && item->data != NULL, "entry item" );

        UNIT_TEST( item->size == sizeof( Set * ) || item->size == sizeof( int ) );

        if ( item->size == sizeof( Set * ) )
            UNIT_TEST( deref_as( Set *, item->data ) == set );
        else if ( item->size == sizeof( int ) )
            UNIT_TEST( deref_as( int, item->data ) == num );
    }

    set_destroy( set );
}
END_TEST


TEST( foreach_queue )
{
    struct fifo_queue *const queue = queue_init( sizeof( int ) );
    assert_that( queue != NULL, "queue init" );

    for ( int i = 16; i > 0; --i )
        assert_that( queue_enqueue( queue, &i ) == RV_SUCCESS, "enqueue" );
    assert_that( queue_get_size( queue ) == 16, );

    foreach_que( queue )
    {
        const int data = deref_as( int, entry.data );
        UNIT_TEST( data == ( int ) ( 16 - foreach_index_entry ) );
    }

    queue_destroy( queue );
}
END_TEST


#endif //CLIBS_TEST_FOREACH_H
