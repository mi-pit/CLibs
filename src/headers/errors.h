/*
 * Utility functions for handling errors.
 *
 * Includes (among others):
 *   return values
 *   better warn
 *   stack trace printing
 *
 *
 * Created by MacBook on 30.10.2024.
 */

#ifndef CLIBS_ERRORS_H
#define CLIBS_ERRORS_H

/* for this header */
#include "attributes.h"      /* PrintfLike, LibraryDefined */
#include "filenames.h"       /* get_prog_name(), __FILE_NAME__ */
#include "terminal_colors.h" /* COLORs, PrintInColor */

#include <errno.h>  /* for WarnUniversal + include */
#include <stddef.h> /* ptrdiff_t */
#include <string.h> /* strerror() */

/* for user */
#include <err.h> /* include */


#define ERRNO_OK 0 /* No error */


#define RV_EXCEPTION ( -2 ) /* Non-fatal, recoverable error e.g. OOB index */
#define RV_ERROR     ( -1 ) /* Fatal, un-recoverable error */
#define RV_SUCCESS   0      /* Success */


/**
 * Turns an RV to a string.
 *
 * @param rv `RV_*` from this file
 * @return string literal
 */
LibraryDefined const char *rv_to_string( const int rv )
{
    switch ( rv )
    {
        case RV_SUCCESS:
            return "Success";
        case RV_EXCEPTION:
            return "Exception";
        case RV_ERROR:
            return "Error";

        default:
            return "Unknown error";
    }
}


#define on_fail( func_call )  if ( ( func_call ) != RV_SUCCESS )
#define on_error( func_call ) if ( ( func_call ) == RV_ERROR )

/**
 * Returns the value (of type `int`) of the function call or variable
 * if it doesn't equal `RV_SUCCESS`
 *
 * @see `return_on_error()`
 */
#define return_on_fail( func_call )           \
    do                                        \
    {                                         \
        int func_call_retval = ( func_call ); \
        on_fail ( func_call_retval )          \
        {                                     \
            ( void ) f_stack_trace( NULL );   \
            return func_call_retval;          \
        }                                     \
    }                                         \
    while ( 0 )

/**
 * Returns the value (of type `int`) of the function call or variable
 * if it equals `RV_ERROR`
 *
 * @see `return_on_fail()`
 */
#define return_on_error( func_call )          \
    do                                        \
    {                                         \
        int func_call_retval = ( func_call ); \
        on_error ( func_call_retval )         \
        {                                     \
            ( void ) f_stack_trace( 0 );      \
            return func_call_retval;          \
        }                                     \
    }                                         \
    while ( 0 )

#define goto_on_fail( GOTO_LABEL, func_call ) \
    do                                        \
    {                                         \
        on_fail ( func_call )                 \
            goto GOTO_LABEL;                  \
    }                                         \
    while ( 0 )

#define goto_on_error( GOTO_LABEL, func_call ) \
    do                                         \
    {                                          \
        on_error ( func_call )                 \
            goto GOTO_LABEL;                   \
    }                                          \
    while ( 0 )


#ifndef COLOR_WARNING
#define COLOR_WARNING FOREGROUND_RED
#endif //COLOR_WARNING

LibraryDefined PrintfLike( 7, 8 ) Cold ptrdiff_t
WarnUniversal( bool PrintProgName,
               const char *__restrict FileName,
               const char *__restrict FunctionName,
               int LineNumber,
               int err_no,
               ptrdiff_t return_value,
               const char *__restrict format,
               ... );

/**
 * Similar to `warn()` (especially `warnc`)
 * <p>
 * Prints
 * @code
 * ‹executable name›: ‹FileName›: ‹FunctionName› @ LineNumber: ‹formatted string›[: strerror(err_no)]
 * @endcode
 * </p>
 * <p>
 * If ‹FileName› or ‹FunctionName› is NULL or if ‹LineNumber› or ‹err_no› equals -1,
 * then they are not printed.
 * </p>
 *
 * Example of an actual function-like macro defined in `errors.h`:
 * @code
 * #define fwarn( ... )  ( void ) WarnUniversal( NULL, __func__, -1, errno, -1, __VA_ARGS__ )
 * @endcode
 *
 * @param PrintProgName true if the function should print the executable name
 * @param FileName      name of the current file (__FILE_NAME__)
 * @param FunctionName  name of the function (__func__)
 * @param LineNumber    number of the line to warn about (__LINE__)
 * @param err_no        errno to warn about
 * @param return_value  returned value
 * @param format        format string
 * @param ...           printf-like arguments for ‹format›
 * @return @code return_value@endcode
 *
 * @bug `"%p"` for some reason sometimes throws compiler errors for non `void *` pointers
 */
