//
// Created by MacBook on 28.10.2024.
//

#ifndef CLIBS_STRING_UTILS_H
#define CLIBS_STRING_UTILS_H

#include <regex.h>     /* regex_t */
#include <stdbool.h>   /* bool */
#include <sys/types.h> /* ssize_t */

#ifndef __result_use_check
#define __result_use_check __attribute__( ( warn_unused_result ) )
#endif //__result_use_check


typedef const char *string_t;
typedef char *str_t;


#ifndef ESCAPED_CHARS
#define ESCAPED_CHARS "\n\t\r\f\v"
#endif //ESCAPED_CHARS

str_t string_stripped( string_t ) __result_use_check;
void string_strip( str_t );

str_t string_escaped( string_t ) __result_use_check;

str_t string_reversed( string_t ) __result_use_check;
void string_reverse( str_t );

/**
 * Splits ‹str› at places matching ‹split_tok›<br>
 * Result may or may not contain empty strings depending on ‹excl_empty›<br>
 * Substring must match perfectly<br>
 *
 * @param str_arr_cont  container for the string_array for the result; the resulting
 *                      string array is allocated in this function
 * @param string        string to be split
 * @param split_tok     boundary
 * @param excl_empty    if true, the resulting string array will not contain empty
 *                      strings ("")
 * @return number of strings in the result,
 *         -1 on allocation fail,
 *         -2 if split_tok is invalid
 */
ssize_t string_split( str_t **str_arr_cont,
                      string_t string,
                      string_t split_tok,
                      bool excl_empty );
/**
 * Splits ‹str› at places matching ‹regex›<br>
 * Result may or may not contain empty strings depending on ‹excl_empty›<br>
 * Substring must match ‹regex› perfectly<br>
 *
 * @param str_arr_cont  container for the string_array for the result; the resulting
 *                      string array is allocated in this function
 * @param string        string to be split
 * @param regexp        boundary; compiled regex
 * @param excl_empty    if true, the resulting string array will not contain empty
 *                      strings ("")
 * @return number of strings in the result,
 *         -1 on allocation fail or regex error,
 *         -2 if split_tok is invalid
 */
ssize_t string_split_regex( str_t **str_arr_cont,
                            string_t string,
                            regex_t *restrict regexp,
                            bool excl_empty );


string_t get_file_name( string_t full_path );

#endif //CLIBS_STRING_UTILS_H
