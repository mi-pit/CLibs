//
// Created by MacBook on 30.10.2024.
//

#ifndef CLIBS_ERRORS_H
#define CLIBS_ERRORS_H

#include <err.h>       /* warn */
#include <errno.h>     /* errno */
#include <stdio.h>     /* fprintf */
#include <sys/cdefs.h> /* __printflike */


#ifndef __FILE_NAME__

#ifndef __FILE__
#define __FILE__ "current-program"
#endif //__FILE__

#define __FILE_NAME__ __FILE__

#endif //__FILE_NAME__


#define RV_EXCEPTION ( -2 )
#define RV_ERROR     ( -1 )
#define RV_SUCCESS   0

#define print_stack_trace() \
    ( ( void ) fprintf(     \
            stderr, "\tin " FFL_FMTSTR "\n", __FILE_NAME__, __func__, __LINE__ ) )

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
            print_stack_trace();              \
            return func_call_retval;          \
        }                                     \
    }                                         \
    while ( 0 )

#define return_on_fail_ptr( func_call )    \
    do                                     \
    {                                      \
        if ( ( func_call ) != RV_SUCCESS ) \
        {                                  \
            print_stack_trace();           \
            return NULL;                   \
        }                                  \
    }                                      \
    while ( 0 )


#define on_fail( func_call ) if ( ( func_call ) != RV_SUCCESS )

#ifndef __printflike
#define __printflike( A, B )
#endif //__printflike

int warn_ret( int rv, const char *fmt, ... ) __printflike( 2, 3 );
int warnx_ret( int rv, const char *fmt, ... ) __printflike( 2, 3 );
void *warn_ret_p( const char *restrict fmt, ... ) __printflike( 1, 2 );
void *warnx_ret_p( const char *restrict fmt, ... ) __printflike( 1, 2 );


#define FFL_FMTSTR    "%s: %s @ %d"
#define FFWARN_FMTSTR FFL_FMTSTR ": "

/**
 * warn() with "__FILE_NAME__: __func__:" at the start
 */
#define ffwarn( fmtstr, ... ) \
    warn( FFWARN_FMTSTR fmtstr, __FILE_NAME__, __func__, __LINE__, __VA_ARGS__ )
/**
 * warnx() with "__FILE_NAME__: __func__:" at the start
 */
#define ffwarnx( fmtstr, ... ) \
    warnx( FFWARN_FMTSTR fmtstr, __FILE_NAME__, __func__, __LINE__, __VA_ARGS__ )

#define ffwarn_ret( rv, fmtstr, ... ) \
    warn_ret( rv, FFWARN_FMTSTR fmtstr, __FILE_NAME__, __func__, __LINE__, __VA_ARGS__ )
#define ffwarnx_ret( rv, fmtstr, ... ) \
    warnx_ret( rv, FFWARN_FMTSTR fmtstr, __FILE_NAME__, __func__, __LINE__, __VA_ARGS__ )

#define ffwarn_ret_p( fmtstr, ... ) \
    warn_ret_p( FFWARN_FMTSTR fmtstr, __FILE_NAME__, __func__, __LINE__, __VA_ARGS__ )
#define ffwarnx_ret_p( fmtstr, ... ) \
    warnx_ret_p( FFWARN_FMTSTR fmtstr, __FILE_NAME__, __func__, __LINE__, __VA_ARGS__ )


#endif //CLIBS_ERRORS_H
