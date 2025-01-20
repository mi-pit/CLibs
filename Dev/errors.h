//
// Created by MacBook on 30.10.2024.
//

#ifndef CLIBS_ERRORS_H
#define CLIBS_ERRORS_H

/* for this header */
#include "attributes.h"      /* PrintfLike, LibraryDefined */
#include "terminal_colors.h" /* COLORs, PrintInColor */

#include <errno.h>         /* for WarnUniversal + include */
#include <stddef.h>        /* ptrdiff_t */
#include <string.h>        /* strerror() */
#include <sys/syslimits.h> /* PATH_MAX */

// Get PATH_MAX
#if defined( __APPLE__ )
#include <sys/syslimits.h>
#elif defined( __linux__ )
#include <linux/limits.h>
#else
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif //ndef PATH_MAX
#endif // Get PATH_MAX

#ifndef __FILE_NAME__
#include <string.h>

#define __FILE_NAME__ \
    ( strrchr( __FILE__, '/' ) ? strrchr( __FILE__, '/' ) + 1 : __FILE__ )
#endif //__FILE_NAME__

/* for user */
#include <err.h> /* include */


/* errno value */
#define E_OK 0 /* No error */


#define RV_EXCEPTION ( -2 ) /* Non-fatal, recoverable error e.g. OOB index */
#define RV_ERROR     ( -1 ) /* Fatal, un-recoverable error */
#define RV_SUCCESS   0      /* Success */

/**
 * Returns the value (of type ‹int›) of the function call or variable
 * if it doesn't equal RV_SUCCESS
 *
 * @see @code return_on_error() @endcode
 */
#define return_on_fail( func_call )           \
    do                                        \
    {                                         \
        int func_call_retval = ( func_call ); \
        if ( func_call_retval < RV_SUCCESS )  \
            return func_call_retval;          \
    }                                         \
    while ( 0 )

/**
 * Returns the value (of type ‹int›) of the function call or variable
 * if it equals RV_ERROR
 *
 * @see @code return_on_fail() @endcode
 */
#define return_on_error( func_call )          \
    do                                        \
    {                                         \
        int func_call_retval = ( func_call ); \
        if ( func_call_retval == RV_ERROR )   \
            return func_call_retval;          \
    }                                         \
    while ( 0 )

#define on_fail( func_call )  if ( ( func_call ) != RV_SUCCESS )
#define on_error( func_call ) if ( ( func_call ) == RV_ERROR )


LibraryDefined char prog_name[ PATH_MAX + 1 ] = { 0 };

LibraryDefined const char *get_prog_name( void )
{
    return prog_name;
}


#if defined( __APPLE__ ) /* not sure if it actually works on iPhones :D */

#include "../string_utils.h" /* get_file_name() */

#include <mach-o/dyld.h> /* _NSGetExecutablePath() */

BeforeMain LibraryDefined int set_prog_name( void )
{
    char path[ PATH_MAX ];
    uint32_t size = PATH_MAX;
    if ( _NSGetExecutablePath( path, &size ) == 0 )
    {
        string_t name = get_file_name( path );
        strncpy( prog_name, name, PATH_MAX );
        return RV_SUCCESS;
    }

    return RV_ERROR;
}

#elif ( defined( __APPLE__ ) || defined( __FreeBSD__ ) ) && !defined( _POSIX_C_SOURCE )

#include <stdlib.h>
#define get_prog_name() getprogname()

#elif defined( __linux__ )

#include "../string_utils.h" /* get_file_name() */

#include <unistd.h> /* readlink */

BeforeMain LibraryDefined int set_prog_name( void )
{
    char path[ PATH_MAX ] = { 0 };

    if ( readlink( "/proc/self/exe", path, PATH_MAX ) == RV_ERROR )
    {
        warn( "%s: readlink", __func__ );
        return RV_ERROR;
    }

    strncpy( prog_name, get_file_name( path ), PATH_MAX );
    return RV_SUCCESS;
}

#elif defined( __FILE_NAME__ ) /* this kinda sucks, but hey, what are you gonna do? */
#define get_prog_name() __FILE_NAME__

#else /* just give up at this point */
#define get_prog_name() "current-program"
#endif // get_prog_name()


#ifndef COLOR_WARNING
#define COLOR_WARNING FOREGROUND_RED
#endif //COLOR_WARNING

