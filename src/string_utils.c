//
// Created by MacBook on 28.10.2024.
//

#include "string_utils.h"

#include "headers/assert_that.h" /* assert_that */
#include "headers/errors.h"      /* *warn* */
#include "headers/foreach.h"     /* foreach_arr */
#include "headers/simple_math.h" /* min */
#include "structs/dynarr.h"      /* List */
#include "structs/dynstring.h"   /* dynstr */

#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

bool string_is_blank( const string_t s )
{
    foreach_str ( c, s )
        if ( !isspace( c ) )
            return false;

    return true;
}

bool string_is_blank_l( const string_t s, const size_t len )
{
    for ( size_t i = 0; i < len; ++i )
        if ( !isspace( s[ i ] ) )
            return false;

    return true;
}


// `string_stripped` is implemented separately from `strip` for memory efficiency

str_t string_stripped_l( const string_t s, size_t length )
{
    size_t start_idx = -1;
    size_t end_idx   = length;

    bool started = false, ended = false;
    for ( size_t i = 0; i < length; ++i )
    {
        if ( !started && !isspace( s[ i ] ) )
        {
            started   = true;
            start_idx = i;
        }

        if ( !ended && !isspace( s[ length - i - 1 ] ) )
        {
            ended   = true;
            end_idx = length - i - 1;
        }

        if ( started && ended )
            break;
    }

    if ( !( started && ended ) )
    {
        char *const new = malloc( 1 );
        if ( new != NULL )
            new[ 0 ] = '\0';
        return new;
    }

    length          = end_idx - start_idx + 1;
    char *const new = malloc( length + 1 );
    if ( new == NULL )
        return ( void * ) fflwarn_ret( NULL, "malloc" );

    for ( size_t i = 0; i < length; ++i )
    {
        new[ i ] = s[ i + start_idx ];
    }
    new[ length ] = '\0';

    return new;
}

str_t string_stripped( const string_t s )
{
    return string_stripped_l( s, strlen( s ) );
}

void string_strip( char *const s )
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


