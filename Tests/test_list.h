//
// Created by MacBook on 12.01.2025.
//

#ifndef CLIBS_TEST_LIST_H
#define CLIBS_TEST_LIST_H

#include "../Dev/unit_tests.h" /* ... */
#include "../misc.h"           /* STRLEN */
#include "../Structs/dynarr.h" /* ... */


Tester test_list_array_uint64( const struct dynamic_array *ls,
                               size_t size,
                               const uint64_t array[ size ] )
{
    for ( size_t i = 0; i < size; ++i )
        if ( array[ i ] != ( ( uint64_t * ) list_items( ls ) )[ i ] )
            return false;
    return true;
}

TEST( list_init )
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
        if ( list_el_size( ls ) != sizeof( uint64_t ) || list_size( ls ) != i + 1 ||
             list_items( ls ) == NULL )
            break;

        if ( !test_list_array_uint64( ls, list_size( ls ), int_arr ) )
            break;
    }
    for ( uint64_t i = 0; i < DEFAULT_DYNSTRING_CAP * 8; ++i )
    {
        assert( list_pop( ls, NULL ) == RV_SUCCESS );
        if ( list_el_size( ls ) != sizeof( uint64_t ) ||
             list_size( ls ) != DEFAULT_DYNSTRING_CAP * 8 - i - 1 ||
             list_items( ls ) == NULL )
            break;

        if ( !test_list_array_uint64( ls, list_size( ls ), int_arr ) )
            break;
    }
    bool InitOverall = true;
    UNIT_TEST( InitOverall );

    list_destroy( ls );
}
END_TEST


Tester test_list_items( const List *ls, size_t count, const int array[ count ] )
{
    assert_that( list_el_size( ls ) == sizeof( int ), "list element size" );

    for ( size_t i = 0; i < count; ++i )
        if ( ( ( int * ) list_items( ls ) )[ i ] != array[ i ] )
            return false;

    return true;
}

#define test_list_items_m( LIST, COUNT, ... ) \
    test_list_items( LIST, COUNT, ( int[ COUNT ] ) { __VA_ARGS__ } )

