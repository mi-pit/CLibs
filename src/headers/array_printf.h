/*
 * Print an array in a formatted way.
 *
 *
 * Created by MacBook on 25.12.2024.
 */

#ifndef CLIBS_ARRAY_PRINTF_H
#define CLIBS_ARRAY_PRINTF_H

#include <stdio.h> /* fprintf */

/* TODO:
 *  array_fprintf */


#define ARRAY_PRINT_DEFAULT_STARTSTR "[ "
#define ARRAY_PRINT_DEFAULT_ENDSTR   " ]"
#define ARRAY_PRINT_DEFAULT_DELIM    ", "


#define array_fprintf_sde( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR, START_STR, \
                           DELIM, END_STR )                                        \
    do                                                                             \
    {                                                                              \
        fprintf( FILESTREAM, "%s", START_STR );                                    \
        for ( size_t print_array_idx__ = 0; print_array_idx__ < ( ARRLEN );        \
              ++print_array_idx__ )                                                \
        {                                                                          \
            fprintf( FILESTREAM, FORMAT_STR,                                       \
                     ( ( TYPE * ) ARRAY )[ print_array_idx__ ] );                  \
            if ( print_array_idx__ != ( ARRLEN ) - 1 )                             \
                fprintf( FILESTREAM, "%s", DELIM );                                \
        }                                                                          \
        fprintf( FILESTREAM, "%s", END_STR );                                      \
    }                                                                              \
    while ( 0 )

#define array_fprintf_d( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR, DELIM ) \
    array_fprintf_sde( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR,           \
                       ARRAY_PRINT_DEFAULT_STARTSTR, DELIM,                   \
                       ARRAY_PRINT_DEFAULT_ENDSTR "\n" )

#define array_fprintf( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR ) \
    array_fprintf_d( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR,    \
                     ARRAY_PRINT_DEFAULT_DELIM )


#define array_printf_sde( ARRAY, ARRLEN, TYPE, FORMAT_STR, START_STR, DELIM, END_STR ) \
    array_fprintf_sde( stdout, ARRAY, ARRLEN, TYPE, FORMAT_STR, START_STR, DELIM,      \
                       END_STR )


/**
 * Prints the array of a set length and type to stdout.
 * <p>
 * Each item is printed according to the ‹FORMAT_STR› (as in printf)\n
 * Array starts with '[' and ends with ']\\n'\n
 * Items in array are separated by ‹DELIM› (string)
 * </p>
 * @example
 * @code
   #define array_printf( ARRAY, ARRLEN, TYPE, FORMAT_STR ) \
       array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, ", " )
 * @endcode
 */
#define array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, DELIM )                     \
    array_printf_sde( ARRAY, ARRLEN, TYPE, FORMAT_STR, ARRAY_PRINT_DEFAULT_STARTSTR, \
                      DELIM, ARRAY_PRINT_DEFAULT_ENDSTR "\n" )

/**
 * Prints the array of a set length and type.
 * <p>
 * Items are separated by ", "
 * </p>
 * <p>
 * requires @code #include "structs/dynstring.h"@endcode
 * </p>
 *
 * @see \code array_printf_d\endcode
*/
#define array_printf( ARRAY, ARRLEN, TYPE, FORMAT_STR ) \
    array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, ARRAY_PRINT_DEFAULT_DELIM )

#if defined( CLIBS_DYNSTRING_H )
/**
 * If allocation fails at any point during the main body of this macro,
 * this results in STRINGVAR being NULL
 */
#define array_sprintf_sde( STRINGVAR, ARRAY, ARRLEN, TYPE, FMTSTR, STARTSTR, DELIM, \
                           ENDSTR )                                                 \
    do                                                                              \
    {                                                                               \
        ( STRINGVAR )                 = NULL;                                       \
        struct dynamic_string *dynstr = dynstr_init_as( STARTSTR );                 \
        if ( dynstr == NULL )                                                       \
            break;                                                                  \
        for ( size_t print_array_idx__ = 0; print_array_idx__ < ( ARRLEN );         \
              ++print_array_idx__ )                                                 \
        {                                                                           \
            if ( dynstr_appendf( dynstr, FMTSTR,                                    \
                                 ( ( TYPE * ) ARRAY )[ print_array_idx__ ] )        \
                 < 0 )                                                              \
            {                                                                       \
                dynstr_destroy( dynstr );                                           \
                break;                                                              \
            }                                                                       \
            if ( print_array_idx__ != ( ARRLEN ) - 1 )                              \
            {                                                                       \
                if ( dynstr_append( dynstr, DELIM ) < 0 )                           \
                {                                                                   \
                    dynstr_destroy( dynstr );                                       \
                    break;                                                          \
                }                                                                   \
            }                                                                       \
        }                                                                           \
        if ( dynstr_append( dynstr, ENDSTR ) < 0 )                                  \
        {                                                                           \
            dynstr_destroy( dynstr );                                               \
            break;                                                                  \
        }                                                                           \
        ( STRINGVAR ) = dynstr_data_copy( dynstr );                                 \
        dynstr_destroy( dynstr );                                                   \
    }                                                                               \
    while ( 0 )


/**
 * Creates a new string of the array contents. Old contents of STRINGVAR are overwritten
 * <p>
 * requires @code #include "Structs/dynstring.h"@endcode
 */
#define array_sprintf_d( STRINGVAR, ARRAY, ARRLEN, TYPE, FMTSTR, DELIM ) \
    array_sprintf_sde( STRINGVAR, ARRAY, ARRLEN, TYPE, FMTSTR,           \
                       ARRAY_PRINT_DEFAULT_STARTSTR, DELIM, ARRAY_PRINT_DEFAULT_ENDSTR )

#define array_sprintf( STRING, ARRAY, ARRLEN, TYPE, FMTSTR ) \
    array_sprintf_d( STRING, ARRAY, ARRLEN, TYPE, FMTSTR, ARRAY_PRINT_DEFAULT_DELIM )
#endif // CLIBS_DYNSTRING_H


#endif //CLIBS_ARRAY_PRINTF_H
