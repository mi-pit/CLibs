//
// Created by MacBook on 03.02.2025.
//

#include "numbers_list.h"

#include "../../Dev/errors.h"
#include "../../Dev/pointer_utils.h"
#include "../../misc.h"

#define DEFAULT_NUMLS_CAP 8

struct numbers_list *numls_init_cap( size_t cap )
{
    struct numbers_list *new_ls = new ( struct numbers_list );
    if ( new_ls == NULL )
        return ( void * ) fwarn_ret( NULL, "calloc" );

    new_ls->cap = cap;
    if ( ( new_ls->numbers = calloc( new_ls->cap, sizeof( uint64_t ) ) ) == NULL )
    {
        free( new_ls );
        return ( void * ) fwarn_ret( NULL, "calloc" );
    }

    return new_ls;
}

struct numbers_list *numls_init( void )
{
    struct numbers_list *ls = numls_init_cap( DEFAULT_NUMLS_CAP );
    if ( ls == NULL )
        f_stack_trace( 0 );

    return ls;
}

void numls_destroy( struct numbers_list *ls )
{
    free_n( ls->numbers );
    free( ls );
}

Private int numls_resize( struct numbers_list *ls, size_t new_cap )
{
    ls->cap       = new_cap;
    uint64_t *tmp = realloc( ls->numbers, sizeof( uint64_t ) * ls->cap );
    if ( tmp == NULL )
        return fwarn_ret( RV_ERROR, "realloc" );

    ls->numbers = tmp;
    return RV_SUCCESS;
}

int numls_append( struct numbers_list *ls, uint64_t app )
{
    if ( ls->cap <= ls->len + 1 )
        return_on_fail( numls_resize( ls, get_next_power_of_two( ls->cap ) ) );

    ls->numbers[ ls->len++ ] = app;
    return RV_SUCCESS;
}

int numls_pop( struct numbers_list *ls )
{
    ls->numbers[ --ls->len ] = 0;

    if ( ls->len < ls->cap / 2 )
        return_on_fail( numls_resize( ls, get_prev_power_of_two( ls->cap ) ) );
    return RV_SUCCESS;
}

int numls_extend_zeroes( struct numbers_list *ls, size_t new_len )
{
    if ( ls->cap <= ls->len + new_len )
        return_on_fail( numls_resize( ls, get_next_power_of_two( ls->len + new_len ) ) );

    memset( ls->numbers + ls->len, 0, new_len * sizeof( uint64_t ) );
    ls->len += new_len;

    return RV_SUCCESS;
}
