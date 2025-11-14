#include "set.h"

#include "../headers/cmp.h" /* cmp_size_t */
#include "dynarr.h"

#include <assert.h>   /* assert */
#include <inttypes.h> /* PRIi64 */
#include <stdio.h>    /* print */
#include <stdlib.h>   /* malloc */
#include <string.h>   /* memcpy */


#define SET_DEFAULT_CAP 64


struct hash_set {
    size_t n_items;
    size_t capacity;
    struct set_item *items;
};

size_t set_size( const Set *set )
{
    return set->n_items;
}


SetEnumeratedEntry set_get_next( const Set *set, const int64_t index_last )
{
    if ( index_last < -1 )
    {
        fwarnx( "index must not be negative except -1 for initialization" );
        return ( SetEnumeratedEntry ) { .item = NULL, .index = -2 };
    }
    if ( index_last >= 0 && ( size_t ) index_last >= set->capacity )
    {
        fwarnx( "index %" PRIi64 " out of bounds for set of length %zu", index_last,
                set->capacity );

        return ( SetEnumeratedEntry ) { .item = NULL, .index = -2 };
    }

    for ( size_t i = index_last + 1; i < set->capacity; ++i )
        if ( set->items[ i ].data != NULL )
            return ( SetEnumeratedEntry ) {
                .item  = ( set->items + i ),
                .index = ( int64_t ) i,
            };

    return ( SetEnumeratedEntry ) { .item = NULL, .index = -1 };
}


static int set_item_cmp( const void *s1, const void *s2 )
{
    const struct set_item *item1 = s1;
    const struct set_item *item2 = s2;

    if ( item1->size != item2->size )
        return cmp_size_t( &item1->size, &item2->size );
    return memcmp( item1->data, item2->data, item1->size );
}

Private int /* NOLINT(*-no-recursion) */ set_insert_item( Set *set,
                                                          const struct set_item *item )
{
    return set_insert_f( set, item->data, item->size, item->func );
}


/* resize -> insert_item -> insert_f -> resize -\
 *  <-------------------------------------------/
 *
 * assert() prevents potential infinite recursion
 */
Private int set_resize( Set *set, const size_t new_size ) // NOLINT(*-no-recursion)
{
    Set *new_set = set_init_cap( new_size );
    if ( new_set == NULL )
        return RV_ERROR;

    for ( size_t i = 0; i < set->capacity; ++i )
    {
        const struct set_item *item = set->items + i;
        if ( item->data == NULL )
            continue;

        const int ins_rv = set_insert_item( new_set, item );
        if ( ins_rv < 0 )
        {
            set_destroy( new_set );
            return ins_rv;
        }
        assert( ins_rv == SETINSERT_INSERTED );
        free( item->data );
    }

    free( set->items );

    memcpy( set, new_set, sizeof( struct hash_set ) );

    free( new_set );

    return RV_SUCCESS;
}


Set *set_init_cap( const size_t capacity )
{
    Set *new_set = calloc( 1, sizeof( Set ) );
    if ( new_set == NULL )
        return fflwarn_ret( NULL, "calloc" );

    if ( ( new_set->items = calloc( capacity, sizeof( struct set_item ) ) ) == NULL )
    {
        free( new_set );
        return fflwarn_ret( NULL, "calloc" );
    }

    new_set->capacity = capacity;

    for ( size_t i = 0; i < new_set->capacity; ++i )
        new_set->items[ i ].func = ITEM_PRINT_FUNCTION_NAME( byte );

    return new_set;
}

Set *set_init( void )
{
    return set_init_cap( SET_DEFAULT_CAP );
}


/**
 * If the element is not already in, the function creates a shallow copy of the data
 * @param set
 * @param data
 * @param len
 * @param func
 * @return
 */
