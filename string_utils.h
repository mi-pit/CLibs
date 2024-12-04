//
// Created by MacBook on 28.10.2024.
//

#ifndef CLIBS_STRING_UTILS_H
#define CLIBS_STRING_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>


typedef const char *string_t;
typedef char *str_t;


#define ESCAPED_CHARS "\n\t\r\f\v\0"


str_t string_stripped( string_t );
void string_strip( char *s );

str_t string_escaped( string_t );
void string_escape( char **strp, size_t *capp );

str_t string_reversed( string_t );
void string_reverse( char *s );

/**
 * Result may contain empty strings
 *
 * @param string_array_container address of the string_array for the result
 * @param str haystack
 * @param split_tok needle
 * @return number of strings in the result,
 *         -1 on allocation fail,
 *         -2 if split_tok is invalid
 */
ssize_t string_split( str_t **str_arr_cont,
                      string_t str,
                      string_t split_tok,
                      bool excl_empty );


string_t get_program_name( string_t argv0 );

#endif //CLIBS_STRING_UTILS_H
