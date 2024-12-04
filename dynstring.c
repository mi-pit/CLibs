//
// Created by MacBook on 18.10.2024.
//
#include "dynstring.h"

#include "errors.h"
#include "misc.h"
#include "pointer_utils.h"
#include "string_utils.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>


struct dynamic_string {
    char *data;
    size_t len;

    size_t cap;
};

DynamicString string_init_cap( size_t capacity )
{
    DynamicString new = calloc( 1, sizeof( struct dynamic_string ) );
    if ( new == NULL )
    {
        fflwarn( "%s", "calloc" );
        return NULL;
    }

    new->cap  = capacity;
    new->len  = 0;
    new->data = calloc( new->cap, 1 );
    if ( new->data == NULL )
    {
        fflwarn( "%s", "calloc" );
        free( new );
        return NULL;
    }

    return new;
}

DynamicString string_init( void )
{
    DynamicString new = calloc( 1, sizeof( struct dynamic_string ) );
    if ( new == NULL )
    {
        fflwarn( "%s", "calloc" );
        return NULL;
    }

    new->cap  = DEFAULT_DYNSTRING_CAP;
    new->data = calloc( new->cap, 1 );
    new->len  = 0;
    if ( new->data == NULL )
    {
        fflwarn( "%s", "calloc" );
        free( new );
        return NULL;
    }

    return new;
}

DynamicString string_init_as( string_t string )
{
    size_t len        = strlen( string );
    DynamicString new = string_init_cap( len + 1 );
    if ( new == NULL )
        return NULL;

    strncpy( new->data, string, len );
    new->len = len;

    return new;
}

void string_destroy( DynamicString dstr )
{
    free( dstr->data );
    dstr->data = NULL;
    free( dstr );
}

void string_destroy_n( DynamicString *dstr_cont )
{
    free_n( ( *dstr_cont )->data );
    free_n( *dstr_cont );
}


int string_resize( DynamicString dstr, size_t new_size )
{
    char *temp = realloc( dstr->data, new_size );
    if ( temp == NULL )
    {
        warn( "%s: realloc", __func__ );
        return RV_ERROR;
    }

    dstr->data = temp;
    dstr->cap  = ( ssize_t ) new_size;
    dstr->len  = min_m( dstr->cap, dstr->len );

    return RV_SUCCESS;
}

int string_append( DynamicString dstr, char app )
{
    if ( dstr->len + 1 >= dstr->cap )
        return_on_fail( string_resize( dstr, dstr->cap * 2 ) != RV_SUCCESS );

    dstr->data[ dstr->len ]     = app;
    dstr->data[ dstr->len + 1 ] = '\0';
    dstr->len++;

    return RV_SUCCESS;
}

int string_extend( DynamicString dstr, string_t app )
{
    size_t app_len  = strlen( app );
    size_t new_size = dstr->len + app_len;
    if ( new_size >= dstr->cap )
    {
        size_t new_cap = dstr->cap;
        /* get next smallest power of 2 */
        while ( new_cap < new_size )
            new_cap *= 2;

        return_on_fail( string_resize( dstr, new_size ) );
    }

    strncpy( dstr->data + dstr->len, app, app_len );
    dstr->data[ new_size ] = '\0';
    dstr->len              = new_size;

    return RV_SUCCESS;
}


char *dynstr_to_str( DynamicString dstr )
{
    return strndup( dstr->data, dstr->len );
}

string_t dynstr_data( DynamicString dstr )
{
    return dstr->data;
}

size_t dynstr_len( DynamicString dstr )
{
    return dstr->len;
}
