#include "dynarr.h"

#include "../headers/util/cmp.h"                /* cmp */
#include "../headers/util/ctrlflow.h"           /* assert_that */
#include "../headers/util/ptrs/alloc.h"         /* deref_as */
#include "queue.h"
#include "set.h"

#include <inttypes.h> /* PRIi64 */
#include <stdlib.h>   /* alloc, bsearch, qsort */
#include <string.h>   /* mem* */


#define LIST_DEF_CAP 64

#define LIST_CAP_SIZE_RATIO 2


#define ListIndexOOBExceptionString( LIST, INDEX ) \
    "index %zu out of bounds for list of size %zu", INDEX, ( LIST )->size

#define ListEmptyExceptionString "list is empty"

#define ListNullPointerExceptionString( ARG ) #ARG " may not be null"


struct dynamic_array {
    size_t capacity; // Amount of allocated memory slots for items
    size_t size;     // Number of items stored in List
    void *items;     // Array of any type
    size_t el_size;  // sizeof a single item
};


Private inline void *list_at_non_safe( const struct dynamic_array *ls, const size_t idx )
{
    return ( byte * ) ls->items + idx * ls->el_size;
}


/* –––––––––––––––––––––––––––––– FUNCTIONAL –––––––––––––––––––––––––––––– */

List *list_init_cap_size( const size_t el_size, const size_t init_cap )
{
    struct dynamic_array *ls = calloc( 1, sizeof( struct dynamic_array ) );
    if ( ls == NULL )
        return fwarn_ret( NULL, "calloc" );

    ls->items = calloc( init_cap, el_size );
    if ( ls->items == NULL )
    {
        free( ls );
        return fwarn_ret( NULL, "calloc" );
    }
    ls->capacity = LIST_DEF_CAP;
    ls->el_size  = el_size;
    return ls;
}

List *list_init_size( const size_t el_size )
{
    List *new = list_init_cap_size( el_size, LIST_DEF_CAP );
    if ( new == NULL )
        return f_stack_trace( NULL );

    return new;
}


/* Immutable */
const void *list_see( const struct dynamic_array *ls, const size_t idx )
{
    if ( idx >= ls->size )
        return fwarnx_ret( NULL, ListIndexOOBExceptionString( ls, idx ) );

    return list_at_non_safe( ls, idx );
}

const void *list_peek( const struct dynamic_array *ls )
{
    if ( ls->size == 0 )
        return fwarnx_ret( NULL, ListEmptyExceptionString );

    return list_at_non_safe( ls, list_size( ls ) - 1 );
}


/* Mutable */
void *list_at( List *ls, const size_t idx )
{
    if ( idx >= ls->size )
        return fwarnx_ret( NULL, ListIndexOOBExceptionString( ls, idx ) );

    return list_at_non_safe( ls, idx );
}

void *list_at_last( List *const ls )
{
    if ( ls->size == 0 )
        return fwarnx_ret( NULL, ListEmptyExceptionString );

    return list_at_non_safe( ls, list_size( ls ) - 1 );
}


/**
 * Changes (realloc) the lists capacity to be the smallest possible power of two
 * capable of holding its items + `min_cap_add`
 * \n
 * Therefore, the List always has enough space
 * to hold all of its items + min_cap_add
 * @return -1 if realloc fails, else 0
 */
Private int list_upsize( struct dynamic_array *ls, const size_t min_cap_add )
{
    const size_t min_new_cap = ls->capacity + min_cap_add;

    size_t new_capacity = ls->capacity;
    while ( new_capacity < min_new_cap )
        new_capacity *= LIST_CAP_SIZE_RATIO;

    void *tmp = realloc( ls->items, new_capacity * ls->el_size );
    if ( tmp == NULL )
        return fwarn_ret( RV_ERROR, "realloc" );

    ls->items    = tmp;
    ls->capacity = new_capacity;
    return RV_SUCCESS;
}

/**
 * Changes (realloc) the lists capacity to be half as large\n
 * @return -1 if realloc fails, else 0
 */
Private int list_downsize( struct dynamic_array *ls )
{
    const size_t new_cap = ls->capacity / LIST_CAP_SIZE_RATIO;

    void *tmp = realloc( ls->items, new_cap * ls->el_size );
    if ( tmp == NULL )
        return fwarn_ret( RV_ERROR, "realloc" );

    ls->items    = tmp;
    ls->capacity = new_cap;
    return RV_SUCCESS;
}


int list_set_at( struct dynamic_array *ls, const size_t index, const void *data )
{
    if ( index >= ls->size )
        return fwarnx_ret( RV_EXCEPTION, ListIndexOOBExceptionString( ls, index ) );

    if ( data == NULL )
        return fwarnx_ret( RV_EXCEPTION, ListNullPointerExceptionString( data ) );

    memcpy( ( ( byte * ) ls->items ) + ( index * ls->el_size ), data, ls->el_size );

    return RV_SUCCESS;
}


