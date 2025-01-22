//
// Created by MacBook on 18.10.2024.
//
#include "dynstring.h"

#include "../Dev/errors.h"   /* RV, warn */
#include "../misc.h"         /* min_64() */
#include "../string_utils.h" /* vasprintf(in case it's not defined in <stdio.h>),
                              * str_t, string_t */

#include <assert.h>
#include <stdio.h>  /* fprintf() */
#include <stdlib.h> /* alloc */
#include <string.h> /* this one should be obvious */


struct dynamic_string {
    char *data;
    size_t len;

    size_t cap;
};


DynamicString dynstr_init_cap( size_t capacity )
{
    DynamicString new = calloc( 1, sizeof( struct dynamic_string ) );
    if ( new == NULL )
        return ( void * ) fwarn_ret( NULL, "calloc" );

    new->cap  = capacity;
    new->len  = 0;
    new->data = calloc( new->cap, 1 );
    if ( new->data == NULL )
    {
        free( new );
        return ( void * ) fwarn_ret( NULL, "calloc" );
    }

    return new;
}

DynamicString dynstr_init( void )
{
    return dynstr_init_cap( DEFAULT_DYNSTRING_CAP );
}

DynamicString dynstr_init_as( string_t s )
{
    if ( s == NULL )
        return ( void * ) fwarnx_ret( NULL, "initial string may not be NULL" );

    size_t len        = strlen( s );
    DynamicString new = dynstr_init_cap( len + 1 );
    if ( new == NULL )
    {
        f_stack_trace();
        return NULL;
    }

    strncpy( new->data, s, len + 1 );
    new->len = len;

    return new;
}

void dynstr_destroy( DynamicString dynstr )
{
    free( dynstr->data );
    free( dynstr );
}


Private int dynstr_resize( DynamicString dynstr, size_t new_size )
{
    assert( new_size > dynstr->len );

    char *temp = realloc( dynstr->data, new_size );
    if ( temp == NULL )
        return fwarn_ret( RV_ERROR, "realloc" );

    dynstr->data = temp;
    dynstr->cap  = new_size;
    dynstr->len  = min_u64( dynstr->cap, dynstr->len );

    dynstr->data[ dynstr->len ] = '\0';

    return RV_SUCCESS;
}


Private int dynstr_VPendF( DynamicString dynstr,
                           int( pender )( DynamicString, const char * ),
                           const char *fmt,
                           va_list vaList )
{
    str_t buffer;
    if ( vasprintf( &buffer, fmt, vaList ) == RV_ERROR )
    {
        f_stack_trace();
        return RV_ERROR;
    }

    if ( pender( dynstr, buffer ) != RV_SUCCESS )
    {
        f_stack_trace();
        return RV_ERROR;
    }

    free( buffer );
    return RV_SUCCESS;
}


int dynstr_append( DynamicString dynstr, const char *app )
{
    return dynstr_appendn( dynstr, app, strlen( app ) );
}

int dynstr_appendn( DynamicString dynstr, const char *app, size_t len )
{
    size_t new_size = dynstr->len + len;
    if ( new_size + 1 >= dynstr->cap )
    {
        size_t new_cap = dynstr->cap;
        /* get next smallest power of 2 */
        while ( new_size + 1 >= new_cap )
            new_cap *= 2;

        return_on_fail( dynstr_resize( dynstr, new_cap ) );
    }

    strncpy( dynstr->data + dynstr->len, app, len );
    dynstr->data[ new_size ] = '\0';
    dynstr->len              = new_size;

    return RV_SUCCESS;
}

int dynstr_appendf( DynamicString dynstr, const char *fmt, ... )
{
    va_list va;
    va_start( va, fmt );
    int rv = dynstr_VPendF( dynstr, dynstr_append, fmt, va );
    va_end( va );
    return rv;
}

int dynstr_vappendf( DynamicString dynstr, const char *fmt, va_list vargs )
{
    return dynstr_VPendF( dynstr, dynstr_append, fmt, vargs );
}