ptrdiff_t WarnUniversal( bool PrintProgName,
                         const char *__restrict FileName,
                         const char *__restrict FunctionName,
                         int LineNumber,
                         int err_no,
                         ptrdiff_t return_value,
                         const char *__restrict format,
                         ... )
{
#ifndef SUPPRESS_WARNINGS
    SetTerminalColor( stderr, COLOR_WARNING );

    if ( PrintProgName )
        fprintf( stderr, "%s", get_prog_name() );
    if ( FileName != NULL )
        fprintf( stderr, ": %s", FileName );
    if ( FunctionName != NULL )
        fprintf( stderr, ": %s", FunctionName );
    if ( LineNumber > 0 )
        fprintf( stderr, " @ %i", LineNumber );

    if ( PrintProgName || FileName != NULL || FunctionName != NULL || LineNumber > 0 )
        fprintf( stderr, ": " );

    va_list vaList;
    va_start( vaList, format );
    vfprintf( stderr, format, vaList );
    va_end( vaList );

    if ( err_no >= 0 )
        fprintf( stderr, ": %s", strerror( err_no ) );

    SetTerminalColor( stderr, COLOR_DEFAULT );

    fprintf( stderr, "\n" );

#else  // defined( SUPPRESS_WARNINGS )
    ( void ) ( PrintProgName );
    ( void ) ( FileName );
    ( void ) ( FunctionName );
    ( void ) ( LineNumber );
    ( void ) ( err_no );
    ( void ) ( format );
#endif //SUPPRESS_WARNINGS

    return return_value;
}

/**
 * prints
 * @code
 *     in ‹__func__›
 *
 * @endcode
 *
 * Example:
 * \code
 * #include "errors.h"
 *
 * int foo()
 * {
 *     fwarnx( "some error message" );
 *     return RV_EXCEPTION;
 * }
 *
 * int main()
 * {
 *     if ( foo() != RV_SUCCESS )
 *     {
 *         f_stack_trace();
 *         return 1;
 *     }
 * }
 * \endcode
 * <br>
 * prints (to stderr):
 * \code
 * ‹program-name›: foo: some error message
 *     in main
 * \endcode
 */
#define f_stack_trace( RETVAL ) \
    WarnUniversal( false, NULL, NULL, -1, -1, ( ptrdiff_t ) RETVAL, "\tin %s", __func__ )

/**
 * Like f_stack_trace, just with `__FILE_NAME__` and `__LINE__`
 * @see `f_stack_trace()`
 */
#define ffl_stack_trace( RETVAL )        \
    WarnUniversal( false,                \
                   NULL,                 \
                   NULL,                 \
                   -1,                   \
                   -1,                   \
                   ( ptrdiff_t ) RETVAL, \
                   "\tin %s: %s @ %d",   \
                   __FILE_NAME__,        \
                   __func__,             \
                   __LINE__ )


/** Warns like `warn()` and returns RETVAL */
#define warn_ret( RETVAL, ... ) \
    WarnUniversal( true, NULL, NULL, -1, errno, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )
#define warnx_ret( RETVAL, ... ) \
    WarnUniversal( true, NULL, NULL, -1, -1, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )

/** `warn()` with function name at the start */
#define fwarn( ... ) \
    ( void ) WarnUniversal( true, NULL, __func__, -1, errno, -1, __VA_ARGS__ )
#define fwarnx( ... ) \
    ( void ) WarnUniversal( true, NULL, __func__, -1, -1, -1, __VA_ARGS__ )
#define fwarn_ret( RETVAL, ... ) \
    WarnUniversal( true, NULL, __func__, -1, errno, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )
#define fwarnx_ret( RETVAL, ... ) \
    WarnUniversal( true, NULL, __func__, -1, -1, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )

#define fflwarn( ... )      \
    ( void ) WarnUniversal( \
            true, __FILE_NAME__, __func__, __LINE__, errno, -1, __VA_ARGS__ )
#define fflwarnx( ... ) \
    ( void ) WarnUniversal( true, __FILE_NAME__, __func__, __LINE__, -1, -1, __VA_ARGS__ )
#define fflwarn_ret( RETVAL, ... )       \
    WarnUniversal( true,                 \
                   __FILE_NAME__,        \
                   __func__,             \
                   __LINE__,             \
                   errno,                \
                   ( ptrdiff_t ) RETVAL, \
                   __VA_ARGS__ )
#define fflwarnx_ret( RETVAL, ... )      \
    WarnUniversal( true,                 \
                   __FILE_NAME__,        \
                   __func__,             \
                   __LINE__,             \
                   -1,                   \
                   ( ptrdiff_t ) RETVAL, \
                   __VA_ARGS__ )


#endif //CLIBS_ERRORS_H
