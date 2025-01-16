#include "sets.h"

#include "Errors/errors.h"
#include "misc.h"

#include <assert.h> /* assert */
#include <stdarg.h> /* va_list, ... */
#include <stdio.h>  /* print */
#include <stdlib.h> /* malloc */
#include <string.h> /* memcpy */


/**
 * Sets implemented via a hash table\n
 * Sets contain at most one copy of an item\n
 * \n
 * Items can be of any type -- they are treated as arrays of bytes.\n
 * Their keys are a combination of the number of bytes and each byte of the data
 *
 * @param n_items   : size_t; number of currently held items in the hash_set
 * @param capacity  : size_t; size of items array
 * @param items     : struct set_item *; array of type set_item
 */
struct hash_set {
    size_t n_items;
    size_t capacity;
    struct set_item *items;
};


static int set_item_cmp( const void *s1, const void *s2 )
{
    const struct set_item *item1 = s1;
    const struct set_item *item2 = s2;

    if ( item1->size != item2->size )
        return cmp_size_t( &item1->size, &item2->size );
    return memcmp( item1->data, item2->data, item1->size );
}


/*  resize -> insert_item -> insert_f -> resize -\
 *   <-------------------------------------------/  */
static int set_resize( Set set, size_t new_size )
{
    Set new_set = set_init_cap( new_size );
    if ( new_set == NULL )
        return RV_ERROR;

    for ( size_t i = 0; i < set->capacity; ++i )
    {
        struct set_item *item = set->items + i;
        if ( item->data == NULL )
            continue;

        int ins_rv = set_insert_item( new_set, item );
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


Set set_init_cap( size_t capacity )
{
    Set new_set = calloc( 1, sizeof( struct hash_set ) );
    if ( new_set == NULL )
        return fflwarn_ret_p( NULL, "calloc" );

    if ( ( new_set->items = calloc( capacity, sizeof( struct set_item ) ) ) == NULL )
    {
        free( new_set );
        return fflwarn_ret_p( NULL, "calloc" );
    }

    new_set->capacity = capacity;

    for ( size_t i = 0; i < new_set->capacity; ++i )
        new_set->items[ i ].func = print_byte;

    return new_set;
}

Set set_init( void )
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
int set_insert_f( Set set, const void *data, size_t len, PrintFunction func )
{
    if ( set->n_items + 1 >= set->capacity )
        if ( set_resize( set, set->capacity * 2 ) )
            return RV_ERROR;

    size_t index = hash_func( data, len );

    const struct set_item new = {
        .size = len, .data = ( void * ) data,
        /* data is still const */
    };

    for ( size_t i = 0; i < set->capacity; ++i )
    {
        size_t current_index  = ( ( index + i ) % set->capacity );
        struct set_item *curr = set->items + current_index;

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

    return RV_ERROR;
}

int set_insert( Set set, const void *data, size_t len )
{
    return set_insert_f( set, data, len, print_byte );
}

int set_insert_item( Set set, const struct set_item *item )
{
    return set_insert_f( set, item->data, item->size, item->func );
}

int set_insert_array( Set set, size_t len, const struct set_item set_item_array[ len ] )
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


int set_remove( Set set, const void *data, size_t len )
{
    size_t index = hash_func( data, len ) % set->capacity;

    const struct set_item to_remove = {
        .size = len, .data = ( void * ) data,
        /* data should still be const */
    };

    struct set_item *curr;
    for ( size_t i = 0; i < set->capacity; ++i )
    {
        size_t current_index = ( ( index + i ) % set->capacity );
        curr                 = set->items + current_index;

        if ( set_item_cmp( &to_remove, curr ) == 0 )
        {
            free( curr->data );

            curr->data    = NULL;
            curr->size    = 0;
            curr->removed = true;

            --set->n_items;

            if ( set->n_items < set->capacity / 4 )
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

int set_remove_item( Set set, const struct set_item *item )
{
    return set_remove( set, item->data, item->size );
}

int set_remove_array( Set set, size_t len, const struct set_item set_item_array[ len ] )
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


bool set_search( ConstSet set, const void *data, size_t len )
{
    size_t index = hash_func( data, len ) % set->capacity;

    const struct set_item new = {
        .size = len, .data = ( void * ) data,
        /* data is still const */
    };

    struct set_item *curr;
    for ( size_t i = 0; i < set->capacity; ++i )
    {
        size_t current_index = ( ( index + i ) % set->capacity );
        curr                 = set->items + current_index;

        if ( set_item_cmp( &new, curr ) == 0 )
            return true;
    }

    return false;
}

bool set_search_item( ConstSet set, const struct set_item *item )
{
    return set_search( set, item->data, item->size );
}


int set_union( ConstSet set_1, ConstSet set_2, Set *result )
{
    if ( *result == NULL )
        *result = set_init_cap( set_1->capacity + set_2->capacity );

    int rv = set_insert_array( *result, set_2->capacity, set_2->items );
    if ( rv < 0 )
        return rv;
    return set_insert_array( *result, set_1->capacity, set_1->items );
}

int set_unionize( Set set, ConstSet snd )
{
    return set_insert_array( set, snd->capacity, snd->items );
}


int set_intersection( ConstSet set_1, ConstSet set_2, Set *result )
{
    if ( *result == NULL )
        *result = set_init_cap( set_1->capacity + set_2->capacity );
    if ( *result == NULL )
        return RV_ERROR;

    for ( size_t i = 0; i < set_2->capacity; ++i )
    {
        struct set_item *item = set_2->items + i;
        if ( item->data == NULL )
            continue;

        if ( !set_search_item( set_1, item ) )
            continue;

        if ( set_insert_item( *result, item ) < 0 )
            return RV_ERROR;
    }

    return RV_SUCCESS;
}

int set_intersect( Set set, ConstSet snd )
{
    for ( size_t i = 0; i < snd->capacity; ++i )
        if ( set->items->data != NULL )
            if ( !set_search_item( snd, set->items + i ) )
                if ( set_remove_item( set, set->items + i ) == RV_ERROR )
                    return RV_ERROR;
    return RV_SUCCESS;
}


int set_difference( ConstSet set, ConstSet snd, Set *result )
{
    if ( *result == NULL )
        *result = set_init_cap( set->capacity + snd->capacity );
    if ( *result == NULL )
        return RV_ERROR;

    for ( size_t i = 0; i < set->capacity; ++i )
    {
        struct set_item *item = set->items + i;
        if ( item->data == NULL )
            continue;

        if ( set_search_item( snd, item ) )
            continue;

        if ( set_insert_item( *result, item ) == RV_ERROR )
            return RV_ERROR;
    }

    return RV_SUCCESS;
}

int set_subtract( Set set, ConstSet snd )
{
    return set_remove_array( set, snd->capacity, snd->items );
}


int set_cmp( ConstSet set_1, ConstSet set_2 )
{
    int cmp = cmp_size_t( set_1->items, set_2->items );
    if ( cmp != 0 )
        return cmp;

    for ( size_t i = 0; i < set_1->capacity; ++i )
    {
        struct set_item *item_1 = set_1->items + i;
        struct set_item *item_2 = set_2->items + i;
        if ( !set_search_item( set_2, item_1 ) )
            return 1;
        if ( !set_search_item( set_1, item_2 ) )
            return -1;
    }

    return 0;
}


void set_destroy( Set set )
{
    for ( size_t i = 0; i < set->capacity; ++i )
        free( set->items[ i ].data );
    free( set->items );
    free( set );
}

void set_destroy_n( int n, ... )
{
    va_list vaList;
    va_start( vaList, n );
    while ( n-- > 0 )
        set_destroy( va_arg( vaList, Set ) );

    va_end( vaList );
}


void set_print_as( ConstSet set, PrintFunction func )
{
    printf( "hash_set (size=%zu): {", set->n_items );

    const char *delim;
    size_t n = 0;
    for ( size_t i = 0; i < set->capacity; ++i )
    {
        struct set_item *item = set->items + i;
        if ( item->data == NULL )
            continue;

        delim = item->size > 16 || n % SET_ITEMCOUNT_LINE_LENGTH == 0 ? ",\n\t" : ", ";
        if ( n == 0 )
        {
            printf( set->n_items > SET_ITEMCOUNT_LINE_LENGTH ? "\n\t" : " " );
        }
        else
        {
            printf( "%s", delim );
        }

        // print
        ( func != NULL ? func : item->func )( item->data, item->size );

        ++n;
    }

    if ( set->n_items > SET_ITEMCOUNT_LINE_LENGTH )
        printf( "\n" );
    else if ( cmpeq( strcmp( delim, ", " ) ) )
        printf( " " );
    printf( "}\n" );
}

void set_print( ConstSet set )
{
    set_print_as( set, NULL );
}
