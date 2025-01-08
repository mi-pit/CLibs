//
// Created by MacBook on 18.10.2024.
//
#include "dynstring.h"

#include "errors.h"       /* RV, warn */
#include "misc.h"         /* min_m */
#include "string_utils.h" /* types */

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
        return fwarn_ret_p( NULL, "calloc" );

    new->cap  = capacity;
    new->len  = 0;
    new->data = calloc( new->cap, 1 );
    if ( new->data == NULL )
    {
        free( new );
        return fwarn_ret_p( NULL, "calloc" );
    }

    return new;
}

DynamicString dynstr_init( void )
{
    return dynstr_init_cap( DEFAULT_DYNSTRING_CAP );
}

DynamicString dynstr_init_as( string_t s )
{
    size_t len        = strlen( s );
    DynamicString new = dynstr_init_cap( len + 1 );
    if ( new == NULL )
        return NULL;

    strncpy( new->data, s, len + 1 );
    new->len = len;

    return new;
}

void dynstr_destroy( DynamicString dynstr )
{
    free( dynstr->data );
    free( dynstr );
}


static int dynstr_resize( DynamicString dynstr, size_t new_size )
{
    char *temp = realloc( dynstr->data, new_size );
    if ( temp == NULL )
        return fwarn_ret( RV_ERROR, "realloc" );

    dynstr->data = temp;
    dynstr->cap  = new_size;
    dynstr->len  = min_64( dynstr->cap, dynstr->len );

    return RV_SUCCESS;
}


int dynstr_append( DynamicString dynstr, const char *app )
{
    size_t app_len  = strlen( app );
    size_t new_size = dynstr->len + app_len;
    if ( new_size >= dynstr->cap )
    {
        size_t new_cap = dynstr->cap;
        /* get next smallest power of 2 */
        while ( new_cap < new_size )
            new_cap *= 2;

        return_on_fail( dynstr_resize( dynstr, new_size ) );
    }

    strcpy( dynstr->data + dynstr->len, app );
    dynstr->data[ new_size ] = '\0';
    dynstr->len              = new_size;

    return RV_SUCCESS;
}

int dynstr_appendn( DynamicString dynstr, const char *app, size_t len )
{
    size_t new_size = dynstr->len + len;
    if ( new_size >= dynstr->cap )
    {
        size_t new_cap = dynstr->cap;
        /* get next smallest power of 2 */
        while ( new_cap < new_size )
            new_cap *= 2;

        return_on_fail( dynstr_resize( dynstr, new_size ) );
    }

    strncpy( dynstr->data + dynstr->len, app, len );
    dynstr->data[ new_size ] = '\0';
    dynstr->len              = new_size;

    return RV_SUCCESS;
}


int dynstr_prepend( DynamicString dynstr, string_t s )
{
    size_t added_len = strlen( s );
    size_t new_size  = dynstr->len + added_len;
    if ( new_size >= dynstr->cap )
    {
        size_t new_cap = dynstr->cap;
        while ( new_cap < new_size )
            new_cap *= 2;

        int rv = dynstr_resize( dynstr, new_size );
        if ( rv != RV_SUCCESS )
        {
            f_stack_trace();
            return rv;
        }
    }

    memmove( dynstr->data + added_len - 1, dynstr->data, dynstr->len );
    strcpy( dynstr->data, s );
    dynstr->data[ new_size ] = '\0';
    dynstr->len              = new_size;

    return RV_SUCCESS;
}

#if !defined( _GNU_SOURCE ) && !defined( __APPLE__ )
static int vasprintf( char **strp, const char *fmt, va_list args )
{
    va_list vaList;
    va_copy( vaList, args );

    int size = vsnprintf( NULL, 0, fmt, args );

    if ( size < 0 )
        return size;

    *strp = malloc( size + 1 );
    if ( !*strp )
        return -1;


    int result = vsnprintf( *strp, size + 1, fmt, vaList );
    va_end( args );

    return result;
}
#endif

int dynstr_appendf( DynamicString dynstr, const char *fmt, ... )
{
    str_t buffer = malloc( strlen( fmt ) );

    va_list vaList;
    va_start( vaList, fmt );
    vasprintf( &buffer, fmt, vaList );
    va_end( vaList );

    if ( dynstr_append( dynstr, buffer ) == RV_ERROR )
    {
        f_stack_trace();
        return RV_ERROR;
    }

    free( buffer );
    return RV_SUCCESS;
}

int dynstr_slice( DynamicString dynstr, size_t start_idx, ssize_t end_idx )
{
    if ( start_idx >= dynstr->len )
        return fwarnx_ret( RV_EXCEPTION, "start index=%zu out of bounds", start_idx );
    if ( end_idx < -1 || ( end_idx != -1 && start_idx > ( size_t ) end_idx ) )
        return fwarnx_ret( RV_EXCEPTION,
                           "invalid indices (start=%zu, end=%zi)",
                           start_idx,
                           end_idx );

    size_t end       = end_idx == -1 ? dynstr->len : ( size_t ) end_idx;
    const size_t len = end - start_idx;
    memmove( dynstr->data, dynstr->data + start_idx, len );
    dynstr->data[ len ] = '\0';
    dynstr->len         = len;
    return RV_SUCCESS;
}

int dynstr_slice_e( DynamicString dynstr, ssize_t end_idx )
{
    const size_t len =
            ( end_idx < 0 ) ? ( dynstr->len + 1 + end_idx ) : ( size_t ) end_idx;
    if ( len >= dynstr->len )
        return fwarnx_ret( RV_EXCEPTION, "invalid index: %zi", len );

    dynstr->data[ len ] = '\0';
    dynstr->len         = len;
    return RV_SUCCESS;
}

int dynstr_slice_s( DynamicString dynstr, size_t start_idx )
{
    if ( start_idx >= dynstr->len )
        return fwarnx_ret( RV_EXCEPTION, "invalid index: %zu", start_idx );

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
    return RV_SUCCESS;
}

/* ====/ /==== */

str_t dynstr_to_str( ConstDynamicString dynstr )
{
    str_t ret = strndup( dynstr->data, dynstr->len );
    if ( ret == NULL )
        fwarn( "strndup" );
    return ret;
}


string_t dynstr_data( ConstDynamicString dynstr )
{
    return dynstr->data;
}

size_t dynstr_len( ConstDynamicString dynstr )
{
    return dynstr->len;
}
