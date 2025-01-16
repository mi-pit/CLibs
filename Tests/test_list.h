//
// Created by MacBook on 12.01.2025.
//

#ifndef CLIBS_TEST_LIST_H
#define CLIBS_TEST_LIST_H

#include "../attributes.h"
#include "../Structs/dynarr.h"
#include "../unit_tests.h"


Tester test_list_array_uint64( struct dynamic_array *ls,
                               size_t size,
                               const uint64_t array[ size ] )
{
    for ( size_t i = 0; i < size; ++i )
        if ( array[ i ] != ( ( uint64_t * ) list_items( ls ) )[ i ] )
            return false;
    return true;
}

Private TEST( list_init )
{
    struct dynamic_array *ls = list_init_type( uint64_t );
    UNIT_TEST( list_el_size( ls ) == sizeof( uint64_t ) );
    UNIT_TEST( list_size( ls ) == 0 );
    UNIT_TEST( list_items( ls ) != NULL );

    uint64_t int_arr[ DEFAULT_DYNSTRING_CAP * 8 ] = { 0 };
    for ( uint64_t i = 0; i < DEFAULT_DYNSTRING_CAP * 8; ++i )
    {
        int_arr[ i ] = i;
        assert( list_append( ls, &i ) == RV_SUCCESS );
        UNIT_TEST( list_el_size( ls ) == sizeof( uint64_t ) && list_size( ls ) == i + 1
                   && list_items( ls ) != NULL );

        UNIT_TEST( test_list_array_uint64( ls, list_size( ls ), int_arr ) );
    }
    for ( uint64_t i = 0; i < DEFAULT_DYNSTRING_CAP * 8; ++i )
    {
        assert( list_pop( ls, NULL ) == RV_SUCCESS );
        UNIT_TEST( list_el_size( ls ) == sizeof( uint64_t )
                   && list_size( ls ) == DEFAULT_DYNSTRING_CAP * 8 - i - 1
                   && list_items( ls ) != NULL );

        UNIT_TEST( test_list_array_uint64( ls, list_size( ls ), int_arr ) );
    }
}
END_TEST

#endif //CLIBS_TEST_LIST_H
