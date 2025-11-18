/*
 * Header for unit-testing.
 *
 * For more info see `docs/unit-tests.md`.
 *
 *
 * Created by MacBook on 03.01.2025.
 */

#ifndef CLIBS_UNIT_TESTS_H
#define CLIBS_UNIT_TESTS_H

#include "errors.h"          /* fwarn, UNREACHABLE */
#include "filenames.h"       /* PATH_MAX */
#include "misc.h"            /* STRLEN */
#include "terminal_colors.h" /* colors */

#include <stdbool.h> /* * */
#include <stdio.h>   /* printf */
#include <stdlib.h>  /* exit */


#if !defined( LINE_PREF_WIDTH )
/** Preferred number of chars in a line */
#define LINE_PREF_WIDTH 100
#endif

#if LINE_PREF_WIDTH < 32
static const size_t TESTS_LINE_WIDTH = ( 32 + 1 );
#else
static const size_t TESTS_LINE_WIDTH = LINE_PREF_WIDTH;
#endif


#ifndef COLOR_FAIL
#define COLOR_FAIL FOREGROUND_RED
#endif //COLOR_FAIL

#ifndef COLOR_SUCC
#define COLOR_SUCC FOREGROUND_GREEN
#endif //COLOR_SUCC

#ifndef COLOR_NOTE
#define COLOR_NOTE FOREGROUND_CYAN
#endif //COLOR_NOTE

#ifndef COLOR_TEST_TAG
#define COLOR_TEST_TAG FOREGROUND_YELLOW
#endif //COLOR_TEST_TAG


/// To avoid namespace collisions
#define TEST_NAME_CREATOR( TOK ) CLIBS_UNIT_TESTS__##TOK


typedef enum {
    UNIT_TESTS_YAP_NONE     = 0,      /* Don't print any */
    UNIT_TESTS_YAP_ALL      = 1 << 1, /* Default; print all cases */
    UNIT_TESTS_YAP_FAILED   = 1 << 2, /* Only print if case fails */
    UNIT_TESTS_YAP_CRITICAL = 1 << 3, /* Only print if case is critical */

    /* Only print if case both fails and is critical */
    UNIT_TESTS_YAP_CRITICAL_FAILED = UNIT_TESTS_YAP_CRITICAL | UNIT_TESTS_YAP_FAILED,
} TEST_NAME_CREATOR( VERBOSITY_ENUM );


static TEST_NAME_CREATOR( VERBOSITY_ENUM )
        TEST_NAME_CREATOR( VERBOSITY ) = UNIT_TESTS_YAP_ALL;

static int TEST_NAME_CREATOR( TOTAL_FAILED ) = 0;
static int TEST_NAME_CREATOR( TOTAL_RAN )    = 0;

static int TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) = 0;
static int TEST_NAME_CREATOR( TOTAL_RAN_UNIT )    = 0;


#ifndef UNIT_TESTS_SILENT
/**
 * Begin a test block.
 *
 * Counts the number of tests ran/failed.
 * Must be terminated by `END_TEST`.
 *
 * If `UNIT_TESTS_SILENT` is **not** defined,
 * this macro prints the test name as the start of the test case.
 *
 * @param HANDLE handle of the test case -- must be unique
 */
