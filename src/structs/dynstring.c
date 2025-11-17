//
// Created by MacBook on 18.10.2024.
//
#include "dynstring.h"

#include "../headers/errors.h"        /* RV, warn */
#include "../headers/simple_math.h"   /* min */
#include "../headers/util/ctrlflow.h" /* assert_that */
#include "../util/string_utils.h"     /* vasprintf (if it's not defined in <stdio.h>) */

#include <assert.h> //
#include <stdio.h>  /* fprintf() */
#include <stdlib.h> /* alloc */
#include <string.h> /* this one should be obvious */


#define DEFAULT_DYNSTRING_CAP 256


struct dynamic_string {
    char *data;
    size_t len;

    size_t cap;
};


struct dynamic_string *dynstr_init_cap( const size_t cap )
{
    struct dynamic_string *new = calloc( 1, sizeof( struct dynamic_string ) );
    if ( new == NULL )
        return fwarn_ret( NULL, "calloc" );

    new->cap  = cap;
    new->len  = 0;
    new->data = calloc( new->cap, 1 );
    if ( new->data == NULL )
    {
        free( new );
        return fwarn_ret( NULL, "calloc" );
    }

    return new;
}

struct dynamic_string *dynstr_init( void )
{
    return dynstr_init_cap( DEFAULT_DYNSTRING_CAP );
}

struct dynamic_string *dynstr_init_as( const string_t s )
{
    if ( s == NULL )
        return fwarnx_ret( NULL, "initial string may not be NULL" );

    const size_t len           = strlen( s );
    struct dynamic_string *new = dynstr_init_cap( len + 1 );
    if ( new == NULL )
        return f_stack_trace( NULL );

    strncpy( new->data, s, len + 1 );
    new->len = len;

    return new;
}

void dynstr_destroy( struct dynamic_string *dynstr )
{
    free( dynstr->data );
    free( dynstr );
}


Private int dynstr_resize( struct dynamic_string *dynstr, const size_t new_size )
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


Private ssize_t dynstr_VPendF( struct dynamic_string *dynstr,
                               ssize_t( pender )( struct dynamic_string *, const char * ),
                               const char *fmt,
                               va_list vaList )
{
    str_t buffer;
    vasprintf( &buffer, fmt, vaList );
    const ssize_t rv = pender( dynstr, buffer ) != RV_SUCCESS;
    if ( rv == RV_ERROR )
        return f_stack_trace( RV_ERROR );

    free( buffer );
    return rv;
}


ssize_t dynstr_append( struct dynamic_string *dynstr, const char *app )
{
    return dynstr_appendn( dynstr, app, strlen( app ) );
}

ssize_t dynstr_appendn( struct dynamic_string *dynstr, const char *app, size_t len )
{
    const size_t new_size = dynstr->len + len;
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

    return ( ssize_t ) len;
}

ssize_t dynstr_appendf( struct dynamic_string *dynstr, const char *fmt, ... )
{
    va_list va;
    va_start( va, fmt );
    const ssize_t rv = dynstr_VPendF( dynstr, dynstr_append, fmt, va );
    va_end( va );
    return rv;
}

ssize_t dynstr_vappendf( struct dynamic_string *dynstr, const char *fmt, va_list vargs )
{
    return dynstr_VPendF( dynstr, dynstr_append, fmt, vargs );
}


ssize_t dynstr_prepend( struct dynamic_string *dynstr, const string_t s )
{
    return dynstr_prependn( dynstr, s, strlen( s ) );
}

ssize_t dynstr_prependn( struct dynamic_string *dynstr, const char *s, size_t len )
{
    const size_t new_size = dynstr->len + len;
    if ( new_size >= dynstr->cap )
    {
        size_t new_cap = dynstr->cap;
        while ( new_cap <= new_size )
            new_cap *= 2;

        return_on_fail( dynstr_resize( dynstr, new_cap ) );
    }

    memmove( dynstr->data + len, dynstr->data, dynstr->len );
    memcpy( dynstr->data, s, len );
    dynstr->data[ new_size ] = '\0';
    dynstr->len              = new_size;

    return ( ssize_t ) len;
}

ssize_t dynstr_prependf( struct dynamic_string *dynstr, const char *fmt, ... )
{
    va_list va;
    va_start( va, fmt );
    const ssize_t rv = dynstr_VPendF( dynstr, dynstr_prepend, fmt, va );
    va_end( va );
    return rv;
}