int list_append( struct dynamic_array *ls, const void *datap )
{
    if ( ls->size + 1 >= ls->capacity )
        if ( list_upsize( ls, 1 ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    ++ls->size;
    memcpy( list_at_last( ls ), datap, ls->el_size );

    return RV_SUCCESS;
}

int list_extend( struct dynamic_array *ls, const void *array, const size_t array_len )
{
    if ( ls->capacity < ls->size + array_len )
        if ( list_upsize( ls, array_len ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    memcpy( list_at_non_safe( ls, ls->size ), array, array_len * ls->el_size );

    ls->size += array_len;

    return RV_SUCCESS;
}

int list_extend_list( struct dynamic_array *ls, const struct dynamic_array *app )
{
    if ( ls->size + app->size >= ls->capacity )
        if ( list_upsize( ls, app->size ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    memcpy( list_at_non_safe( ls, ls->size ), app->items, app->size * app->el_size );

    ls->size += app->size;

    return RV_SUCCESS;
}

int list_insert( struct dynamic_array *ls, const size_t index, const void *data )
{
    if ( index > ls->size )
        return fwarnx_ret( RV_EXCEPTION, ListIndexOOBExceptionString( ls, index ) );

    if ( ls->size + 1 >= ls->capacity )
        if ( list_upsize( ls, 1 ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    memmove( list_at_non_safe( ls, index + 1 ),
             list_at_non_safe( ls, index ),
             ( ls->size - index ) * ls->el_size );

    memcpy( list_at_non_safe( ls, index ), data, ls->el_size );

    ++ls->size;

    return RV_SUCCESS;
}


Private inline void list_remove_last_nonsafe( struct dynamic_array *ls )
{
    memset( list_at_last( ls ), 0, ls->el_size );
    --ls->size;
}

int list_pop( struct dynamic_array *ls, void *container )
{
    if ( ls->size == 0 )
        return fwarnx_ret( RV_EXCEPTION, ListEmptyExceptionString );

    if ( ls->capacity / LIST_CAP_SIZE_RATIO > ls->size )
        if ( list_downsize( ls ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    if ( container != NULL )
        // copy popped element to container
        memcpy( container, list_at_last( ls ), ls->el_size );

    list_remove_last_nonsafe( ls );

    return RV_SUCCESS;
}

int list_remove( struct dynamic_array *ls, const size_t index, void *container )
{
    if ( index >= ls->size )
        return fwarnx_ret( RV_EXCEPTION, ListIndexOOBExceptionString( ls, index ) );

    if ( index == ls->size - 1 )
        return list_pop( ls, container );

    if ( ls->capacity / LIST_CAP_SIZE_RATIO > ls->size )
        if ( list_downsize( ls ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    if ( container != NULL )
        memcpy( container, list_at( ls, index ), ls->el_size );

    memmove( list_at( ls, index ),
             list_at( ls, index + 1 ),
             ( ls->size - index ) * ls->el_size );

    --ls->size;

    return RV_SUCCESS;
}

int list_remove_fast( struct dynamic_array *ls, const size_t index, void *container )
{
    if ( index >= ls->size )
        return fwarnx_ret( RV_EXCEPTION, ListIndexOOBExceptionString( ls, index ) );

    if ( ls->capacity / LIST_CAP_SIZE_RATIO > ls->size )
        if ( list_downsize( ls ) )
            return RV_ERROR;

    if ( container != NULL )
        // put element at index into the container
        memmove( container, list_at( ls, index ), ls->el_size );

    // put last element at ‹index›
    memmove( list_at( ls, index ), list_at_last( ls ), ls->el_size );

    // pop last element
    list_remove_last_nonsafe( ls );

    return RV_SUCCESS;
}


const void *list_bsearch_p( const struct dynamic_array *ls,
                            const void *needle,
                            int ( *cmp )( const void *, const void * ) )
{
    return bsearch( needle, ls->items, ls->size, ls->el_size, cmp );
}

int64_t list_bsearch_i( const struct dynamic_array *ls,
                        const void *needle,
                        int ( *cmp )( const void *, const void * ) )
{
    const byte *res = list_bsearch_p( ls, needle, cmp );
    if ( res == NULL )
        return -1;
    return res - ( byte * ) ls->items;
}

const void *list_lsearch_p( const struct dynamic_array *ls, const void *needle )
{
    for ( size_t i = 0; i < ls->size; ++i )
        if ( cmpeq( memcmp( list_see( ls, i ), needle, ls->el_size ) ) )
            return list_see( ls, i );

    return NULL;
}

int64_t list_lsearch_i( const struct dynamic_array *ls, const void *needle )
{
    const byte *res = list_lsearch_p( ls, needle );
    if ( res == NULL )
        return -1;
    return res - ( byte * ) ls->items;
}

void list_sort( struct dynamic_array *ls, int ( *cmp )( const void *, const void * ) )
{
    qsort( ls->items, ls->size, ls->el_size, cmp );
}


int list_reverse( struct dynamic_array *ls )
{
    if ( ls->size == 0 )
        return RV_SUCCESS;

    void *buffer = malloc( ls->el_size );
    if ( buffer == NULL )
        return fwarn_ret( RV_ERROR, "buffer allocation" );

    byte *l = ls->items;
    byte *r = list_at_non_safe( ls, ls->size - 1 );

    for ( size_t i = 0; i < ls->size / 2; ++i )
    {
        // [ _ ], [ l, ..., m, ..., r ] –> [ l ], [ l, ..., m, ..., r ]
        memcpy( buffer, l, ls->el_size );

        // [ l ], [ l, ..., m, ..., r ] –> [ l ], [ r, ..., m, ..., r ]
        memcpy( l, r, ls->el_size );

        // [ l ], [ r, ..., m, ..., r ] –> [ l ], [ r, ..., m, ..., l ]
        memcpy( r, buffer, ls->el_size );

        l += ls->el_size;
        r += ls->el_size;
    }

    free( buffer );

    return RV_SUCCESS;
}

struct dynamic_array *list_reversed( const struct dynamic_array *ls )
{
    struct dynamic_array *rev = list_init_size( ls->el_size );
    if ( rev == NULL )
        return f_stack_trace( NULL );

    for ( size_t i = 0; i < ls->size; ++i )
    {
        const size_t index = ls->size - 1 - i;
        const void *datap  = list_see( ls, index );
        assert_that( datap != NULL,
                     "index %zu must be in bounds (size=%zu)",
                     index,
                     ls->size );

        if ( list_append( rev, datap ) != RV_SUCCESS )
        {
            list_destroy( rev );
            return f_stack_trace( NULL );
        }
    }

    return rev;
}


int list_copy( const struct dynamic_array *old, struct dynamic_array **new_ls_container )
{
    struct dynamic_array *new_ls = list_init_size( old->el_size );
    if ( new_ls == NULL )
        return RV_ERROR;

    if ( list_extend_list( new_ls, old ) != RV_SUCCESS )
    {
        list_destroy( new_ls );
        return f_stack_trace( RV_ERROR );
    }

    *new_ls_container = new_ls;

    return RV_SUCCESS;
}

struct dynamic_array *list_copy_of( const struct dynamic_array *ls )
{
    struct dynamic_array *new;
    if ( list_copy( ls, &new ) )
        return NULL;
    return new;
}


int list_cmp_size( const void *l1, const void *l2 )
{
    const struct dynamic_array *ls_1 = deref_as( const struct dynamic_array *, l1 );
    const struct dynamic_array *ls_2 = deref_as( const struct dynamic_array *, l2 );

    return cmp_size_t( &ls_1->size, &ls_2->size );
}

int list_cmp_elsize( const void *l1, const void *l2 )
{
    const struct dynamic_array *ls_1 = deref_as( const struct dynamic_array *, l1 );
    const struct dynamic_array *ls_2 = deref_as( const struct dynamic_array *, l2 );

    return cmp_size_t( &ls_1->el_size, &ls_2->el_size );
}


void list_destroy( struct dynamic_array *ls )
{
    free_n( ls->items );
    free( ls );
}

int list_clear( struct dynamic_array *ls )
{
    free_n( ls->items );
    ls->size     = 0;
    ls->capacity = LIST_DEF_CAP;
    if ( ( ls->items = calloc( ls->capacity, ls->el_size ) ) == NULL )
        return fwarn_ret( RV_ERROR, "calloc" );

    return RV_SUCCESS;
}


/* ––––– GETTERS/SETTERS ––––– */

bool list_is_empty( const struct dynamic_array *ls )
{
    return ls->size == 0;
}

size_t list_size( const struct dynamic_array *ls )
{
    return ls->size;
}

size_t list_el_size( const struct dynamic_array *ls )
{
    return ls->el_size;
}

const void *list_items( const struct dynamic_array *ls )
{
    return ls->items;
}

void *list_items_copy( const struct dynamic_array *ls )
{
    void *copy = calloc( ls->size, ls->el_size );
    if ( copy == NULL )
        return fwarn_ret( NULL, "calloc" );

    memcpy( copy, ls->items, ls->size * ls->el_size );
    return copy;
}