#define TEST( HANDLE )                                                     \
    int TEST_NAME_CREATOR( HANDLE )( void )                                \
    {                                                                      \
        int TEST_NAME_CREATOR( failed_total )        = 0;                  \
        int TEST_NAME_CREATOR( ran_total )           = 0;                  \
        const char *TEST_NAME_CREATOR( test_handle ) = #HANDLE;            \
        printf( COLOR_TEST_TAG "[TEST ... " COLOR_NOTE "%s" COLOR_TEST_TAG \
                               "]" COLOR_DEFAULT " %s\n",                  \
                __FILE_NAME__,                                             \
                #HANDLE );
#else
#define TEST( HANDLE )                                    \
    int TEST_NAME_CREATOR( HANDLE )( void )               \
    {                                                     \
        int TEST_NAME_CREATOR( failed_total )        = 0; \
        int TEST_NAME_CREATOR( ran_total )           = 0; \
        const char *TEST_NAME_CREATOR( test_handle ) = #HANDLE;
#endif //UNIT_TESTS_SILENT

/**
 * Ends a `TEST()` block.
 *
 * Prints a summary of the test case and returns if it failed.
 */
#define END_TEST                                                                         \
    printf( COLOR_TEST_TAG "[TEST ... " COLOR_NOTE "%s" COLOR_TEST_TAG "]" COLOR_DEFAULT \
                           " %s: ran %i tests, " PRINT_COLOR                             \
                           "%i successful" COLOR_DEFAULT ", " PRINT_COLOR                \
                           "%i failed" COLOR_DEFAULT "\n",                               \
            __FILE_NAME__,                                                               \
            TEST_NAME_CREATOR( test_handle ),                                            \
            TEST_NAME_CREATOR( ran_total ),                                              \
            TEST_NAME_CREATOR( ran_total ) - TEST_NAME_CREATOR( failed_total ) == 0      \
                    ? COLOR_FAIL /* if any fail -> COLOR_FAIL */                         \
                    : COLOR_SUCC,                                                        \
            TEST_NAME_CREATOR( ran_total ) - TEST_NAME_CREATOR( failed_total ),          \
            TEST_NAME_CREATOR( failed_total ) == 0 ? COLOR_SUCC : COLOR_FAIL,            \
            TEST_NAME_CREATOR( failed_total ) );                                         \
    TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) += TEST_NAME_CREATOR( failed_total );         \
    TEST_NAME_CREATOR( TOTAL_RAN_UNIT ) += TEST_NAME_CREATOR( ran_total );               \
    return TEST_NAME_CREATOR( failed_total ) == 0 ? 0 : 1;                               \
    }


/**
 * Runs a test block and increments the proper static variables.
 *
 * @param HANDLE handle as used in `TEST( HANDLE )`
 */
#define RUN_TEST( HANDLE )                                                  \
    do                                                                      \
    {                                                                       \
        TEST_NAME_CREATOR( TOTAL_FAILED ) += TEST_NAME_CREATOR( HANDLE )(); \
        ++TEST_NAME_CREATOR( TOTAL_RAN );                                   \
    }                                                                       \
    while ( 0 )


/**
 * Prints a summary of all `TEST()` and `UNIT_TEST()` cases and exits.
 *
 * Exit value is `EXIT_SUCCESS` if no unit test failed, else `EXIT_FAILURE`.
 */
LibraryDefined NoReturn void FINISH_TESTING( void )
{
    printf( "\n" COLOR_NOTE "[SUMMARY]" COLOR_DEFAULT " total unit tests ran: " COLOR_NOTE
            "%i" COLOR_DEFAULT ", total unit tests failed: " PRINT_COLOR
            "%i" COLOR_DEFAULT "\n",
            TEST_NAME_CREATOR( TOTAL_RAN_UNIT ),
            TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) == 0 ? COLOR_SUCC : COLOR_FAIL,
            TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) );

    printf( COLOR_NOTE "[SUMMARY]" COLOR_DEFAULT " total tests ran: " COLOR_NOTE
                       "%i" COLOR_DEFAULT ", total tests failed: " PRINT_COLOR
                       "%i" COLOR_DEFAULT "\n",
            TEST_NAME_CREATOR( TOTAL_RAN ),
            TEST_NAME_CREATOR( TOTAL_FAILED ) == 0 ? COLOR_SUCC : COLOR_FAIL,
            TEST_NAME_CREATOR( TOTAL_FAILED ) );

    exit( TEST_NAME_CREATOR( TOTAL_FAILED ) == 0 ? EXIT_SUCCESS : EXIT_FAILURE );
}


