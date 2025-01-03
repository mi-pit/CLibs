//
// Created by MacBook on 28.10.2024.
//

#ifndef CLIBS_STRING_UTILS_H
#define CLIBS_STRING_UTILS_H

#include <regex.h>     /* regex_t */
#include <stdbool.h>   /* bool */
#include <string.h>    /* import */
#include <sys/types.h> /* ssize_t */

#ifndef __result_use_check
#define __result_use_check __attribute__( ( warn_unused_result ) )
#endif //__result_use_check


typedef const char *string_t;
typedef char *str_t;


/**
 * Heap-allocates a new string with all whitespace (as defined in isspace(3)) from either end stripped.
 * @return  New string – copy of the original with no whitespace at either end.
 *          Pointer should be freed with free(3).
 */
str_t string_stripped( string_t ) __result_use_check;
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
str_t string_escaped( string_t ) __result_use_check;

/**
 * Creates a new string with the same characters as the supplied one, reversed.
 *
 * @return New reversed string. Pointer should be freed with free(3).
 */
str_t string_reversed( string_t ) __result_use_check;
/**
 * Reverses the string.
 * This is done in place.
 */
void string_reverse( str_t );


void string_to_upper( str_t );
str_t string_as_upper( string_t ) __result_use_check;

void string_to_lower( str_t );
str_t string_as_lower( string_t ) __result_use_check;


#define STRSPLIT_EXCLUDE_EMPTY   0x01
#define STRSPLIT_KEEP_DELIM_PRE  0x02
#define STRSPLIT_KEEP_DELIM_POST 0x04

/**
 * Flags for the string_split[_regex] functions
 * <p>
 * @code
 * STRSPLIT_EXCLUDE_EMPTY   // = 0x01
 *     - resulting string array doesn't include empty strings ("")
 *
 * STRSPLIT_KEEP_DELIM_PRE  // = 0x02
 *     - items include the delimiting strings;
 *       the delim is included at the end of the previous item
 *
 * STRSPLIT_KEEP_DELIM_POST // = 0x04
 *     - items include the delimiting strings;
 *       the delim is included at the start of the next item
 * @endcode
 */
typedef int strsplit_mode_t;

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
