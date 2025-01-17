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
 * Prints the array of a set length and type to stdout.\n
 * <p>
 * Each item is printed according to the ‹FORMAT_STR› (as in printf)\n
 * Array starts with '[' and ends with ']\\n'\n
 * Items in array are separated by ‹DELIM› (string)
 * <p>
 * @example
 * @code
 * #define array_printf( ARRAY, ARRLEN, TYPE, FORMAT_STR ) \
 *     array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, ", " )
 * @endcode
 */
#define array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, DELIM )             \
    do                                                                       \
    {                                                                        \
        printf( "[ " );                                                      \
        for ( size_t print_array_idx__ = 0; print_array_idx__ < ARRLEN;      \
              ++print_array_idx__ )                                          \
        {                                                                    \
            printf( FORMAT_STR, ( ( TYPE * ) ARRAY )[ print_array_idx__ ] ); \
            if ( print_array_idx__ != ARRLEN - 1 )                           \
                printf( "%s", DELIM );                                       \
        }                                                                    \
        printf( " ]\n" );                                                    \
    }                                                                        \
    while ( 0 )

/**
 * Prints the array of a set length and type.\n
 * <p>
 * Items are separated by ", "\n
 *
 * @see \code array_printf_d
*/
#define array_printf( ARRAY, ARRLEN, TYPE, FORMAT_STR ) \
    array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, ", " )


#endif //CLIBS_ARRAY_PRINTF_H