/** @cond INTERNAL */
LibraryDefined bool TEST_NAME_CREATOR( UNIT_TEST )( const char *cond_str,
                                                    const bool passed,
                                                    const char *filename,
                                                    const int lineno,
                                                    const bool isCritical )
{
#ifndef UNIT_TESTS_SILENT
    switch ( TEST_NAME_CREATOR( VERBOSITY ) )
    {
        case UNIT_TESTS_YAP_NONE:
            return passed;

        case UNIT_TESTS_YAP_ALL:
            break;

        case UNIT_TESTS_YAP_FAILED:
            if ( passed )
                return passed;
            break;

        case UNIT_TESTS_YAP_CRITICAL:
            if ( !isCritical )
                return passed;
            break;

        case UNIT_TESTS_YAP_CRITICAL_FAILED:
            if ( !isCritical || passed )
                return passed;
            break;

        default:
            UNREACHABLE();
    }

    printf( "    " );
    static const size_t msg_indent = STRLEN( "    " );
    ssize_t ln                     = TESTS_LINE_WIDTH - msg_indent;

    SetTerminalColor( stdout,
                      !isCritical ? COLOR_TEST_TAG
                      : passed    ? COLOR_SUCC
                                  : COLOR_FAIL );

    ln -= printf( "[%s", isCritical ? "CRITICAL" : "UNIT TEST" );

    if ( !passed )
    {
        static char buffer[ PATH_MAX ];
        snprintf( buffer, sizeof buffer, " // %s @ %d", filename, lineno );
        ln -= printf( "%s", buffer );
    }
    ln -= printf( "]" );
    SetTerminalColor( stdout, COLOR_DEFAULT );
    ln -= printf( " %s ", cond_str );

    static const size_t MSG_END_PART_LEN = STRLEN( " SUCCESS" ) /* or failure */;

    ln -= MSG_END_PART_LEN;

    if ( ln < 0 )
    {
        printf( "\n" );
        for ( size_t i = 0; i < msg_indent; ++i )
            printf( " " );
    }

#if defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 201112L
    _Static_assert( TESTS_LINE_WIDTH > ( MSG_END_PART_LEN + msg_indent ),
                    "TESTS_LINE_WIDTH must be greater than the const part" );
#endif

    const size_t ndots =
            ln > 0 ? ( size_t ) ln : TESTS_LINE_WIDTH - MSG_END_PART_LEN - msg_indent;

    for ( size_t i = 0; i < ndots; ++i )
        printf( "." );

    printf( " " PRINT_COLOR "%s" COLOR_DEFAULT "\n",
            passed ? COLOR_SUCC : COLOR_FAIL,
            passed ? "SUCCESS" : "FAILURE" );
#else
    ( void ) cond_str;
    ( void ) filename;
    ( void ) lineno;
    ( void ) isCritical;
#endif //UNIT_TESTS_SILENT

    return passed;
}
/** @endcond */


/**
 * If condition evaluates to true, "SUCCESS" is printed
 * in the color defined in `COLOR_SUCC` (green by default).
 *
 * If condition evaluates to false, "FAILURE" is printed
 * in the color defined in `COLOR_FAIL` (red by default)
 */
#define UNIT_TEST( CONDITION )                                                 \
    do                                                                         \
    {                                                                          \
        if ( !TEST_NAME_CREATOR( UNIT_TEST )(                                  \
                     #CONDITION, CONDITION, __FILE_NAME__, __LINE__, false ) ) \
            ++TEST_NAME_CREATOR( failed_total );                               \
        ++TEST_NAME_CREATOR( ran_total );                                      \
    }                                                                          \
    while ( 0 )


// clang-format off
/**
 * If condition evaluates to true, "SUCCESS" is printed
 * in the color defined in `COLOR_SUCC` (green by default).
 *
 * If condition evaluates to false, "FAILURE" is printed
 * in the color defined in `COLOR_FAIL` (red by default),
 * and the `TEST()` case returns.
 */
#define CRITICAL_TEST( CONDITION )                                                      \
    do                                                                                  \
    {                                                                                   \
        ++TEST_NAME_CREATOR( ran_total );                                               \
        if ( !TEST_NAME_CREATOR( UNIT_TEST )(                                           \
                #CONDITION, CONDITION, __FILE_NAME__, __LINE__, true ) )                \
        {                                                                               \
            ++TEST_NAME_CREATOR( failed_total );                                        \
            { END_TEST /* the END_TEST macro has a terminating bracket `}' */           \
        }                                                                               \
    }                                                                                   \
    while ( 0 )
// clang-format on


/**
 * TODO
 * @param v
 */
LibraryDefined bool SET_UNIT_TEST_VERBOSITY( const TEST_NAME_CREATOR( VERBOSITY_ENUM ) v )
{
    switch ( v )
    {
        case UNIT_TESTS_YAP_NONE:
        case UNIT_TESTS_YAP_ALL:
        case UNIT_TESTS_YAP_FAILED:
        case UNIT_TESTS_YAP_CRITICAL:
        case UNIT_TESTS_YAP_CRITICAL_FAILED:
            TEST_NAME_CREATOR( VERBOSITY ) = v;
            return true;

        default:
            return fwarnx_ret( false, "invalid argument (%d)", v );
    }
}

/**
 * @return value of the static verbosity variable
 */
LibraryDefined TEST_NAME_CREATOR( VERBOSITY_ENUM ) GET_UNIT_TEST_VERBOSITY( void )
{
    return TEST_NAME_CREATOR( VERBOSITY );
}

#endif //CLIBS_UNIT_TESTS_H
