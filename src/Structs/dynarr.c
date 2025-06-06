
#include "dynarr.h"

#include "../Dev/errors.h" /* RVs, fwarn* */
#include "../Dev/extra_types.h"
#include "../Dev/pointer_utils.h"
#include "../misc.h" /* cmpeq() */

#include <stdio.h>  /* *print* */
#include <stdlib.h> /* malloc, free, bsearch, qsort */
#include <string.h> /* mem* */


#define ListIndexOOBExceptionString( LIST, INDEX ) \
    "index %zu out of bounds for list of size %zu", INDEX, ( LIST )->size

#define ListEmptyExceptionString "list is empty"

#define ListNullPointerExceptionString( ARG ) #ARG " may not be null"


/**
 * A dynamic array. (List)<br>
 * The List can store any elements of arbitrary size.
 * <p>
 * A new List can (and should) be created with list_init_* functions
 * <p>
 * list_init_type* functions take as its `type' argument the C keyword
 * e.g. 'char'
 * <br>
 * list_init_size* functions take as its `el_size' argument
 * the number of bytes (acquired by sizeof)
 * for the desired type to be stored
 * <p>
 * The list_init_*_p functions take a second parameter
 * – a constant for the print functions. This field can be changed
 * later with list_set_print_mode().
 * <p>
 * list_init_* functions allocate and initialize a List
 * with these default values:
 * @code
 * List::capacity = LIST_DEF_SIZE
 * List::size     = 0
 * List::items    = pointer to heap allocated memory of width ‹capacity›, set to 0
 * List::el_size  = width of a single element
 * @endcode
 *
 * @attention
 * All elements must be the same number of bytes long
*/
struct dynamic_array {
    size_t capacity; // Amount of allocated memory slots for items
    size_t size;     // Number of items stored in List
    void *items;     // Array of any type
    size_t el_size;  // sizeof a single item

    enum ListPrinters def_print_mode; // for print functions
};


Private inline void *list_at_non_safe( const struct dynamic_array *ls, size_t idx )
{
    return ( byte * ) ls->items + idx * ls->el_size;
}


/* –––––––––––––––––––––––––––––– FUNCTIONAL –––––––––––––––––––––––––––––– */

struct dynamic_array *list_init_size( size_t el_size )
{
    struct dynamic_array *ls = calloc( 1, sizeof( struct dynamic_array ) );
    if ( ls == NULL )
        return ( void * ) fwarn_ret( NULL, "calloc" );

    ls->items = calloc( LIST_DEF_SIZE, el_size );
    if ( ls->items == NULL )
    {
        free( ls );
        return ( void * ) fwarn_ret( NULL, "calloc" );
    }
    ls->capacity = LIST_DEF_SIZE;
    ls->el_size  = el_size;
    return ls;
}

struct dynamic_array *list_init_size_p( size_t el_size, int mode )
{
    struct dynamic_array *new_ls = list_init_size( el_size );
    if ( new_ls == NULL )
        return ( void * ) f_stack_trace( NULL );

    new_ls->def_print_mode = mode;
    return new_ls;
}


/* Immutable */
const void *list_see( const struct dynamic_array *ls, size_t idx )
{
    if ( idx >= ls->size )
        return ( void * ) fwarnx_ret( NULL, ListIndexOOBExceptionString( ls, idx ) );

    return list_at_non_safe( ls, idx );
}

const void *list_peek( const struct dynamic_array *ls )
{
    if ( ls->size == 0 )
        return ( void * ) fwarnx_ret( NULL, ListEmptyExceptionString );

    return list_at_non_safe( ls, list_size( ls ) - 1 );
}


/* Mutable */
void *list_at( struct dynamic_array *ls, size_t idx )
{
    if ( idx >= ls->size )
        return ( void * ) fwarnx_ret( NULL, ListIndexOOBExceptionString( ls, idx ) );

    return list_at_non_safe( ls, idx );
}

