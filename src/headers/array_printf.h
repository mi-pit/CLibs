/*
 * Print an array in a formatted way.
 *
 *
 * Created by MacBook on 25.12.2024.
 */

#ifndef CLIBS_ARRAY_PRINTF_H
#define CLIBS_ARRAY_PRINTF_H

#include <stdio.h> /* fprintf */


#define ARRAY_PRINT_DEFAULT_STARTSTR "[ "
#define ARRAY_PRINT_DEFAULT_ENDSTR   " ]"
#define ARRAY_PRINT_DEFAULT_DELIM    ", "


/**
 * Prints each element of an array according to a format string as if by `printf`.
 * <p>
 * Every other `array_*printf*` is a specialization of this one.
 * </p>
 *
 * @param FILESTREAM    File to print to
 * @param ARRAY         array of type `TYPE` and length `ARRLEN`
 * @param ARRLEN        length of `ARRAY`
 * @param TYPE          type of member of `ARRAY`
 * @param FORMAT_STR    format string for printing elements
 * @param START_STR     printed once at the start
 * @param DELIM         printed between each element
 * @param END_STR       printed once at the end
 */
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

/// @see `array_fprintf_sde()`
#define array_fprintf_d( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR, DELIM ) \
    array_fprintf_sde( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR,           \
                       ARRAY_PRINT_DEFAULT_STARTSTR, DELIM,                   \
                       ARRAY_PRINT_DEFAULT_ENDSTR "\n" )

/// @see `array_fprintf_sde()`
#define array_fprintf( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR ) \
    array_fprintf_d( FILESTREAM, ARRAY, ARRLEN, TYPE, FORMAT_STR,    \
                     ARRAY_PRINT_DEFAULT_DELIM )


/// @see `array_fprintf_sde()`
#define array_printf_sde( ARRAY, ARRLEN, TYPE, FORMAT_STR, START_STR, DELIM, END_STR ) \
    array_fprintf_sde( stdout, ARRAY, ARRLEN, TYPE, FORMAT_STR, START_STR, DELIM,      \
                       END_STR )

/// @see `array_fprintf_sde()`
#define array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, DELIM )                     \
    array_printf_sde( ARRAY, ARRLEN, TYPE, FORMAT_STR, ARRAY_PRINT_DEFAULT_STARTSTR, \
                      DELIM, ARRAY_PRINT_DEFAULT_ENDSTR "\n" )

/// @see `array_fprintf_sde()`
#define array_printf( ARRAY, ARRLEN, TYPE, FORMAT_STR ) \
    array_printf_d( ARRAY, ARRLEN, TYPE, FORMAT_STR, ARRAY_PRINT_DEFAULT_DELIM )

#if defined( CLIBS_DYNSTRING_H ) || defined( CLIBS__GENERATE_DOCS )
/**
 * The contents of `STRINGVAR` are overwritten (the pointer itself is).
 *
 * If allocation fails at any point during the main body of this macro,
 * this results in `STRINGVAR` being `NULL`.
 *
 * Requires previous definition of `CLIBS_DYNSTRING_H`,
 * for example by `#include "src/structs/dynstring.h"`
 *
 * @see `array_fprintf_sde()`
 *
 * @param STRINGVAR a local variable into which a pointer to
 * a string (a `char *`) gets saved
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

/// @see `array_sprintf_sde()`
#define array_sprintf_d( STRINGVAR, ARRAY, ARRLEN, TYPE, FMTSTR, DELIM ) \
    array_sprintf_sde( STRINGVAR, ARRAY, ARRLEN, TYPE, FMTSTR,           \
                       ARRAY_PRINT_DEFAULT_STARTSTR, DELIM, ARRAY_PRINT_DEFAULT_ENDSTR )

/// @see `array_sprintf_sde()`
#define array_sprintf( STRING, ARRAY, ARRLEN, TYPE, FMTSTR ) \
    array_sprintf_d( STRING, ARRAY, ARRLEN, TYPE, FMTSTR, ARRAY_PRINT_DEFAULT_DELIM )
#endif // CLIBS_DYNSTRING_H


#endif //CLIBS_ARRAY_PRINTF_H