ssize_t dynstr_vprependf( struct dynamic_string *dynstr, const char *fmt, va_list vargs )
{
    return dynstr_VPendF( dynstr, dynstr_prepend, fmt, vargs );
}


int dynstr_slice( struct dynamic_string *dynstr,
                  const size_t start_idx,
                  const ssize_t end_idx )
{
    const size_t end = end_idx < 0 ? dynstr->len + end_idx + 1 : ( size_t ) end_idx;

    if ( start_idx > dynstr->len || end > dynstr->len || start_idx > end )
        return fwarnx_ret(
                RV_EXCEPTION,
                "invalid indices (start=%zu, end=%zi) for string of length %zu",
                start_idx, end_idx, dynstr->len );

    const size_t len = end - start_idx;
    memmove( dynstr->data, dynstr->data + start_idx, len );
    dynstr->data[ len ] = '\0';
    dynstr->len         = len;

    if ( len < dynstr->cap / 2 )
    {
        char *re = realloc( dynstr->data, len + 1 );
        if ( re == NULL )
            return fwarn_ret(
                    RV_ERROR,
                    "realloc failed; not freeing the original chunk of memory" );

        dynstr->data = re;
        dynstr->cap  = len + 1;
    }
    assert_that( dynstr->data[ len ] == '\0', "dynstr must be null terminated" );

    return RV_SUCCESS;
}

int dynstr_slice_e( struct dynamic_string *dynstr, const ssize_t end_idx )
{
    const size_t end = end_idx < 0 ? dynstr->len + end_idx + 1 : ( size_t ) end_idx;
    if ( end > dynstr->len )
        return fwarnx_ret( RV_EXCEPTION,
                           "index %zd out of bounds for string of length %zu",
                           end_idx,
                           dynstr->len );

    return dynstr_slice( dynstr, 0, end_idx );
}

int dynstr_slice_s( struct dynamic_string *const dynstr, const size_t start_idx )
{
    if ( start_idx > dynstr->len )
        return fwarnx_ret( RV_EXCEPTION,
                           "index %zu out of bounds for string of length %zu",
                           start_idx,
                           dynstr->len );

    return dynstr_slice( dynstr, start_idx, -1 );
}


int dynstr_reset( struct dynamic_string *dynstr )
{
    free( dynstr->data );
    if ( ( dynstr->data = malloc( DEFAULT_DYNSTRING_CAP ) ) == NULL )
        return fwarn_ret( RV_ERROR, "malloc" );
    dynstr->data[ 0 ] = '\0';
    dynstr->len       = 0;
    dynstr->cap       = DEFAULT_DYNSTRING_CAP;
    return RV_SUCCESS;
}

int dynstr_set( struct dynamic_string *dynstr, const string_t string )
{
    const size_t len = strlen( string );
    if ( ( dynstr->data = strdup( string ) ) == NULL )
        return fwarn_ret( RV_ERROR, "strdup" );
    dynstr->len = len;
    dynstr->cap = len + 1;
    return RV_SUCCESS;
}


int dynstr_set_at( struct dynamic_string *dynstr, const size_t idx, const char c )
{
    if ( idx >= dynstr->len )
        return fwarnx_ret( RV_EXCEPTION,
                           "index %zu out of bounds for string of length %zu",
                           idx,
                           dynstr->len );

    dynstr->data[ idx ] = c;
    return RV_SUCCESS;
}

/* ==== */ /* ==== */

str_t dynstr_data_copy( const struct dynamic_string *dynstr )
{
    char *const ret = strndup( dynstr->data, dynstr->len );
    if ( ret == NULL )
        fwarn( "strndup" );
    return ret;
}


string_t dynstr_data( const struct dynamic_string *dynstr )
{
    return dynstr->data;
}

size_t dynstr_len( const struct dynamic_string *dynstr )
{
    return dynstr->len;
}


int DynString__map( DynString *dynstr,
                    char *( *string_map )( const char *string, size_t string_length ),
                    const char *mapper_name )
{
    const str_t mapped = string_map( dynstr->data, dynstr->len );
    if ( mapped == NULL )
        // use different function name
        return WarnUniversal( true, NULL, "string_map", -1, -1, RV_ERROR,
                              "failed to map string in function `%s`", mapper_name );

    free_n( dynstr->data );

    dynstr->data = mapped;
    dynstr->len  = strlen( mapped );
    dynstr->cap  = dynstr->len;

    return RV_SUCCESS;
}
