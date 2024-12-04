//
// Created by MacBook on 28.10.2024.
//

#include "string_utils.h"

#include "dynarr.h" /* List */
#include "errors.h" /* *warn* */
#include "swexpr.h" /* swex_* */

#include <ctype.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED( EXPR ) ( ( void ) ( EXPR ) )

// string_stripped( string_t ) is implemented separately for memory efficiency
str_t string_stripped( string_t s )
{
    size_t str_len = strlen( s );

    size_t start_idx = -1;
    size_t end_idx   = str_len;

    bool started = false, ended = false;
    for ( size_t i = 0; i < str_len; ++i )
    {
        if ( !started && !isspace( s[ i ] ) )
        {
            started   = true;
            start_idx = i;
        }

        if ( !ended && !isspace( s[ str_len - i - 1 ] ) )
        {
            ended   = true;
            end_idx = str_len - i - 1;
        }

        if ( started && ended )
            break;
    }

    if ( !( started && ended ) )
    {
        str_t new = malloc( 1 );
        if ( new != NULL )
            new[ 0 ] = '\0';
        return new;
    }

    str_len   = end_idx - start_idx + 1;
    str_t new = malloc( str_len + 1 );
    if ( new == NULL )
        return fflwarn_ret_p( "%s", "malloc" );

    for ( size_t i = 0; i < str_len; ++i )
    {
        new[ i ] = s[ i + start_idx ];
    }
    new[ str_len ] = '\0';

    return new;
}

void string_strip( str_t s )
{
    /*
    "\n  \n \t\t\rHovno prdel.\nSračka.  \n\n \n\t\n"
        |
        V
    "Hovno prdel.\nSračka."
    */

    size_t str_len = strlen( s );

    size_t start_idx = 0;
    size_t end_idx   = str_len - 1;

    bool started = false, ended = false;
    for ( size_t i = 0; i < str_len; ++i )
    {
        if ( !started && !isspace( s[ i ] ) )
        {
            started   = true;
            start_idx = i;
        }

        if ( !ended && !isspace( s[ str_len - i - 1 ] ) )
        {
            ended   = true;
            end_idx = str_len - i - 1;
        }

        if ( started && ended )
            break;
    }
    if ( !( started && ended ) )
    {
        s[ 0 ] = '\0';
        return;
    }

    str_len = end_idx - start_idx + 1;
    memmove( s, s + start_idx, str_len );
    s[ str_len ] = '\0';
}


str_t string_escaped( string_t s )
{
    if ( s == NULL )
    {
        errno = EINVAL;
        return fflwarn_ret_p( "%s", "string must not be null" );
    }

    size_t len = strlen( s );

    str_t new = calloc( len * 2 + 1, 1 );
    if ( new == NULL )
        return fflwarn_ret_p( "%s", "calloc" );

    size_t new_idx = 0;
    for ( size_t i = 0; i < len; ++i, ++new_idx )
    {
        if ( strchr( ESCAPED_CHARS, s[ i ] ) )
            new[ new_idx++ ] = '\\';

        swex_init_val( char, s[ i ] ) as( new[ new_idx ] )
        {
            swex_case_imm( char, '\n' ) resolve( char, 'n' );
            swex_case_imm( char, '\t' ) resolve( char, 't' );
            swex_case_imm( char, '\r' ) resolve( char, 'r' );
            swex_case_imm( char, '\f' ) resolve( char, 'f' );
            swex_case_imm( char, '\0' ) resolve( char, '0' );
            swex_case_imm( char, '\v' ) resolve( char, 'v' );

            swex_default() resolve( char, s[ i ] );
        }
        swex_finish();
    }

    return new;
}

void string_escape( str_t *strp, size_t *capp )
{
    // TODO
    UNUSED( strp && ( *capp = *capp ) );
    err( ENOSYS, "%s not implemented", __func__ );
}


str_t string_reversed( string_t s )
{
    str_t new = strdup( s );
    if ( new == NULL )
        return warn_ret_p( "%s", "strdup" );

    string_reverse( new );
    return new;
}

void string_reverse( str_t s )
{
    size_t l = strlen( s );

    char aux;
    for ( size_t i = 0; i < ( l / 2 ); ++i )
    {
        aux            = s[ i ];
        s[ i ]         = s[ l - i - 1 ];
        s[ l - i - 1 ] = aux;
    }
}


ssize_t string_split( str_t **str_arr_cont,
                      string_t str,
                      string_t split_tok,
                      bool excl_empty )
{
    if ( split_tok == NULL || ( strcmp( split_tok, "" ) == 0 ) )
    {
        errno = EINVAL;
        return fflwarnx_ret( RV_EXCEPTION, "%s", "invalid split token" );
    }

    List ls = list_init_type( str_t );
    if ( ls == NULL )
    {
        print_stack_trace();
        return RV_ERROR;
    }

    size_t split_tok_len = strlen( split_tok );
    size_t count         = 0;

    const char *curr = str;
    const char *next;
    while ( curr != NULL )
    {
        size_t offset = ( count != 0 ? split_tok_len : 0 );
        next          = strstr( curr + offset, split_tok );

        if ( !excl_empty || strlen( curr + offset ) != 0 )
        {
            str_t dup;
            if ( next == NULL )
                dup = strdup( curr + offset );
            else
                dup = strndup( curr + offset, next - curr - offset );
            if ( dup == NULL )
                return fflwarn_ret( RV_ERROR, "%s", "strdup" );

            list_append( ls, &dup );
            ++count;
        }

        if ( next == NULL )
            break;
        curr = next;
    }

    ( *str_arr_cont ) = list_items_copy( ls );

    list_destroy( ls );

    return ( ssize_t ) count;
}


string_t get_program_name( string_t const argv0 )
{
    const char *program_name = strrchr( argv0, '/' );
    if ( program_name == NULL )
    {
        return fflwarnx_ret_p( "string must contain at least one '/' (\"%s\")",
                               program_name );
    }
    return program_name + 1;
}
