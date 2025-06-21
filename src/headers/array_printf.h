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

#define array_printf_sde( ARRAY, ARRLEN, TYPE, FORMAT_STR, START_STR, DELIM, END_STR ) \
    do                                                                                 \
    {                                                                                  \
        printf( START_STR );                                                           \
        for ( size_t print_array_idx__ = 0; print_array_idx__ < ( ARRLEN );            \
              ++print_array_idx__ )                                                    \
        {                                                                              \
            printf( FORMAT_STR, ( ( TYPE * ) ARRAY )[ print_array_idx__ ] );           \
            if ( print_array_idx__ != ( ARRLEN ) - 1 )                                 \
                printf( "%s", DELIM );                                                 \
        }                                                                              \
        printf( END_STR );                                                             \
    }                                                                                  \
    while ( 0 )


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
#define array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, DELIM ) \
    array_printf_sde( ARRAY, ARRLEN, TYPE, FORMAT_STR, "[ ", DELIM, " ]\n" )

/**
 * Prints the array of a set length and type.
 * <p>
 * Items are separated by ", "
 * </p>
 * <p>
 * requires @code #include "Structs/dynstring.h"@endcode
 * </p>
 *
 * @see \code array_printf_d\endcode
*/
#define array_printf( ARRAY, ARRLEN, TYPE, FORMAT_STR ) \
    array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, ", " )

/**
 * Creates a new string of the array contents. Old contents of STRINGVAR are overwritten
 * <p>
 * requires @code #include "Structs/dynstring.h"@endcode
 */
#define array_sprintf_d( STRINGVAR, ARRAY, ARRLEN, TYPE, FMTSTR, DELIM )           \
    do                                                                             \
    {                                                                              \
        ( STRINGVAR )                 = NULL;                                      \
        struct dynamic_string *dynstr = dynstr_init_as( "[ " );                    \
        if ( dynstr == NULL )                                                      \
            break;                                                                 \
        for ( size_t print_array_idx__ = 0; print_array_idx__ < ( ARRLEN );        \
              ++print_array_idx__ )                                                \
        {                                                                          \
            if ( dynstr_appendf( dynstr, FMTSTR,                                   \
                                 ( ( TYPE * ) ARRAY )[ print_array_idx__ ] ) < 0 ) \
            {                                                                      \
                dynstr_destroy( dynstr );                                          \
                break;                                                             \
            }                                                                      \
            if ( print_array_idx__ != ( ARRLEN ) - 1 )                             \
            {                                                                      \
                if ( dynstr_append( dynstr, DELIM ) < 0 )                          \
                {                                                                  \
                    dynstr_destroy( dynstr );                                      \
                    break;                                                         \
                }                                                                  \
            }                                                                      \
        }                                                                          \
        if ( dynstr_append( dynstr, " ]" ) < 0 )                                   \
        {                                                                          \
            dynstr_destroy( dynstr );                                              \
            break;                                                                 \
        }                                                                          \
        ( STRINGVAR ) = dynstr_data_copy( dynstr );                                \
        dynstr_destroy( dynstr );                                                  \
    }                                                                              \
    while ( 0 )

#define array_sprintf( STRING, ARRAY, ARRLEN, TYPE, FMTSTR ) \
    array_sprintf_d( STRING, ARRAY, ARRLEN, TYPE, FMTSTR, ", " )


#endif //CLIBS_ARRAY_PRINTF_H