void *list_at_last( struct dynamic_array *ls )
{
    if ( ls->size == 0 )
        return ( void * ) fwarnx_ret( NULL, ListEmptyExceptionString );

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
Private int list_upsize( struct dynamic_array *ls, size_t min_cap_add )
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


int list_set_at( struct dynamic_array *ls, size_t index, const void *data )
{
    if ( index >= ls->size )
        return fwarnx_ret( RV_EXCEPTION, ListIndexOOBExceptionString( ls, index ) );

    if ( data == NULL )
        return fwarnx_ret( RV_EXCEPTION, ListNullPointerExceptionString( data ) );

    memcpy( ( ( byte * ) ls->items ) + ( index * ls->el_size ), data, ls->el_size );

    return RV_SUCCESS;
}


int list_append( struct dynamic_array *ls, const void *data )
{
    if ( ls->size + 1 >= ls->capacity )
        if ( list_upsize( ls, 1 ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    ++ls->size;
    memcpy( list_at_last( ls ), data, ls->el_size );

    return RV_SUCCESS;
}

int list_extend( struct dynamic_array *ls, const void *array, size_t len )
{
    if ( ls->capacity < ls->size + len )
        if ( list_upsize( ls, len ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    memcpy( list_at_non_safe( ls, ls->size ), array, len * ls->el_size );

    ls->size += len;

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

int list_insert( struct dynamic_array *ls, size_t at, const void *data )
{
    if ( at > ls->size )
        return fwarnx_ret( RV_EXCEPTION, ListIndexOOBExceptionString( ls, at ) );

    if ( ls->size + 1 >= ls->capacity )
        if ( list_upsize( ls, 1 ) != RV_SUCCESS )
            return f_stack_trace( RV_ERROR );

    memmove( list_at_non_safe( ls, at + 1 ),
             list_at_non_safe( ls, at ),
             ( ls->size - at ) * ls->el_size );

    memcpy( list_at_non_safe( ls, at ), data, ls->el_size );

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

int list_remove( struct dynamic_array *ls, size_t index, void *container )
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

int list_remove_fast( struct dynamic_array *ls, size_t index, void *container )
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
    // todo: re-implement this for efficiency
    struct dynamic_array *rev;
    if ( list_copy( ls, &rev ) != RV_SUCCESS )
        return ( void * ) f_stack_trace( NULL );

    if ( list_reverse( rev ) != RV_SUCCESS )
    {
        list_destroy( rev );
        return ( void * ) f_stack_trace( NULL );
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

struct dynamic_array *list_copy_p( const struct dynamic_array *ls )
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

void list_destroy_p( struct dynamic_array ls )
{
    free_n( ls.items );
}


/* ––––––––––––––––––––––––––––––– PRINTERS ––––––––––––––––––––––––––––––– */

Private void list_print_static( const struct dynamic_array *ls,
                                int print_mode,
                                bool print_size );

Private void list_print_mode( const struct dynamic_array *ls, // NOLINT(misc-no-recursion)
                              int print_mode,
                              bool print_size )
{
    switch ( print_mode )
    {
        default:
            fwarnx( "invalid print format" );
            __attribute__( ( fallthrough ) );
        case LS_PRINT_BYTE:
            list_printf_d( ls, byte, "%02x", " " );
            break;
        case LS_PRINT_SIGNED:
            list_printf( ls, long long int, "%lli" );
            break;
        case LS_PRINT_UNSIGNED:
            list_printf( ls, unsigned long long int, "%llu" );
            break;
        case LS_PRINT_DEC:
            list_printf( ls, double, "%.2f" );
            break;
        case LS_PRINT_PTR:
            list_printf( ls, void *, "@%p" );
            break;
        case LS_PRINT_STR:
            printf( "\"%s\"\n", ( char * ) ls->items );
            break;
        case LS_PRINT_NOFORMAT:
            list_print_mode( ls, ls->def_print_mode, print_size );
            break;
        case LS_PRINT_LIST:
            printf( "[\n" );
            for ( size_t i = 0; i < ls->size; ++i )
            {
                printf( "\t" );
                list_print_static( list_fetch( ls, i, struct dynamic_array * ),
                                   LS_PRINT_NOFORMAT,
                                   print_size );
            }
            printf( "]\n" );
            break;
    }
}

Private void list_print_static( // NOLINT(misc-no-recursion)
        const struct dynamic_array *ls,
        int print_mode,
        bool print_size )
{
    if ( ls == NULL )
    {
        printf( "NULL\n" );
        return;
    }
    if ( ls->items == NULL )
    {
        printf( "List (%p) has no items\n", ( void * ) ls );
        return;
    }

    if ( print_size )
        printf( "%zu:", ls->size );

    list_print_mode( ls, print_mode, print_size );
}

void list_prints( const struct dynamic_array *ls, int print_mode )
{
    list_print_static( ls, print_mode, true );
}

void list_printm( const struct dynamic_array *ls, int print_mode )
{
    list_print_static( ls, print_mode, false );
}

void list_print( const struct dynamic_array *ls )
{
    list_print_static( ls, LS_PRINT_NOFORMAT, false );
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
        return ( void * ) fwarn_ret( NULL, "calloc" );

    memcpy( copy, ls->items, ls->size * ls->el_size );
    return copy;
}


void list_set_print_mode( struct dynamic_array *ls, int fmt )
{
    ls->def_print_mode = fmt;
}