str_t string_escaped( const string_t old )
{
    if ( old == NULL )
        return ( void * ) fwarnx_ret( NULL, "string must not be null" );

    const size_t len = strlen( old );

    char *const new = calloc( len * 2 + 1, 1 );
    if ( new == NULL )
        return ( void * ) fwarn_ret( NULL, "calloc" );

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


str_t string_unescaped( const string_t old )
{
    if ( old == NULL )
        return ( void * ) fwarnx_ret( NULL, "string must not be null" );

    const size_t len = strlen( old );

    char *const new = calloc( len + 1, 1 );
    if ( new == NULL )
        return ( void * ) fwarn_ret( NULL, "calloc" );

    size_t new_idx = 0;
    for ( size_t i = 0; i < len; ++i, ++new_idx )
    {
        const char old_c = old[ i ];
        if ( old_c != '\\' )
        {
            new[ new_idx ] = old[ i ];
            continue;
        }

        switch ( old[ ++i ] )
        {
            case 'n':
                new[ new_idx ] = '\n';
                break;
            case 't':
                new[ new_idx ] = '\t';
                break;
            case 'r':
                new[ new_idx ] = '\r';
                break;
            case 'f':
                new[ new_idx ] = '\f';
                break;
            case 'v':
                new[ new_idx ] = '\v';
                break;

            default: // backslash, double-quote, ...?
                new[ new_idx ] = old[ i ];
                break;
        }
    }

    return new;
}


str_t string_reversed( const string_t s )
{
    char *const new = strdup( s );
    if ( new == NULL )
        return ( void * ) fwarn_ret( NULL, "strdup" );

    string_reverse( new );
    return new;
}

void string_reverse( char *const s )
{
    const size_t l = strlen( s );

    for ( size_t i = 0; i < l / 2; ++i )
    {
        const char aux = s[ i ];
        s[ i ]         = s[ l - i - 1 ];
        s[ l - i - 1 ] = aux;
    }
}


void string_to_upper( char *const str )
{
    size_t idx = 0;
    while ( str[ idx ] != '\0' )
    {
        if ( islower( str[ idx ] ) )
            str[ idx ] -= 0x20;

        ++idx;
    }
}

str_t string_as_upper( const string_t old )
{
    const size_t len = strlen( old );

    char *const new = malloc( len + 1 );
    if ( new == NULL )
        return ( void * ) fwarn_ret( NULL, "malloc" );

    new[ len ] = '\0';

    for ( size_t i = 0; i < len; ++i )
    {
        char c = old[ i ];
        if ( islower( c ) )
            c -= 0x20;
        new[ i ] = c;
    }
    return new;
}

void string_to_lower( char *const str )
{
    size_t idx = 0;
    while ( str[ idx ] != '\0' )
    {
        if ( isupper( str[ idx ] ) )
            str[ idx ] += 0x20;

        ++idx;
    }
}

str_t string_as_lower( const string_t old )
{
    const size_t len = strlen( old );

    char *const new = malloc( len + 1 );
    if ( new == NULL )
        return ( void * ) fwarn_ret( NULL, "malloc" );

    new[ len ] = '\0';

    for ( size_t i = 0; i < len; ++i )
    {
        char c = old[ i ];
        if ( isupper( c ) )
            c += 0x20;
        new[ i ] = c;
    }
    return new;
}


str_t string_replaced( string_t const str, const string_t old, const string_t new )
{
    if ( *old == '\0' )
        return ( void * ) fwarnx_ret( NULL, "cannot replace \"%s\"", old );

    const char *curr              = str;
    struct dynamic_string *result = dynstr_init();
    do
    {
        const char *next = strstr( curr, old );
        if ( next == NULL )
        {
            goto_on_error( ERROR, dynstr_append( result, curr ) );
            break;
        }
        const size_t app_len = next - curr;
        if ( app_len > 0 )
            goto_on_error( ERROR, dynstr_appendn( result, curr, app_len ) );

        goto_on_error( ERROR, dynstr_append( result, new ) );

        curr = next + strlen( old );
    }
    while ( *curr != '\0' );

    char *const ret = dynstr_data_copy( result );
    dynstr_destroy( result );

    if ( ret == NULL )
        return ( void * ) f_stack_trace( NULL );

    return ret;

ERROR:
    dynstr_destroy( result );
    return ( void * ) f_stack_trace( NULL );
}

int string_replace( const str_t string, const string_t old, const string_t new )
{
    if ( string == NULL || old == NULL || new == NULL )
        return fwarnx_ret( RV_EXCEPTION, "an argument is NULL" );

    const size_t repl_len = strlen( old );
    if ( repl_len != strlen( new ) )
        return fwarnx_ret(
                RV_EXCEPTION,
                "replacement must be the same length as the string it's replacing" );

    if ( repl_len == 0 )
        return RV_SUCCESS;

    char *occ = strstr( string, old );
    if ( occ == NULL )
        return RV_SUCCESS;

    do
        strncpy( occ, new, repl_len );
    while ( ( occ = strstr( occ, old ) ) != NULL );

    return RV_SUCCESS;
}


Private const strsplit_mode_t ALL_MODES = STRSPLIT_STRIP_RESULTS | STRSPLIT_EXCLUDE_EMPTY
                                          | STRSPLIT_KEEP_DELIM_AFTER
                                          | STRSPLIT_KEEP_DELIM_BEFORE;

Private ssize_t string_split_empty( str_t **str_arr_cont, const string_t string )
{
    const size_t count = strlen( string );
    *str_arr_cont      = calloc( count, sizeof( str_t * ) );
    if ( *str_arr_cont == NULL )
        return fwarn_ret( RV_ERROR, "string array calloc" );

    for ( size_t i = 0; i < count; ++i )
    {
        if ( ( ( *str_arr_cont )[ i ] = malloc( 2 ) ) == NULL )
        {
            while ( i-- > 0 )
                free( ( *str_arr_cont )[ i ] );
            free( *str_arr_cont );
            return fwarn_ret( RV_ERROR, "string malloc" );
        }
        ( *str_arr_cont )[ i ][ 0 ] = string[ i ];
        ( *str_arr_cont )[ i ][ 1 ] = '\0';
    }
    return ( ssize_t ) count;
}

ssize_t string_split( str_t **str_arr_cont,
                      const string_t string,
                      const string_t split_tok,
                      const strsplit_mode_t mode )
{
    if ( split_tok == NULL )
        return fwarnx_ret( RV_EXCEPTION, "invalid split token" );
    if ( string == NULL )
        return fwarnx_ret( RV_EXCEPTION, "string cannot be null" );

    if ( mode & ~ALL_MODES ) // contains mode not in ALL_MODES
        return fwarnx_ret( RV_EXCEPTION,
                           "invalid mode: 0x%X, only valid options are 0x%X",
                           mode,
                           ALL_MODES );

    if ( strcmp( split_tok, "" ) == 0 )
    {
        const ssize_t rv = string_split_empty( str_arr_cont, string );
        if ( rv == RV_ERROR )
            return f_stack_trace( RV_ERROR );
        return rv;
    }

    struct dynamic_array *ls = list_init_type( str_t );
    if ( ls == NULL )
        return f_stack_trace( RV_ERROR );

    const size_t split_tok_len = strlen( split_tok );

    bool is_first = true;
    const char *curr;
    const char *next = string;
    while ( ( curr = next ) != NULL )
    {
        const size_t start_offset =
                is_first || mode & STRSPLIT_KEEP_DELIM_AFTER ? 0 : split_tok_len;

        if ( is_first )
        {
            next     = strstr( curr, split_tok );
            is_first = false;
        }
        else
            next = strstr( curr + split_tok_len, split_tok );

        const char *start       = curr + start_offset;
        const size_t end_offset = mode & STRSPLIT_KEEP_DELIM_BEFORE ? split_tok_len : 0;

        if ( mode & STRSPLIT_EXCLUDE_EMPTY
             && ( ( next == NULL && *start == '\0' )
                  || ( next != NULL && next - start + end_offset == 0 ) ) )
            continue;

        const size_t len = next == NULL ? strlen( start ) : next - start + end_offset;

        if ( mode & STRSPLIT_EXCLUDE_EMPTY && mode & STRSPLIT_STRIP_RESULTS
             && string_is_blank_l( start, len ) )
            continue;

        char *const dup = mode & STRSPLIT_STRIP_RESULTS ? string_stripped_l( start, len )
                                                        : strndup( start, len );
        if ( dup == NULL )
        {
            list_destroy( ls );
            return fwarn_ret( RV_ERROR, "strdup" );
        }

        on_fail( list_append( ls, &dup ) )
        {
            list_destroy( ls );
            return f_stack_trace( RV_ERROR );
        }
    }

    const ssize_t count = ( ssize_t ) list_size( ls );
    *str_arr_cont       = list_items_copy( ls );

    list_destroy( ls );
    return count;
}

ssize_t string_split_regex( str_t **str_arr_cont,
                            const string_t string,
                            const regex_t *const __restrict regexp,
                            const strsplit_mode_t mode )
{
    if ( string == NULL )
        return fwarnx_ret( RV_EXCEPTION, "string may not be NULL" );
    if ( regexp == NULL )
        return fwarnx_ret( RV_EXCEPTION, "regexp may not be NULL" );

    if ( mode & ~ALL_MODES ) // contains mode not in ALL_MODES
        return fwarnx_ret( RV_EXCEPTION,
                           "invalid mode: 0x%X, only valid options are 0x%X",
                           mode,
                           ALL_MODES );

    struct dynamic_array *ls = list_init_type( str_t );
    if ( ls == NULL )
        return f_stack_trace( RV_ERROR );

    const char *searched = string;

    regmatch_t regmatch = { 0 };

    const char *next_start = NULL;
    do
    {
        const int ree_rv = regexec( regexp, searched, 1, &regmatch, 0 );
        if ( ree_rv == REG_NOMATCH )
        {
            const char *to_dup =
                    !( mode & STRSPLIT_KEEP_DELIM_AFTER ) || next_start == NULL
                            ? searched
                            : next_start;
            if ( mode & STRSPLIT_EXCLUDE_EMPTY && *to_dup == '\0' )
                break;
            char *const dup = strdup( to_dup );
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
                !( mode & STRSPLIT_KEEP_DELIM_AFTER ) || next_start == NULL ? searched
                                                                            : next_start;

        const size_t end_offset_from_searched =
                mode & STRSPLIT_KEEP_DELIM_BEFORE ? regmatch.rm_eo : regmatch.rm_so;

        const size_t actual_len = searched + end_offset_from_searched - actual_start;

        if ( actual_len != 0 || !( mode & STRSPLIT_EXCLUDE_EMPTY ) )
        {
            str_t duped = strndup( actual_start, actual_len );
            if ( list_append( ls, &duped ) != RV_SUCCESS )
            {
                list_destroy( ls );
                return f_stack_trace( RV_ERROR );
            }
        }
        next_start = mode & STRSPLIT_KEEP_DELIM_AFTER
                             ? searched + regmatch.rm_so
                             : searched + end_offset_from_searched;
        searched += regmatch.rm_eo;
    }
    while ( regmatch.rm_eo != -1 && regmatch.rm_so != -1 );

    *str_arr_cont      = list_items_copy( ls );
    const size_t count = list_size( ls );

    list_destroy( ls );

    return ( ssize_t ) count;
}

void string_split_destroy( const size_t size, str_t **str_arr_cont )
{
    foreach_arr ( char *const, string, *str_arr_cont, size )
        free( string );

    free( *str_arr_cont );
    *str_arr_cont = NULL;
}


str_t string_join( const size_t len, const string_t strarr[ len ], const string_t joiner )
{
    if ( strarr == NULL )
        return ( void * ) fwarnx_ret( NULL, "strarr must not be NULL" );
    if ( joiner == NULL )
        return ( void * ) fwarnx_ret( NULL, "joiner must not be NULL" );

    if ( len == 0 )
        return calloc( 1, 1 ); // empty string

    DynString *builder = dynstr_init();
    if ( builder == NULL )
        return ( void * ) f_stack_trace( NULL );

    const size_t joiner_len = strlen( joiner );
    foreach_arr ( const string_t, string, strarr, len )
    {
        if ( dynstr_append( builder, string ) < 0 )
            return ( void * ) f_stack_trace( NULL );

        if ( foreach_index_string < len - 1 )
            if ( dynstr_appendn( builder, joiner, joiner_len ) != ( ssize_t ) joiner_len )
                return ( void * ) f_stack_trace( NULL );
    }

    const str_t data_copy = dynstr_data_copy( builder );
    dynstr_destroy( builder );

    return data_copy;
}


str_t mul_uint_strings( const string_t str_1, const string_t str_2 )
{
    typedef uint8_t digit_t;

    const size_t len_1 = strlen( str_1 );
    const size_t len_2 = strlen( str_2 );
    size_t result_len  = len_1 + len_2;

    char *result = malloc( result_len + 1 );
    if ( result == NULL )
        return ( void * ) fwarn_ret( NULL, "malloc" );
    memset( result, '0', result_len );
    result[ result_len ] = '\0';

    // Perform multiplication using the "long multiplication" algorithm
    for ( ssize_t i = ( ssize_t ) len_1 - 1; i >= 0; --i )
    {
        digit_t carry = 0;
        for ( ssize_t j = ( ssize_t ) len_2 - 1; j >= 0; --j )
        {
            const digit_t prod = ( str_1[ i ] - '0' ) * ( str_2[ j ] - '0' ) + carry
                                 + ( result[ i + j + 1 ] - '0' );
            result[ i + j + 1 ] = ( char ) ( prod % 10 + '0' );
            carry               = prod / 10;
        }
        result[ i ] = ( char ) ( result[ i ] + carry );
    }

    // Remove leading zeros
    size_t start = 0;
    while ( start < result_len && result[ start ] == '0' )
        ++start;

    char *final_result = start == result_len ? strdup( "0" ) : strdup( result + start );
    free( result );

    return final_result;
}

str_t add_uint_strings( const string_t str_1, const string_t str_2 )
{
    typedef uint8_t digit_t;

    struct dynamic_string *result = dynstr_init();
    if ( result == NULL )
        return ( void * ) f_stack_trace( NULL );

    const size_t str_1_len = strlen( str_1 );
    const size_t str_2_len = strlen( str_2 );

    digit_t carry = 0;
    for ( size_t idx = 0; idx < max_u64( str_1_len, str_2_len ); ++idx )
    {
        const digit_t str_1_digit =
                idx < str_1_len ? str_1[ str_1_len - idx - 1 ] - '0' : 0;
        const digit_t str_2_digit =
                idx < str_2_len ? str_2[ str_2_len - idx - 1 ] - '0' : 0;

        const digit_t val = str_1_digit + str_2_digit + carry;

        carry             = val >= 10;
        const digit_t dig = val % 10;

        char app = ( char ) ( dig + '0' );
        goto_on_fail( ERROR, dynstr_appendn( result, &app, 1 ) );
    }
    if ( carry )
        goto_on_fail( ERROR, dynstr_appendn( result, "1", 1 ) );

    char *const ret = string_reversed( dynstr_data( result ) );
    dynstr_destroy( result );
    return ret;

ERROR:
    dynstr_destroy( result );
    return ( void * ) f_stack_trace( NULL );
}


void string_strip_lead_zeroes( char *const s )
{
    size_t fst_non0_idx = 0;
    while ( s[ fst_non0_idx ] == '0' )
        fst_non0_idx++;

    if ( s[ fst_non0_idx ] == '\0' )
    {
        s[ 0 ] = '0';
        s[ 1 ] = '\0';
    }
    else
    {
        size_t new_len = strlen( s + fst_non0_idx );
        memmove( s, s + fst_non0_idx, new_len );
        s[ new_len ] = '\0';
    }
}


Private int hex_char_to_int( const char hex )
{
    if ( hex >= '0' && hex <= '9' )
        return hex - '0';
    if ( hex >= 'a' && hex <= 'f' )
        return hex - 'a' + 10;
    if ( hex >= 'A' && hex <= 'F' )
        return hex - 'A' + 10;

    return -1; // Invalid hex character
}

Private bool issign( const char c )
{
    return c == '-' || c == '+';
}


char *hex_to_decimal( const char *hex )
{
    const size_t len = strlen( hex );

    struct dynamic_string *dynstr = dynstr_init_as( "0" );
    if ( dynstr == NULL )
        goto ERROR;

    for ( size_t i = 0; i < len; ++i )
    {
        if ( i == 0 && issign( hex[ i ] ) )
            continue;

        const int digit = hex_char_to_int( hex[ i ] );
        assert_that( digit >= 0,
                     "invalid input string: '%s' (offending digit: '%c')",
                     hex,
                     hex[ i ] );

        unsigned long carry = digit;
        for ( ssize_t j = ( ssize_t ) dynstr_len( dynstr ) - 1; j >= 0; --j )
        {
            const unsigned long temp = ( dynstr_data( dynstr )[ j ] - '0' ) * 16 + carry;
            dynstr_set_at( dynstr, j, ( char ) ( '0' + temp % 10 ) );
            carry = temp / 10;
        }

        while ( carry > 0 )
        {
            char p = ( char ) ( '0' + carry % 10 );
            goto_on_error( ERROR, dynstr_prependn( dynstr, &p, 1 ) );
            carry /= 10;
        }
    }

    if ( issign( hex[ 0 ] ) )
        goto_on_fail( ERROR, dynstr_prependn( dynstr, hex + 0, 1 ) );

    char *const decimal = dynstr_data_copy( dynstr );
    dynstr_destroy( dynstr );
    return decimal;

ERROR:
    if ( dynstr != NULL )
        dynstr_destroy( dynstr );
    return ( void * ) f_stack_trace( NULL );
}
