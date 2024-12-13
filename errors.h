//
// Created by MacBook on 30.10.2024.
//

#ifndef CLIBS_ERRORS_H
#define CLIBS_ERRORS_H

#include <err.h>       /* include */
#include <errno.h>     /* include */
#include <printf.h>    /* include fprintf (for stack_traces) */
#include <stddef.h>    /* ptrdiff_t */
#include <sys/cdefs.h> /* __printflike */


#define E_OK 0 /* No error */


#ifndef __FILE_NAME__
#ifndef __FILE__
#define __FILE__ "current-program"
#endif //__FILE__

#define __FILE_NAME__ __FILE__
#endif //__FILE_NAME__

#ifndef __printflike
#define __printflike( A, B )
#endif //__printflike


#define RV_EXCEPTION ( -2 )
#define RV_ERROR     ( -1 )
#define RV_SUCCESS   0

#define return_on_fail( func_call )           \
    do                                        \
    {                                         \
        int func_call_retval = ( func_call ); \
        if ( func_call_retval != RV_SUCCESS ) \
            return func_call_retval;          \
    }                                         \
    while ( 0 )

#define return_on_error( func_call )          \
    do                                        \
    {                                         \
        int func_call_retval = ( func_call ); \
        if ( func_call_retval == RV_ERROR )   \
        {                                     \
            f_stack_trace();                  \
            return func_call_retval;          \
        }                                     \
    }                                         \
    while ( 0 )

#define return_on_fail_ptr( func_call )    \
    do                                     \
    {                                      \
        if ( ( func_call ) != RV_SUCCESS ) \
        {                                  \
            f_stack_trace();               \
            return NULL;                   \
        }                                  \
    }                                      \
    while ( 0 )


#define on_fail( func_call ) if ( ( func_call ) != RV_SUCCESS )

/**
 * Similar to warn(3) (especially warnc)
 * <p>
 * Prints
 * @code
 * ‹executable name›: ‹FileName›: ‹FunctionName› @ LineNumber: ‹formatted string›[:strerror(err_no)]
 * @endcode
 * The strerror part is only printed if err_no >= 0
 * </p>
 * <p>
 * If FileName or FunctionName is NULL, then they are not printed
 * </p>
 *
 * @example actual function-like macro defined in errors.h:
 * \code
 * #define fwarn( ... ) \
 *     ( void ) WarnUniversal( FUNC_ONLY, NULL, __func__, -1, errno, -1, __VA_ARGS__ )
 * \endcode
 *
 * @param FileName      name of the current file (__FILE_NAME__)
 * @param FunctionName  name of the function (__func__)
 * @param LineNumber    number of the line to warn about (__LINE__)
 * @param err_no        errno to warn about
 * @param return_value  returned value
 * @param format        format string
 * @param ...           printf-like arguments for ‹format›
 * @return  @code return_value
 */
int WarnUniversal( const char *restrict FileName,
                   const char *restrict FunctionName,
                   int LineNumber,
                   int err_no,
                   int return_value,
                   const char *__restrict format,
                   ... ) __printflike( 6, 7 );

#define FFL_FMTSTR "%s: %s @ %d"


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
#define f_stack_trace() ( ( void ) fprintf( stderr, "\tin %s\n", __func__ ) )

/**
 * Like f_stack_trace, just with __FILE_NAME__ and __LINE__
 * @see \code f_stack_trace()
 */
#define ffl_stack_trace() \
    ( ( void ) fprintf(   \
            stderr, "\tin " FFL_FMTSTR "\n", __FILE_NAME__, __func__, __LINE__ ) )


#define warn_ret( RETVAL, ... ) \
    WarnUniversal( NULL, NULL, -1, errno, RETVAL, __VA_ARGS__ )
#define warnx_ret( RETVAL, ... ) WarnUniversal( NULL, NULL, -1, -1, RETVAL, __VA_ARGS__ )

#define warn_ret_p( RETVAL, ... )           \
    ( void * ) ( ptrdiff_t ) WarnUniversal( \
            NULL, NULL, -1, -1, ( int ) ( ptrdiff_t ) RETVAL, __VA_ARGS__ )
#define warnx_ret_p( RETVAL, ... )          \
    ( void * ) ( ptrdiff_t ) WarnUniversal( \
            NULL, NULL, -1, -1, ( int ) ( ptrdiff_t ) RETVAL, __VA_ARGS__ )

/**
 * warn() with function name at the start
 */
#define fwarn( ... )  ( void ) WarnUniversal( NULL, __func__, -1, errno, -1, __VA_ARGS__ )
#define fwarnx( ... ) ( void ) WarnUniversal( NULL, __func__, -1, -1, -1, __VA_ARGS__ )
#define fwarn_ret( RETVAL, ... ) \
    WarnUniversal( NULL, __func__, -1, errno, RETVAL, __VA_ARGS__ )
#define fwarnx_ret( RETVAL, ... ) \
    WarnUniversal( NULL, __func__, -1, -1, RETVAL, __VA_ARGS__ )
#define fwarn_ret_p( RETVAL, ... )          \
    ( void * ) ( ptrdiff_t ) WarnUniversal( \
            NULL, __func__, -1, errno, ( int ) ( ptrdiff_t ) RETVAL, __VA_ARGS__ )
#define fwarnx_ret_p( RETVAL, ... )         \
    ( void * ) ( ptrdiff_t ) WarnUniversal( \
            NULL, __func__, -1, -1, ( int ) ( ptrdiff_t ) RETVAL, __VA_ARGS__ )

#define fflwarn( ... ) \
    ( void ) WarnUniversal( __FILE_NAME__, __func__, __LINE__, errno, -1, __VA_ARGS__ )
#define fflwarnx( ... ) \
    ( void ) WarnUniversal( __FILE_NAME__, __func__, __LINE__, -1, -1, __VA_ARGS__ )
#define fflwarn_ret( RETVAL, ... ) \
    WarnUniversal( __FILE_NAME__, __func__, __LINE__, errno, RETVAL, __VA_ARGS__ )
#define fflwarnx_ret( RETVAL, ... ) \
    WarnUniversal( __FILE_NAME__, __func__, __LINE__, -1, RETVAL, __VA_ARGS__ )
#define fflwarn_ret_p( RETVAL, ... )                                      \
    ( void * ) ( ptrdiff_t ) WarnUniversal( __FILE_NAME__,                \
                                            __func__,                     \
                                            __LINE__,                     \
                                            errno,                        \
                                            ( int ) ( ptrdiff_t ) RETVAL, \
                                            __VA_ARGS__ )
#define fflwarnx_ret_p( ... )                                             \
    ( void * ) ( ptrdiff_t ) WarnUniversal( __FILE_NAME__,                \
                                            __func__,                     \
                                            __LINE__,                     \
                                            -1,                           \
                                            ( int ) ( ptrdiff_t ) RETVAL, \
                                            __VA_ARGS__ )


#endif //CLIBS_ERRORS_H
