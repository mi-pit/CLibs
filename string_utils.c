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
        return fflwarn_ret_p( NULL, "malloc" );

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


str_t string_escaped( string_t old )
{
    if ( old == NULL )
        return fwarnx_ret_p( NULL, "string must not be null" );

    size_t len = strlen( old );

    str_t new = calloc( len * 2 + 1, 1 );
    if ( new == NULL )
        return fwarn_ret_p( NULL, "calloc" );

    size_t new_idx = 0;
    for ( size_t i = 0; i < len; ++i, ++new_idx )
    {
        if ( strchr( ESCAPED_CHARS, old[ i ] ) )
            new[ new_idx++ ] = '\\';

        switch ( old[ i ] )
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

            default: // backslash, double-quote, ...?
                new[ new_idx ] = old[ i ];
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
    if ( string == NULL )
        return fwarnx_ret( RV_EXCEPTION, "string cannot be null" );

    List ls = list_init_type( str_t );
    if ( ls == NULL )
    {
        f_stack_trace();
        return RV_ERROR;
    }

    size_t split_tok_len = strlen( split_tok );

    bool is_first = true;
    const char *curr;
    const char *next = string;
    while ( ( curr = next ) != NULL )
    {
        size_t start_offset =
                ( is_first || mode & STRSPLIT_KEEP_DELIM_POST ) ? 0 : split_tok_len;

        if ( is_first )
        {
            next     = strstr( curr, split_tok );
            is_first = false;
        }
        else
            next = strstr( curr + split_tok_len, split_tok );

        const char *start = curr + start_offset;
        size_t end_offset = ( mode & STRSPLIT_KEEP_DELIM_PRE ) ? split_tok_len : 0;

        if ( mode & STRSPLIT_EXCLUDE_EMPTY &&
             ( ( next == NULL && *start == '\0' ) ||
               ( next != NULL && ( ( next - start + end_offset ) == 0 ) ) ) )
            continue;

        str_t dup = next == NULL ? strdup( start )
                                 : strndup( start, next - start + end_offset );
        if ( dup == NULL )
        {
            list_destroy( ls );
            return fflwarn_ret( RV_ERROR, "strdup" );
        }

        if ( list_append( ls, &dup ) != RV_SUCCESS )
        {
            list_destroy( ls );
            return RV_ERROR;
        }
    }

    ssize_t count     = ( ssize_t ) list_size( ls );
    ( *str_arr_cont ) = list_items_copy( ls );

    list_destroy( ls );
    return count;
}

ssize_t string_split_regex( str_t **str_arr_cont,
                            string_t const __restrict string,
                            const regex_t *const __restrict regexp,
                            strsplit_mode_t mode )
{
    List ls = list_init_type( str_t );
    if ( ls == NULL )
    {
        f_stack_trace();
        return RV_ERROR;
    }

    const char *searched = string;

    regmatch_t regmatch = { 0 };

    const char *last_end = NULL;
    do
    {
        int ree_rv = regexec( regexp, searched, 1, &regmatch, 0 );
        if ( ree_rv == REG_NOMATCH )
        {
            const char *to_dup =
                    ( !( mode & STRSPLIT_KEEP_DELIM_POST ) || last_end == NULL )
                            ? searched
                            : last_end;
            if ( ( mode & STRSPLIT_EXCLUDE_EMPTY ) && *to_dup == '\0' )
                break;
            str_t dup = strdup( to_dup );
            list_append( ls, &dup );
            break;
        }
        if ( ree_rv != 0 )
        {
            char buffer[ 128 ] = { 0 };
            regerror( ree_rv, regexp, buffer, sizeof buffer );
            fwarnx( "%s", buffer );
            list_destroy( ls );
            return RV_ERROR;
        }

        const char *actual_start =
                ( !( mode & STRSPLIT_KEEP_DELIM_POST ) || last_end == NULL ) ? searched
                                                                             : last_end;

        size_t end_offset_from_searched =
                ( mode & STRSPLIT_KEEP_DELIM_PRE ) ? regmatch.rm_eo : regmatch.rm_so;

        size_t actual_len = ( searched + end_offset_from_searched ) - actual_start;

        if ( actual_len != 0 || !( mode & STRSPLIT_EXCLUDE_EMPTY ) )
        {
            str_t duped = strndup( actual_start, actual_len );
            if ( list_append( ls, &duped ) != RV_SUCCESS )
            {
                f_stack_trace();
                list_destroy( ls );
                return RV_ERROR;
            }
        }
        last_end = searched + end_offset_from_searched;
        searched = searched + regmatch.rm_eo;
    }
    while ( regmatch.rm_eo != -1 && regmatch.rm_so != -1 );

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