/* NOLINT(*-no-recursion) */ int set_insert_f( Set *set, const void *data, size_t len,
                                               const PrintFunction func )
{
    if ( set->n_items + 1 >= set->capacity )
        if ( set_resize( set, set->capacity * 2 ) )
            return RV_ERROR;

    const size_t index = hash_func( data, len );

    const struct set_item new = {
        .size = len, .data = ( void * ) data,
        /* data is still const */
    };

    for ( size_t i = 0; i < set->capacity; ++i )
    {
        const size_t current_index = ( index + i ) % set->capacity;
        struct set_item *curr      = set->items + current_index;

        if ( set_item_cmp( &new, curr ) == 0 )
            return SETINSERT_WAS_IN;

        if ( curr->data != NULL )
            continue;

        // elements rightful place is empty => insert
        curr->data = malloc( len );
        if ( curr->data == NULL )
            return fwarn_ret( RV_ERROR, "malloc" );

        memcpy( curr->data, data, len );
        curr->size    = len;
        curr->removed = false;
        curr->func    = func;

        ++set->n_items;

        return SETINSERT_INSERTED;
    }

    return fflwarnx_ret( RV_ERROR, "unrecognized exception; something went wrong" );
}

int set_insert( Set *set, const void *data, const size_t len )
{
    return set_insert_f( set, data, len, ITEM_PRINT_FUNCTION_NAME( byte ) );
}

Private int set_insert_array( Set *set, const size_t len,
                              const struct set_item set_item_array[ len ] )
{
    for ( size_t i = 0; i < len; ++i )
    {
        const struct set_item *item = set_item_array + i;
        if ( item->data != NULL )
            if ( set_insert_item( set, item ) == RV_ERROR )
                return RV_ERROR;
    }
    return RV_SUCCESS;
}


int set_remove( Set *set, const void *data, const size_t len )
{
    const size_t index = hash_func( data, len ) % set->capacity;

    const struct set_item to_remove = {
        .size = len, .data = ( void * ) data,
        /* data should still be const */
    };

    for ( size_t i = 0; i < set->capacity; ++i )
    {
        const size_t current_index = ( index + i ) % set->capacity;
        struct set_item *curr      = set->items + current_index;

        if ( set_item_cmp( &to_remove, curr ) == 0 )
        {
            free( curr->data );

            curr->data    = NULL;
            curr->size    = 0;
            curr->removed = true;

            --set->n_items;

            if ( set->n_items <= set->capacity / 4 )
                if ( set_resize( set, set->capacity / 2 ) )
                    // [ a, b, _, _, _, _, _, _ ] => [ a, b, _, _ ]
                    return RV_ERROR;

            return SETREMOVE_REMOVED;
        }

        if ( curr->data == NULL && !curr->removed )
            break;
    }

    return SETREMOVE_NOT_FOUND;
}

Private int set_remove_item( Set *set, const struct set_item *item )
{
    return set_remove( set, item->data, item->size );
}

int set_remove_array( Set *set, const size_t len,
                      const struct set_item set_item_array[ len ] )
{
    for ( size_t i = 0; i < len; ++i )
    {
        const struct set_item *item = set_item_array + i;
        if ( item->data != NULL )
        {
            if ( set_remove_item( set, item ) == RV_ERROR )
                return RV_ERROR;
        }
    }
    return RV_SUCCESS;
}


bool set_search( const Set *set, const void *data, const size_t len )
{
    const size_t index = hash_func( data, len ) % set->capacity;

    const struct set_item new = {
        .size = len, .data = ( void * ) data,
        /* data is still const */
    };

    for ( size_t i = 0; i < set->capacity; ++i )
    {
        const size_t current_index  = ( index + i ) % set->capacity;
        const struct set_item *curr = set->items + current_index;

        if ( set_item_cmp( &new, curr ) == 0 )
            return true;
    }

    return false;
}

Private bool set_search_item( const Set *set, const struct set_item *item )
{
    return set_search( set, item->data, item->size );
}


int set_union( const Set *set_1, const Set *set_2, Set **result )
{
    if ( *result == NULL )
        *result = set_init_cap( set_1->capacity + set_2->capacity );

    const int rv = set_insert_array( *result, set_2->capacity, set_2->items );
    if ( rv < 0 )
        return rv;
    return set_insert_array( *result, set_1->capacity, set_1->items );
}

int set_unionize( Set *set, const Set *add )
{
    return set_insert_array( set, add->capacity, add->items );
}


