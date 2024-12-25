//
// Created by MacBook on 25.12.2024.
//

#ifndef CLIBS_ARRAY_PRINTF_H
#define CLIBS_ARRAY_PRINTF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ––––– Array Print ––––– */
#define ARRAY_PRINT_STRING_FMTSTR "\\s"

/**
 * Prints the array of a set length and type to a specified file descriptor.\n
 * \n
 * Each item is printed according to the ‹format_str› (as in printf)\n
 * Array starts with '[' and ends with ']\\n'\n
 * Items in array are separated by ‹delim›\n
 * \n
 * If the format string contains the sub-string defined in ARRAY_PRINT_STRING_FMTSTR,
 * array_printf() prints the whole string without any delimiters, also formatted
 * according to the format
 * string.\n
 */
#define array_printf_d( array, len, type, format_str, delim )                      \
    do                                                                             \
    {                                                                              \
        const char *sub_fmt_str = strstr( format_str, ARRAY_PRINT_STRING_FMTSTR ); \
        if ( sub_fmt_str != NULL )                                                 \
        {                                                                          \
            char *print_array_format__ = strdup( format_str );                     \
                                                                                   \
            print_array_format__[ sub_fmt_str - format_str ]     = '%';            \
            print_array_format__[ sub_fmt_str - format_str + 1 ] = 's';            \
            printf( print_array_format__, ( char * ) array );                      \
            free( print_array_format__ );                                          \
        }                                                                          \
        else                                                                       \
        {                                                                          \
            printf( "[ " );                                                        \
            for ( size_t print_array_idx__ = 0; print_array_idx__ < len;           \
                  ++print_array_idx__ )                                            \
            {                                                                      \
                const char *print_array_format__ = format_str;                     \
                printf( print_array_format__,                                      \
                        ( ( type * ) array )[ print_array_idx__ ] );               \
                if ( print_array_idx__ != len - 1 )                                \
                    printf( "%s", delim );                                         \
            }                                                                      \
            printf( " ]\n" );                                                      \
        }                                                                          \
    }                                                                              \
    while ( 0 )

/**
 * Prints the array of a set length and type.\n
 * <p>
 * Items are separated by ", "\n
 *
 * @see \code array_printf_d
*/
#define array_printf( array, len, type, format ) \
    array_printf_d( array, len, type, format, ", " )


#endif //CLIBS_ARRAY_PRINTF_H
