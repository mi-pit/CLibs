/*
 * Utility functions and macros for working with C ASCII strings.
 *
 *
 * Created by MacBook on 28.10.2024.
 */

#ifndef CLIBS_STRING_UTILS_H
#define CLIBS_STRING_UTILS_H

#include "headers/attributes.h" /* UseResult */

#include <ctype.h>     /* is* (space, digit, ...) */
#include <regex.h>     /* regex_t */
#include <stdbool.h>   /* bool */
#include <sys/types.h> /* ssize_t */

/* include for user */
#include <ctype.h>  /* is* */
#include <string.h> /* str* */


/// Immutable string
typedef const char *string_t;
/// Mutable string
typedef char *str_t;


#if __STDC_VERSION__ < 202311L && !defined( _POSIX_C_SOURCE )
/*
 * Define
 * strdup && strndup
 */

#include <stdlib.h> /* malloc */

#define strndup string_duplicate_l
#define strdup  string_duplicate

/// `strndup` implementation (standard in POSIX and C23+)
LibraryDefined UseResult str_t string_duplicate_l( string_t s, size_t l )
{
    const str_t n = malloc( l + 1 );
    if ( n == NULL )
        return NULL;

    strncpy( n, s, l );
    n[ l ] = '\0';
    return n;
}

/// `strdup` implementation (standard in POSIX and C23+)
LibraryDefined UseResult str_t string_duplicate( string_t s )
{
    const size_t l = strlen( s );
    const str_t n  = malloc( l + 1 );
    if ( n == NULL )
        return NULL;

    n[ l ] = '\0';
    strcpy( n, s );
    return n;
}
#endif // ndef strdup

#if ( !defined( _GNU_SOURCE ) && !defined( __APPLE__ ) ) || defined( _POSIX_C_SOURCE )
// non-standard

/*
 * Define
 * asprintf && vasprintf
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef vsnprintf
/// `va_list` version of `snprintf()`
int vsnprintf( char *str, size_t size, const char *restrict format, va_list ap );
#endif

/**
 * Like `vsprintf()`, except it heap-allocates memory for the resulting string.
 * `*strp` may be passed to `free(3)`
 */
LibraryDefined int vasprintf( str_t *strp, const string_t fmt, va_list args )
{
    va_list vaList;
    va_copy( vaList, args );

    int size = vsnprintf( NULL, ( size_t ) 0, fmt, args );

    if ( size < 0 )
        return size;

    *strp = malloc( size + 1 );
    if ( !*strp )
        return -1;

    const int result = vsnprintf( *strp, ( size_t ) size + 1, fmt, vaList );
    va_end( args );

    return result;
}

/**
 * Like `sprintf()`, except it heap-allocates memory for the resulting string.
 * `*strp` may be passed to `free(3)` *
 */
LibraryDefined int asprintf( str_t *strp, const string_t fmt, ... )
{
    va_list va;
    va_start( va, fmt );
    const int rv = vasprintf( strp, fmt, va );
    va_end( va );
    return rv;
}
#endif


/**
 * Returns true if ascii string is blank
 *
 * @return true if each char in s tests positive for `isspace(3)`
 */
bool string_is_blank( string_t );
/**
 * Returns true if ascii string is blank
 *
 * @param len max string length
 * @return true if each char in s tests positive for `isspace(3)`
 */
bool string_is_blank_l( string_t, size_t len );

/**
 * Evaluates to true if string contains no characters
 *
 * @param s string
 * @return true if string is empty (`""`)
 */
bool string_is_empty( string_t s );

/**
 * Heap-allocates a new string with all whitespace (as defined in `isspace(3)`) from either end stripped.
 *
 * @return  New string – copy of the original with no whitespace at either end.
 *          Pointer should be freed with `free(3)`.
 */
UseResult str_t string_stripped( string_t );
/**
 * Heap-allocates a new string with all whitespace (as defined in `isspace(3)`) from either end stripped.
 *
 * String is treated as if it was `length` characters long (at most, may be less)
 *
 * @param length maximum number of characters read from string
 * @return  New string – copy of the original with no whitespace at either end.
 *          Pointer should be freed with free(3).
 */
UseResult str_t string_stripped_l( string_t, size_t length );
/**
 * Removes all whitespace from either end of the string.
 * This is done in place.
 */
void string_strip( str_t );


/**
 * Collapses all occurrences of `substring` so that
 * they don't repeat more than once at a time.
 *
 * @param string    string
 * @param substring sub
 */
void string_collapse( str_t string, string_t substring );


/**
 * Modifies the string so that for each backspace, one previous char is removed.
 */
void string_collapse_backspaces( str_t );


/// Charset of special characters that can be escaped
#define ESCAPED_CHARS "\n\t\r\f\v\"\'\\\b\a"

/**
 * Escapes characters defined in `ESCAPED_CHARS`
 * <p>
 * Example:
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
 * "Hello, \"World\"!\n"
 * @endcode
 * </p>
 * @return New escaped string. Pointer should be freed with `free(3)`.
 */