int set_intersection( const Set *set_1, const Set *set_2, Set **result )
{
    if ( *result == NULL )
        *result = set_init_cap( set_1->capacity + set_2->capacity );
    if ( *result == NULL )
        return RV_ERROR;

    for ( size_t i = 0; i < set_2->capacity; ++i )
    {
        const struct set_item *item = set_2->items + i;
        if ( item->data == NULL )
            continue;

        if ( !set_search_item( set_1, item ) )
            continue;

        if ( set_insert_item( *result, item ) < 0 )
            return RV_ERROR;
    }

    return RV_SUCCESS;
}

int set_intersect( Set *set, const Set *intr )
{
    for ( size_t i = 0; i < intr->capacity; ++i )
        if ( set->items->data != NULL )
            if ( !set_search_item( intr, set->items + i ) )
                if ( set_remove_item( set, set->items + i ) == RV_ERROR )
                    return RV_ERROR;
    return RV_SUCCESS;
}


int set_difference( const Set *set, const Set *sub, Set **result )
{
    if ( *result == NULL )
        *result = set_init_cap( set->capacity );
    if ( *result == NULL )
        return RV_ERROR;

    for ( size_t i = 0; i < set->capacity; ++i )
    {
        const struct set_item *item = set->items + i;
        if ( item->data == NULL )
            continue;

        if ( set_search_item( sub, item ) )
            continue;

        if ( set_insert_item( *result, item ) == RV_ERROR )
            return RV_ERROR;
    }

    return RV_SUCCESS;
}

int set_subtract( Set *set, const Set *sub )
{
    return set_remove_array( set, sub->capacity, sub->items );
}


int set_cmp( const Set *set_1, const Set *set_2 )
{
    const int cmp = cmp_size_t( &set_1->n_items, &set_2->n_items );
    if ( cmp != 0 )
        return cmp;

    for ( size_t i = 0; i < set_1->capacity; ++i )
    {
        const struct set_item *item_1 = set_1->items + i;
        const struct set_item *item_2 = set_2->items + i;
        if ( !set_search_item( set_2, item_1 ) )
            return 1;
        if ( !set_search_item( set_1, item_2 ) )
            return -1;
    }

    return 0;
}


Set *set_from_list( const List *list )
{
    Set *const new_set = set_init_cap( list_size( list ) );
    if ( new_set == NULL )
        return f_stack_trace( NULL );

    const size_t el_size = list_el_size( list );
    for ( size_t i = 0; i < list_size( list ); ++i )
    {
        const void *const data = list_see( list, i );

        switch ( set_insert( new_set, data, el_size ) )
        {
            case SETINSERT_WAS_IN:
                // fflwarnx( "Actually found a collision in a set!" );
            case SETINSERT_INSERTED:
                // don't care
                break;

            case RV_ERROR:
            case RV_EXCEPTION:
                return f_stack_trace( NULL );
        }
    }

    return new_set;
}


void set_destroy( Set *set )
{
    for ( size_t i = 0; i < set->capacity; ++i )
        free( set->items[ i ].data );
    free( set->items );
    free( set );
}


void set_print_as( const Set *set, const PrintFunction func )
{
    printf( "hash_set (size=%zu): {", set->n_items );

    size_t n = 0;
    for ( size_t i = 0; i < set->capacity; ++i )
    {
        const struct set_item *item = set->items + i;
        if ( item->data == NULL )
            continue;

        if ( n == 0 )
            printf( set->n_items > SET_ITEMCOUNT_LINE_LENGTH ? "\n\t" : " " );
        else if ( n % SET_ITEMCOUNT_LINE_LENGTH == 0 )
            printf( set->n_items > SET_ITEMCOUNT_LINE_LENGTH ? ",\n\t" : ", " );
        else
            printf( item->size > 16 ? ",\n\t" : ", " );

        // print the data
        ( func != NULL ? func : item->func )( item->data, item->size );

        ++n;
    }

    printf( "%s}\n", set->n_items > SET_ITEMCOUNT_LINE_LENGTH ? "\n" : " " );
}

void set_print( const Set *set )
{
    set_print_as( set, NULL );
}