/**
 * Similar to warn(3) (especially warnc)
 * <p>
 * Prints
 * @code
 * ‹executable name›: ‹FileName›: ‹FunctionName› @ LineNumber: ‹formatted string›[:strerror(err_no)]
 * @endcode
 * </p>
 * <p>
 * If ‹FileName› or ‹FunctionName› is NULL or if ‹LineNumber› or ‹err_no› equals -1,
 * then they are not printed.
 * </p>
 *
 * @example actual function-like macro defined in errors.h:
 * \code
 * #define fwarn( ... )  ( void ) WarnUniversal( NULL, __func__, -1, errno, -1, __VA_ARGS__ )
 * \endcode
 *
 * @param FileName      name of the current file (__FILE_NAME__)
 * @param FunctionName  name of the function (__func__)
 * @param LineNumber    number of the line to warn about (__LINE__)
 * @param err_no        errno to warn about
 * @param return_value  returned value
 * @param format        format string
 * @param ...           printf-like arguments for ‹format›
 * @return @code return_value @endcode
 * @bugs %p for some reason sometimes throws compiler errors for non `void *` pointers
 */
LibraryDefined PrintfLike( 6, 7 ) Cold ptrdiff_t
        WarnUniversal( const char *__restrict FileName,
                       const char *__restrict FunctionName,
                       int LineNumber,
                       int err_no,
                       ptrdiff_t return_value,
                       const char *__restrict format,
                       ... )
{
#ifndef SUPPRESS_WARNINGS
    PrintInColor( stderr, COLOR_WARNING, "%s", get_prog_name() );
    if ( FileName != NULL )
        PrintInColor( stderr, COLOR_WARNING, ": %s", FileName );
    if ( FunctionName != NULL )
        PrintInColor( stderr, COLOR_WARNING, ": %s", FunctionName );
    if ( LineNumber > 0 )
        PrintInColor( stderr, COLOR_WARNING, " @ %i", LineNumber );
    PrintInColor( stderr, COLOR_WARNING, ": " );

    va_list vaList;
    va_start( vaList, format );
    VPrintInColor( stderr, COLOR_WARNING, format, vaList );
    va_end( vaList );

    if ( err_no >= 0 )
        PrintInColor( stderr, COLOR_WARNING, ": %s", strerror( err_no ) );

    PrintInColor( stderr, COLOR_WARNING, "\n" );
#else
    UNUSED( FileName );
    UNUSED( FunctionName );
    UNUSED( LineNumber );
    UNUSED( err_no );
    UNUSED( format );
#endif //SUPPRESS_WARNINGS

    return return_value;
}

/**
 * prints
 * @code
 *     in ‹__func__›
 *
 * @endcode
 * @example
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
#define f_stack_trace() PrintInColor( stderr, COLOR_WARNING, "\tin %s\n", __func__ )

/**
 * Like f_stack_trace, just with __FILE_NAME__ and __LINE__
 * @see \code f_stack_trace()
 */
#define ffl_stack_trace()               \
    PrintInColor( stderr,               \
                  COLOR_WARNING,        \
                  "\tin %s: %s @ %d\n", \
                  __FILE_NAME__,        \
                  __func__,             \
                  __LINE__ )


/** Warns like warn(3) and returns RETVAL */
#define warn_ret( RETVAL, ... ) \
    WarnUniversal( NULL, NULL, -1, errno, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )
#define warnx_ret( RETVAL, ... ) \
    WarnUniversal( NULL, NULL, -1, -1, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )

/** warn(3) with function name at the start */
#define fwarn( ... )  ( void ) WarnUniversal( NULL, __func__, -1, errno, -1, __VA_ARGS__ )
#define fwarnx( ... ) ( void ) WarnUniversal( NULL, __func__, -1, -1, -1, __VA_ARGS__ )
#define fwarn_ret( RETVAL, ... ) \
    WarnUniversal( NULL, __func__, -1, errno, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )
#define fwarnx_ret( RETVAL, ... ) \
    WarnUniversal( NULL, __func__, -1, -1, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )

#define fflwarn( ... ) \
    ( void ) WarnUniversal( __FILE_NAME__, __func__, __LINE__, errno, -1, __VA_ARGS__ )
#define fflwarnx( ... ) \
    ( void ) WarnUniversal( __FILE_NAME__, __func__, __LINE__, -1, -1, __VA_ARGS__ )
#define fflwarn_ret( RETVAL, ... )       \
    WarnUniversal( __FILE_NAME__,        \
                   __func__,             \
                   __LINE__,             \
                   errno,                \
                   ( ptrdiff_t ) RETVAL, \
                   __VA_ARGS__ )
#define fflwarnx_ret( RETVAL, ... ) \
    WarnUniversal(                  \
            __FILE_NAME__, __func__, __LINE__, -1, ( ptrdiff_t ) RETVAL, __VA_ARGS__ )


#endif //CLIBS_ERRORS_H