UseResult str_t string_escaped( string_t );

/// Opposite of `string_escaped()`
UseResult str_t string_unescaped( string_t );

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
 * Creates a new string with all occurrences of the sub-string `old` replaced with `new`
 *
 * @param old old substring
 * @param new new substring
 * @return new string, should be freed
 */
UseResult str_t string_replaced( string_t, string_t old, string_t new );

/**
 * Replaces all occurrences of a substring with another one.
 * This is done in-place.
 *
 * The substitution string must be the same length as the old substituted string.
 * (for arbitrary lengths use `string_replaced`)
 *
 * @param string    this string is modified
 * @param old   must be the same length as new
 * @param new   must be the same length as old
 * @return `RV_SUCCESS`/`RV_EXCEPTION`
 */
int string_replace( str_t string, string_t old, string_t new );


typedef unsigned int streq_mode_t;

#define STREQ_IGNORE_CASE       ( 1 << 0 )
#define STREQ_IGNORE_WHITESPACE ( 1 << 1 )

bool string_equal( string_t s1, string_t s2, streq_mode_t mode );
bool string_equal_l( size_t len1,
                     string_t s1,
                     size_t len2,
                     string_t s2,
                     streq_mode_t mode );


/**
 * Flags for the `string_split[_regex]` functions
 * <p>
 *
 * - `STRSPLIT_EXCLUDE_EMPTY`       // = 0x01
 *      - resulting string array doesn't include empty strings ("")
 *
 * - `STRSPLIT_KEEP_DELIM_BEFORE`   // = 0x02
 *      - items include the delimiting strings;
 *        the delim is included at the end of the previous item
 *
 * - `STRSPLIT_KEEP_DELIM_AFTER`    // = 0x04
 *      - items include the delimiting strings;
 *        the delim is included at the start of the next item
 *
 * - `STRSPLIT_STRIP_RESULTS`       // = 0x08
 *      - strips whitespace from either end of each entry
 *        only for string_split() (not regex)
 * </p>
 */
typedef unsigned int strsplit_mode_t;


#define STRSPLIT_EXCLUDE_EMPTY     ( 1 << 0 )
#define STRSPLIT_KEEP_DELIM_BEFORE ( 1 << 1 )
#define STRSPLIT_KEEP_DELIM_AFTER  ( 1 << 2 )

/// Only for `string_split()` (not regex)
#define STRSPLIT_STRIP_RESULTS ( 1 << 3 )


/**
 * Splits `str` at places matching `split_tok`
 * <p>
 * Substring must match perfectly
 * </p>
 *
 * @param str_arr_cont  container for the string_array for the result; the resulting
 *                      string array is allocated in this function
 * @param string        string to be split
 * @param split_tok     boundary
 * @param mode          see strsplit_mode_t
 * @return number of strings in the result,
 *         `RV_ERROR` on allocation fail,
 *         `RV_EXCEPTION` if split_tok is invalid
 */
ssize_t string_split( str_t **str_arr_cont,
                      string_t string,
                      string_t split_tok,
                      strsplit_mode_t mode );
/**
 * Splits `str` at places matching `regex`<br>
 * Substring must match `regex`<br>
 *
 * @param str_arr_cont  container for the string_array for the result; the resulting
 *                      string array is allocated in this function
 * @param string        string to be split
 * @param regexp        boundary; compiled regex
 * @param mode          see strsplit_mode_t
 * @return number of strings in the result,
 *         `RV_ERROR` on allocation fail or regex error,
 *         `RV_EXCEPTION` if a parameter is invalid
 */
ssize_t string_split_regex( str_t **str_arr_cont,
                            string_t string,
                            const regex_t *__restrict regexp,
                            strsplit_mode_t mode );

/**
 * `free()`s all memory allocated by `string_split()` or `string_split_regex()`.
 *
 * Also sets str_arr to `NULL`
 *
 * @param size number of strings in `*str_arr_cont`
 * @param str_arr_cont same variable as the one passed to `string_split()`
 */
void string_split_destroy( size_t size, str_t **str_arr_cont );


/**
 * Joins a string array into one long string.
 *
 * @param len length of `strarr`
 * @param strarr array of string (not modified in this function)
 * @param joiner string to be put between each member of `strarr`
 * @return new heap-allocated string to be freed with `free(3)`
 */
UseResult str_t string_join( size_t len, const string_t strarr[ len ], string_t joiner );

/// String array being passed to `string_join` may be typecast to this
typedef const string_t *strjoin_strarr_t;


/* ==== Mathematical stuff ==== */

UseResult char *add_uint_strings( const char *, const char * );
UseResult char *mul_uint_strings( const char *, const char * );


void string_strip_lead_zeroes( str_t );

UseResult str_t hex_to_decimal( string_t );


#endif //CLIBS_STRING_UTILS_H
