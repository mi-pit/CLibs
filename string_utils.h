//
// Created by MacBook on 28.10.2024.
//

#ifndef CLIBS_STRING_UTILS_H
#define CLIBS_STRING_UTILS_H

#include "Dev/attributes.h" /* UseResult */

#include <regex.h>     /* regex_t */
#include <stdbool.h>   /* bool */
#include <string.h>    /* import */
#include <sys/types.h> /* ssize_t */


typedef const char *string_t;
typedef char *str_t;

#if __STDC_VERSION__ < 202311L && !defined( __APPLE__ )
#include <stdlib.h> /* malloc */

#define strndup string_nduplicate
#define strdup  string_duplicate

LibraryDefined UseResult str_t string_nduplicate( string_t s, size_t l )
{
    str_t n = malloc( l + 1 );
    if ( n == NULL )
        return NULL;

    strncpy( n, s, l );
    n[ l ] = '\0';
    return n;
}

LibraryDefined UseResult str_t string_duplicate( string_t s )
{
    size_t l = strlen( s );
    str_t n  = malloc( l + 1 );
    if ( n == NULL )
        return NULL;

    n[ l ] = '\0';
    strcpy( n, s );
    return n;
}
#endif // ndef strdup

#if !defined( _GNU_SOURCE ) && !defined( __APPLE__ ) // non-standard
/**
 * Like `vsprintf`, except it heap-allocates memory for the resulting string.
 * (*strp) may be passed to free(3)
 */
int vasprintf( str_t *strp, string_t fmt, va_list args )
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

int asprintf( str_t *strp, string_t fmt, ... )
{
    va_list va;
    va_start( va, fmt );
    int rv = vasprintf( strp, fmt, va );
    va_end( va );
    return rv;
}
#endif


/**
 * Heap-allocates a new string with all whitespace (as defined in isspace(3)) from either end stripped.
 * @return  New string – copy of the original with no whitespace at either end.
 *          Pointer should be freed with free(3).
 */
UseResult str_t string_stripped( string_t );
/**
 * Removes all whitespace from either end of the string.
 * This is done in place.
 */
void string_strip( str_t );


#define ESCAPED_CHARS "\n\t\r\f\v\"\\"

/**
 * Escapes characters defined in ESCAPED_CHARS
 * @example
 * @code
 * #include "string_utils.h"
 *
 * string_t old = "Hello, \"World\"!\n";
 * str_t esc = string_escaped( old );
 * printf( "\"%s\"\n", old );
 * printf( "\"%s\"\n", esc );
 * @endcode
 * prints
 * @code
 * "Hello, "World"!
 * "
 * "Hello,\"World\"!\n"
 * @endcode
 * @return New escaped string. Pointer should be freed with free(3).
 */
UseResult str_t string_escaped( string_t );

/**
 * Creates a new string with the same characters as the supplied one, reversed.
 *
 * @return New reversed string. Pointer should be freed with free(3).
 */
UseResult str_t string_reversed( string_t );
/**
 * Reverses the string.
 * This is done in place.
 */
void string_reverse( str_t );


/**
 * Replaces all lowercase letters to corresponding uppercase letters
 */
void string_to_upper( str_t );
/**
 * Creates a new string with all lowercase letters replaced with uppercase ones
 * @return new string, should be freed
 */
UseResult str_t string_as_upper( string_t );

/**
 * Replaces all uppercase letters to corresponding lowercase letters
 */
void string_to_lower( str_t );
/**
 * Creates a new string with all uppercase letters replaced with lowercase ones
 * @return new string, should be freed
 */
UseResult str_t string_as_lower( string_t );


/**
 * Creates a new string with all occurrences of the sub-string ‹old› replaced with ‹new›
 * @param old
 * @param new
 * @return new string, should be freed
 */
UseResult str_t string_replaced( string_t, string_t old, string_t new );


#define STRSPLIT_EXCLUDE_EMPTY     0x01
#define STRSPLIT_KEEP_DELIM_BEFORE 0x02
#define STRSPLIT_KEEP_DELIM_AFTER  0x04

/**
 * Flags for the string_split[_regex] functions
 * @code
 * STRSPLIT_EXCLUDE_EMPTY       // = 0x01
 *     - resulting string array doesn't include empty strings ("")
 *
 * STRSPLIT_KEEP_DELIM_BEFORE   // = 0x02
 *     - items include the delimiting strings;
 *       the delim is included at the end of the previous item
 *
 * STRSPLIT_KEEP_DELIM_AFTER    // = 0x04
 *     - items include the delimiting strings;
 *       the delim is included at the start of the next item
 * @endcode
 */
typedef unsigned int strsplit_mode_t;

/**
 * Splits ‹str› at places matching ‹split_tok›<br>
 * Substring must match perfectly<br>
 *
 * @param str_arr_cont  container for the string_array for the result; the resulting
 *                      string array is allocated in this function
 * @param string        string to be split
 * @param split_tok     boundary
 * @param mode          see strsplit_mode_t
 * @return number of strings in the result,
 *         -1 on allocation fail,
 *         -2 if split_tok is invalid
 */
ssize_t string_split( str_t **str_arr_cont,
                      string_t string,
                      string_t split_tok,
                      strsplit_mode_t mode );
/**
 * Splits ‹str› at places matching ‹regex›<br>
 * Substring must match ‹regex›<br>
 *
 * @param str_arr_cont  container for the string_array for the result; the resulting
 *                      string array is allocated in this function
 * @param string        string to be split
 * @param regexp        boundary; compiled regex
 * @param mode          see strsplit_mode_t
 * @return number of strings in the result,
 *         RV_ERROR (-1) on allocation fail or regex error,
 *         RV_EXCEPTION (-2) if split_tok is invalid
 */
ssize_t string_split_regex( str_t **str_arr_cont,
                            string_t __restrict string,
                            const regex_t *__restrict regexp,
                            strsplit_mode_t mode );


/**
 * @param full_path Full path to file
 * @return pointer to the last part of ‹full_path›
 */
string_t get_file_name( string_t full_path );

#endif //CLIBS_STRING_UTILS_H
