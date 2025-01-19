//
// Created by MacBook on 25.12.2024.
//

#ifndef CLIBS_ARRAY_PRINTF_H
#define CLIBS_ARRAY_PRINTF_H

#include "Structs/dynstring.h" /* array_sprintf */

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
        for ( size_t print_array_idx__ = 0; print_array_idx__ < ( ARRLEN );  \
              ++print_array_idx__ )                                          \
        {                                                                    \
            printf( FORMAT_STR, ( ( TYPE * ) ARRAY )[ print_array_idx__ ] ); \
            if ( print_array_idx__ != ( ARRLEN ) -1 )                        \
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

/// Creates a new string of the array contents. Old contents of STRINGVAR are overwritten
#define array_sprintf_d( STRINGVAR, ARRAY, ARRLEN, FMTSTR, DELIM )                      \
    do                                                                                  \
    {                                                                                   \
        ( STRINGVAR )        = NULL;                                                    \
        DynamicString dynstr = dynstr_init_as( "[ " );                                  \
        if ( dynstr == NULL )                                                           \
            break;                                                                      \
        for ( size_t print_array_idx__ = 0; print_array_idx__ < ( ARRLEN );             \
              ++print_array_idx__ )                                                     \
        {                                                                               \
            on_fail( dynstr_appendf( dynstr, FMTSTR, ( ARRAY )[ print_array_idx__ ] ) ) \
            {                                                                           \
                dynstr_destroy( dynstr );                                               \
                break;                                                                  \
            }                                                                           \
            if ( print_array_idx__ != ( ARRLEN ) -1 )                                   \
            {                                                                           \
                on_fail( dynstr_append( dynstr, DELIM ) )                               \
                {                                                                       \
                    dynstr_destroy( dynstr );                                           \
                    break;                                                              \
                }                                                                       \
            }                                                                           \
        }                                                                               \
        on_fail( dynstr_append( dynstr, " ]" ) )                                        \
        {                                                                               \
            dynstr_destroy( dynstr );                                                   \
            break;                                                                      \
        }                                                                               \
        ( STRINGVAR ) = dynstr_data_copy( dynstr );                                     \
        dynstr_destroy( dynstr );                                                       \
    }                                                                                   \
    while ( 0 )

#define array_snprintf( STRING, STRCAP, ARRAY, ARRLEN, FMTSTR ) \
    array_sprintf_d( STRING, STRCAP, ARRAY, ARRLEN, FMTSTR, ", " )


#endif //CLIBS_ARRAY_PRINTF_H