int dynstr_prepend( DynamicString dynstr, string_t s )
{
    return dynstr_prependn( dynstr, s, strlen( s ) );
}

int dynstr_prependn( DynamicString dynstr, const char *s, size_t len )
{
    size_t new_size = dynstr->len + len;
    if ( new_size >= dynstr->cap )
    {
        size_t new_cap = dynstr->cap;
        while ( new_cap <= new_size )
            new_cap *= 2;

        int rv = dynstr_resize( dynstr, new_cap );
        if ( rv != RV_SUCCESS )
        {
            f_stack_trace();
            return rv;
        }
    }

    memmove( dynstr->data + len, dynstr->data, dynstr->len );
    memcpy( dynstr->data, s, len );
    dynstr->data[ new_size ] = '\0';
    dynstr->len              = new_size;

    return RV_SUCCESS;
}

int dynstr_prependf( DynamicString dynstr, const char *fmt, ... )
{
    va_list va;
    va_start( va, fmt );
    int rv = dynstr_VPendF( dynstr, dynstr_prepend, fmt, va );
    va_end( va );
    return rv;
}

int dynstr_vprependf( DynamicString dynstr, const char *fmt, va_list vargs )
{
    return dynstr_VPendF( dynstr, dynstr_prepend, fmt, vargs );
}


int dynstr_slice( DynamicString dynstr, size_t start_idx, ssize_t end_idx )
{
    const size_t end = end_idx < 0 ? dynstr->len + end_idx + 1 : ( size_t ) end_idx;

    if ( start_idx > dynstr->len || end > dynstr->len || start_idx > end )
        return fwarnx_ret( RV_EXCEPTION,
                           "invalid indices (start=%zu, end=%zi)",
                           start_idx,
                           end_idx );

    const size_t len = end - start_idx;
    memmove( dynstr->data, dynstr->data + start_idx, len );
    dynstr->data[ len ] = '\0';
    dynstr->len         = len;
    return RV_SUCCESS;
}

int dynstr_slice_e( DynamicString dynstr, ssize_t end_idx )
{
    const size_t end = end_idx < 0 ? dynstr->len + end_idx + 1 : ( size_t ) end_idx;
    if ( end > dynstr->len )
        return fwarnx_ret( RV_EXCEPTION,
                           "index %zu out of bounds for string of length %zu",
                           end,
                           dynstr->len );

    dynstr->data[ end ] = '\0';
    dynstr->len         = end;
    return RV_SUCCESS;
}

int dynstr_slice_s( DynamicString dynstr, size_t start_idx )
{
    if ( start_idx > dynstr->len )
        return fwarnx_ret( RV_EXCEPTION,
                           "index %zu out of bounds for string of length %zu",
                           start_idx,
                           dynstr->len );

    const size_t len = dynstr->len - start_idx;
    memmove( dynstr->data, dynstr->data + start_idx, len );
    dynstr->data[ len ] = '\0';
    return RV_SUCCESS;
}


int dynstr_reset( DynamicString dynstr )
{
    free( dynstr->data );
    if ( ( dynstr->data = malloc( DEFAULT_DYNSTRING_CAP ) ) == NULL )
        return fwarn_ret( RV_ERROR, "malloc" );
    dynstr->data[ 0 ] = '\0';
    dynstr->len       = 0;
    dynstr->cap       = DEFAULT_DYNSTRING_CAP;
    return RV_SUCCESS;
}

/* ==== */ /* ==== */

str_t dynstr_data_copy( ConstDynamicString dynstr )
{
    str_t ret = strndup( dynstr->data, dynstr->len );
    if ( ret == NULL )
        fwarn( "strndup" );
    return ret;
}


str_t dynstr_data( DynamicString dynstr )
{
    return dynstr->data;
}

size_t dynstr_len( ConstDynamicString dynstr )
{
    return dynstr->len;
}