TEST( list_basic )
{
    int array[ 100 ];
    for ( size_t i = 0; i < countof( array ); ++i )
        array[ i ] = ( int ) hash_func( &i, sizeof i );

    List *ls = list_init_type( int );
    assert_that( ls != NULL, "list init" );

    assert( list_insert( ls, 0, array + 0 ) == RV_SUCCESS );
    UNIT_TEST( test_list_items( ls, 1, ( int[ 1 ] ) { array[ 0 ] } ) );

    UNIT_TEST( list_insert( ls, 2, array + 10 ) == RV_EXCEPTION );
    UNIT_TEST( test_list_items( ls, 1, ( int[ 1 ] ) { array[ 0 ] } ) );

    assert( list_insert( ls, 0, array + 25 ) == RV_SUCCESS );
    UNIT_TEST( test_list_items( ls, 2, ( int[ 2 ] ) { array[ 25 ], array[ 0 ] } ) );

    assert( list_append( ls, array + 12 ) == RV_SUCCESS );
    UNIT_TEST( test_list_items( ls, 3,
                                ( int[ 3 ] ) { array[ 25 ], array[ 0 ], array[ 12 ] } ) );

    assert( list_pop( ls, NULL ) == RV_SUCCESS );
    UNIT_TEST( test_list_items( ls, 2, ( int[ 2 ] ) { array[ 25 ], array[ 0 ] } ) );

    assert( list_remove( ls, 1, NULL ) == RV_SUCCESS );
    UNIT_TEST( test_list_items( ls, 1, array + 25 ) );

    assert( list_remove_fast( ls, 0, NULL ) == RV_SUCCESS );
    UNIT_TEST( test_list_items( ls, 0, NULL ) );

    UNIT_TEST( list_pop( ls, NULL ) == RV_EXCEPTION );
    UNIT_TEST( test_list_items( ls, 0, NULL ) );

    assert( list_extend( ls, array + 50, 10 ) == RV_SUCCESS );
    UNIT_TEST( test_list_items( ls, 10, array + 50 ) );

    int removed;
    assert( list_remove_fast( ls, 5, &removed ) == RV_SUCCESS );
    UNIT_TEST( removed == array[ 55 ] );
    UNIT_TEST( test_list_items_m( ls,
                                  9,
                                  array[ 50 ],
                                  array[ 51 ],
                                  array[ 52 ],
                                  array[ 53 ],
                                  array[ 54 ],
                                  array[ 59 ],
                                  array[ 56 ],
                                  array[ 57 ],
                                  array[ 58 ] ) );

    UNIT_TEST( list_remove_fast( ls, list_size( ls ), &removed ) == RV_EXCEPTION );
    UNIT_TEST( removed == array[ 55 ] );

    assert( list_remove_fast( ls, list_size( ls ) - 1, &removed ) == RV_SUCCESS );
    UNIT_TEST( removed == array[ 58 ] );
    UNIT_TEST( test_list_items_m( ls,
                                  8,
                                  array[ 50 ],
                                  array[ 51 ],
                                  array[ 52 ],
                                  array[ 53 ],
                                  array[ 54 ],
                                  array[ 59 ],
                                  array[ 56 ],
                                  array[ 57 ] ) );

    assert( list_pop( ls, &removed ) == RV_SUCCESS );
    UNIT_TEST( removed == array[ 57 ] );
    UNIT_TEST( test_list_items_m( ls,
                                  7,
                                  array[ 50 ],
                                  array[ 51 ],
                                  array[ 52 ],
                                  array[ 53 ],
                                  array[ 54 ],
                                  array[ 59 ],
                                  array[ 56 ] ) );

    assert( list_set_at( ls, 1, NULL ) == RV_EXCEPTION );

    int set = 123;
    assert( list_set_at( ls, list_size( ls ), &set ) == RV_EXCEPTION );
    assert( list_set_at( ls, 1, &set ) == RV_SUCCESS );
    UNIT_TEST( test_list_items_m( ls, 7, array[ 50 ], set, array[ 52 ], array[ 53 ],
                                  array[ 54 ], array[ 59 ], array[ 56 ] ) );

    list_destroy( ls );
}
END_TEST

TEST( list_advanced )
{
#define STRING  "Hopspop Lorem ipsum, KOKOS!"
#define STRING2 "Hopspop lorem ipsum, KOKOS!"
    List *ls = list_init_type( char );
    assert( ls != NULL );

    UNIT_TEST( list_extend( ls, STRING, STRLEN( STRING ) ) == RV_SUCCESS );

    bool correct = true;
    for ( size_t i = 0; i < STRLEN( STRING ); ++i )
    {
        correct = list_fetch( ls, i, char ) == STRING[ i ];
        if ( !correct )
            break;
    }
    UNIT_TEST( correct );

    const string_t str = list_items( ls );
    UNIT_TEST( strcmp( str, STRING ) == 0 );

    list_access( ls, 8, char ) = 'l';
    UNIT_TEST( strcmp( str, STRING ) != 0 );
    UNIT_TEST( strcmp( str, STRING2 ) == 0 );

    list_access( ls, 8, char ) = 'L';
    UNIT_TEST( list_extend_list( ls, ls ) == RV_SUCCESS );
    UNIT_TEST( strcmp( str, STRING STRING ) == 0 );

    {
        char cont;
        UNIT_TEST( list_pop( ls, &cont ) == RV_SUCCESS );
        UNIT_TEST( cont == STRING[ STRLEN( STRING ) - 1 ] );

        UNIT_TEST( list_remove( ls, 0, &cont ) == RV_SUCCESS );
        UNIT_TEST( cont == STRING[ 0 ] );
    }

    UNIT_TEST( list_remove_fast( ls, SIZE_MAX, ( void * ) 1 ) == RV_EXCEPTION );
    UNIT_TEST( list_remove( ls, list_size( ls ), ( void * ) 1 ) == RV_EXCEPTION );

    list_printf_sde( ls, char, "%c", "\t\t> '" FOREGROUND_CYAN, "",
                     COLOR_DEFAULT "' <\n" );

    list_destroy( ls );
#undef STRING
#undef STRING2
}
END_TEST


#endif //CLIBS_TEST_LIST_H
