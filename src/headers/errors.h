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
#include "unreachable.h"     /* UNREACHABLE */

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
LibraryDefined inline const char *rv_to_string( const int rv )
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
WarnUniversal( bool PrintProgName, const char *FileName, const char *FunctionName,
               int LineNumber, int err_no, ptrdiff_t return_value,
               const char *__restrict format, ... );

LibraryDefined Cold ptrdiff_t VWarnUniversal( bool PrintProgName, const char *FileName,
                                              const char *FunctionName, int LineNumber,
                                              int err_no, ptrdiff_t return_value,
                                              const char *__restrict format,
                                              va_list va_ls );

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
 * #define fwarn( ... )  ( void ) WarnUniversal( true, NULL, __func__, -1, errno, -1, __VA_ARGS__ )
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
ptrdiff_t WarnUniversal( const bool PrintProgName, const char *FileName,
                         const char *FunctionName, const int LineNumber, const int err_no,
                         const ptrdiff_t return_value, const char *__restrict format,
                         ... )
{
    va_list va;
    va_start( va, format );
    ( void ) VWarnUniversal( PrintProgName, FileName, FunctionName, LineNumber, err_no,
                             return_value, format, va );
    va_end( va );

    return return_value;
}


ptrdiff_t VWarnUniversal( bool PrintProgName, const char *FileName,
                          const char *FunctionName, int LineNumber, int err_no,
                          ptrdiff_t return_value, const char *format, va_list va_ls )
{
#ifndef SUPPRESS_WARNINGS
    ( void ) SetTerminalColor( stderr, COLOR_WARNING );

#if defined( __APPLE__ ) && defined( CLIBS_USE_EMOJIS )
#define FILENAME_PRETTIFIER     "📄"
#define FUNCTIONNAME_PRETTIFIER "⚙️"
#else
#define FILENAME_PRETTIFIER     ""
#define FUNCTIONNAME_PRETTIFIER ""
#endif // __APPLE__

    if ( PrintProgName )
        ( void ) fprintf( stderr, "%s: ", get_prog_name() );
    if ( FileName != NULL )
        ( void ) fprintf( stderr, FILENAME_PRETTIFIER "%s: ", FileName );
    if ( FunctionName != NULL )
        ( void ) fprintf( stderr, FUNCTIONNAME_PRETTIFIER "%s: ", FunctionName );
    if ( LineNumber > 0 )
        ( void ) fprintf( stderr, "@ %i: ", LineNumber );

    ( void ) vfprintf( stderr, format, va_ls );

    if ( err_no >= 0 )
        ( void ) fprintf( stderr, ": %s", strerror( err_no ) );

    ( void ) SetTerminalColor( stderr, COLOR_DEFAULT );

    ( void ) fprintf( stderr, "\n" );

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
#define f_stack_trace( RETVAL )                                                    \
    ( ( void ) WarnUniversal( false, NULL, NULL, -1, -1, 0, "\tin %s", __func__ ), \
      RETVAL )

/**
 * Like f_stack_trace, just with `__FILE_NAME__` and `__LINE__`
 * @see `f_stack_trace()`
 */
#define ffl_stack_trace( RETVAL )                                               \
    ( ( void ) WarnUniversal( false, NULL, NULL, -1, -1, 0, "\tin %s: %s @ %d", \
                              __FILE_NAME__, __func__, __LINE__ ),              \
      RETVAL )


/** Warns like `warn()`, but in color */
#define xwarn( ... ) \
    ( ( void ) WarnUniversal( true, NULL, NULL, -1, errno, -1, __VA_ARGS__ ) )
#define xwarnx( ... ) \
    ( ( void ) WarnUniversal( true, NULL, NULL, -1, 0, -1, __VA_ARGS__ ) )
/** Warns like `warn()` and evaluates to `RETVAL` */
#define xwarn_ret( RETVAL, ... ) \
    ( ( void ) WarnUniversal( true, NULL, NULL, -1, errno, 0, __VA_ARGS__ ), RETVAL )
#define xwarnx_ret( RETVAL, ... ) \
    ( ( void ) WarnUniversal( true, NULL, NULL, -1, -1, 0, __VA_ARGS__ ), RETVAL )

/** `warn()` with function name at the start */
#define fwarn( ... ) \
    ( ( void ) WarnUniversal( true, NULL, __func__, -1, errno, -1, __VA_ARGS__ ) )
#define fwarnx( ... ) \
    ( ( void ) WarnUniversal( true, NULL, __func__, -1, -1, -1, __VA_ARGS__ ) )
#define fwarn_ret( RETVAL, ... ) \
    ( ( void ) WarnUniversal( true, NULL, __func__, -1, errno, 0, __VA_ARGS__ ), RETVAL )
#define fwarnx_ret( RETVAL, ... ) \
    ( ( void ) WarnUniversal( true, NULL, __func__, -1, -1, 0, __VA_ARGS__ ), RETVAL )

#define fflwarn( ... )                                                            \
    ( ( void ) WarnUniversal( true, __FILE_NAME__, __func__, __LINE__, errno, -1, \
                              __VA_ARGS__ ) )
#define fflwarnx( ... )                                                        \
    ( ( void ) WarnUniversal( true, __FILE_NAME__, __func__, __LINE__, -1, -1, \
                              __VA_ARGS__ ) )
#define fflwarn_ret( RETVAL, ... )                                               \
    ( ( void ) WarnUniversal( true, __FILE_NAME__, __func__, __LINE__, errno, 0, \
                              __VA_ARGS__ ),                                     \
      RETVAL )
#define fflwarnx_ret( RETVAL, ... )                                           \
    ( ( void ) WarnUniversal( true, __FILE_NAME__, __func__, __LINE__, -1, 0, \
                              __VA_ARGS__ ),                                  \
      RETVAL )


LibraryDefined NoReturn void VErrUniversal( int exit_code,
                                            bool print_progname,
                                            const char *file,
                                            const char *func,
                                            int line,
                                            const int err_no,
                                            const char *__restrict fmt,
                                            va_list va );

void VErrUniversal( int exit_code,
                    bool print_progname,
                    const char *file,
                    const char *func,
                    int line,
                    const int err_no,
                    const char *__restrict fmt,
                    va_list va )
{
    ( void ) VWarnUniversal( print_progname, file, func, line, err_no, 0, fmt, va );
    exit( exit_code );
}

LibraryDefined PrintfLike( 7, 8 ) NoReturn void ErrUniversal( const int exit_code,
                                                              const bool print_progname,
                                                              const char *const file,
                                                              const char *const func,
                                                              const int line,
                                                              const int err_no,
                                                              const char *__restrict fmt,
                                                              ... )
{
    va_list va;
    va_start( va, fmt );
    VErrUniversal( exit_code, print_progname, file, func, line, err_no, fmt, va );

    UNREACHABLE();
}

#define ferr( EXIT_VALUE, ... ) \
    ( ErrUniversal( EXIT_VALUE, true, NULL, __func__, -1, errno, __VA_ARGS__ ) )
#define ferrx( EXIT_VALUE, ... ) \
    ( ErrUniversal( EXIT_VALUE, true, NULL, __func__, -1, -1, __VA_ARGS__ ) )

#define ferr_e( EXPR_TYPE, EXIT_VALUE, ... )                                    \
    ( ErrUniversal( EXIT_VALUE, true, NULL, __func__, -1, errno, __VA_ARGS__ ), \
      ( EXPR_TYPE ) 0 )
#define ferrx_e( EXPR_TYPE, EXIT_VALUE, ... )                                \
    ( ErrUniversal( EXIT_VALUE, true, NULL, __func__, -1, -1, __VA_ARGS__ ), \
      ( EXPR_TYPE ) 0 )


#define fflerr( EXIT_VALUE, ... )                                               \
    ( ErrUniversal( EXIT_VALUE, true, __FILE_NAME__, __func__, __LINE__, errno, \
                    __VA_ARGS__ ) )
#define fflerrx( EXIT_VALUE, ... )                                           \
    ( ErrUniversal( EXIT_VALUE, true, __FILE_NAME__, __func__, __LINE__, -1, \
                    __VA_ARGS__ ) )

#define fflerr_e( EXPR_TYPE, EXIT_VALUE, ... )                                  \
    ( ErrUniversal( EXIT_VALUE, true, __FILE_NAME__, __func__, __LINE__, errno, \
                    __VA_ARGS__ ),                                              \
      ( EXPR_TYPE ) 0 )
#define fflerrx_e( EXPR_TYPE, EXIT_VALUE, ... )                              \
    ( ErrUniversal( EXIT_VALUE, true, __FILE_NAME__, __func__, __LINE__, -1, \
                    __VA_ARGS__ ),                                           \
      ( EXPR_TYPE ) 0 )


#endif //CLIBS_ERRORS_H
