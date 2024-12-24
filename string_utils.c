//
// Created by MacBook on 28.10.2024.
//

#include "string_utils.h"

#include "dynarr.h" /* List */
#include "errors.h" /* *warn* */

#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>


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
        return fwarn_ret_p( "%s", "string must not be null" );
    }

    size_t len = strlen( s );

    str_t new = calloc( len * 2 + 1, 1 );
    if ( new == NULL )
        return fwarn_ret_p( "%s", "calloc" );

    size_t new_idx = 0;
    for ( size_t i = 0; i < len; ++i, ++new_idx )
    {
        if ( strchr( ESCAPED_CHARS, s[ i ] ) )
            new[ new_idx++ ] = '\\';

        switch ( s[ i ] )
        {
            case '\n':
                new[ new_idx ] = 'n';
                break;
            case '\t':
                new[ new_idx ] = 't';
                break;
            case '\r':
                new[ new_idx ] = 'r';
                break;
            case '\f':
                new[ new_idx ] = 'f';
                break;
            case '\v':
                new[ new_idx ] = 'v';
                break;

            default:
                new[ new_idx ] = s[ i ];
                break;
        }
    }

    return new;
}


str_t string_reversed( string_t s )
{
    str_t new = strdup( s );
    if ( new == NULL )
        return warn_ret_p( NULL, "strdup" );

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
                      const string_t string,
                      const string_t split_tok,
                      strsplit_mode_t mode )
{
    if ( split_tok == NULL || ( strcmp( split_tok, "" ) == 0 ) )
    {
        // fixme: "" -> split by char
        return fwarnx_ret( RV_EXCEPTION, "invalid split token" );
    }

    List ls = list_init_type( str_t );
    if ( ls == NULL )
    {
        f_stack_trace();
        return RV_ERROR;
    }

    size_t split_tok_len = strlen( split_tok );
    size_t count         = 0;

    const char *curr = string;
    const char *next;
    while ( curr != NULL )
    {
        next = strstr( curr + ( count == 0 ? 0 : split_tok_len ), split_tok );

        size_t offset;
        if ( count == 0 || mode & STRSPLIT_KEEP_DELIM_POST )
            offset = 0;
        else
            offset = split_tok_len;

        if ( !( mode & STRSPLIT_EXCLUDE_EMPTY ) || strlen( curr + offset ) != 0 )
        {
            str_t dup;
            if ( next == NULL )
                dup = strdup( curr + offset );
            else
                dup = strndup(
                        curr + offset,
                        next - curr - offset +
                                ( mode & STRSPLIT_KEEP_DELIM_PRE ? split_tok_len : 0 ) );
            if ( dup == NULL )
                return fflwarn_ret( RV_ERROR, "strdup" );

            list_append( ls, &dup );
            ++count;
        }

        curr = next;
    }

    ( *str_arr_cont ) = list_items_copy( ls );

    list_destroy( ls );

    return ( ssize_t ) count;
}

ssize_t string_split_regex( str_t **str_arr_cont,
                            string_t __restrict string,
                            regex_t *__restrict regexp,
                            strsplit_mode_t mode )
{
    List ls = list_init_type( str_t );
    if ( ls == NULL )
    {
        f_stack_trace();
        return RV_ERROR;
    }

    const char *p = string;

    regmatch_t regmatch[ 1 ] = { 0 };

    int ree_rv;
    do
    {
        ree_rv = regexec( regexp, p, 1, regmatch, 0 );
        if ( ree_rv == REG_NOMATCH )
        {
            if ( ( mode & STRSPLIT_EXCLUDE_EMPTY ) && *p == '\0' )
                break;
            str_t dup = strdup( p );
            list_append( ls, &dup );
            break;
        }
        if ( ree_rv != 0 )
        {
            char buffer[ 64 ] = { 0 };
            regerror( ree_rv, regexp, buffer, 64 );
            fprintf( stderr, "%s\n", buffer );
            return RV_ERROR;
        }

        if ( !( mode & STRSPLIT_EXCLUDE_EMPTY ) || regmatch->rm_eo != 1 )
        {
            str_t dup = strndup( p, regmatch->rm_so );
            list_append( ls, &dup );
        }

        p = p + regmatch->rm_eo;
    }
    while ( regmatch->rm_eo != -1 && regmatch->rm_so != -1 );

    ( *str_arr_cont ) = list_items_copy( ls );
    size_t count      = list_size( ls );

    list_destroy( ls );

    return ( ssize_t ) count;
}


//

string_t get_file_name( string_t const full_path )
{
    const char *program_name = strrchr( full_path, '/' );
    if ( program_name == NULL )
        return full_path;
    if ( program_name[ 1 ] != '\0' )
        return program_name + 1;

    for ( const char *ptr = program_name - 1; ptr >= full_path; --ptr )
        if ( *ptr == '/' )
            return ptr + 1;

    return full_path;
}
