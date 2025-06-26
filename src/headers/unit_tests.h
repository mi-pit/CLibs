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

#include "filenames.h"       /* PATH_MAX */
#include "terminal_colors.h" /* colors */

#include <stdbool.h> /* * */
#include <stdio.h>   /* printf */
#include <stdlib.h>  /* exit */

#define Tester static bool


/* number of chars */
#if !defined( LINE_PREF_WIDTH )
#define LINE_PREF_WIDTH 166
#endif

#if LINE_PREF_WIDTH < 32
#define TESTS_LINE_WIDTH ( 32 + 1 )
#else
#define TESTS_LINE_WIDTH LINE_PREF_WIDTH
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


#define TEST_NAME_CREATOR( TOK ) CLIBS_UNIT_TESTS_##TOK


static int TEST_NAME_CREATOR( TOTAL_FAILED ) = 0;
static int TEST_NAME_CREATOR( TOTAL_RAN )    = 0;

static int TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) = 0;
static int TEST_NAME_CREATOR( TOTAL_RAN_UNIT )    = 0;

static bool TEST_NAME_CREATOR( YAP ) = true;


#ifndef UNIT_TESTS_SILENT
#define TEST( HANDLE )                                          \
    int TEST_NAME_CREATOR( HANDLE )( void )                     \
    {                                                           \
        int TEST_NAME_CREATOR( failed_total )        = 0;       \
        int TEST_NAME_CREATOR( ran_total )           = 0;       \
        const char *TEST_NAME_CREATOR( test_handle ) = #HANDLE; \
        printf( COLOR_TEST_TAG "[TEST]" COLOR_DEFAULT " " #HANDLE "\n" );
#else
#define TEST( HANDLE )                                    \
    int TEST_NAME_CREATOR( HANDLE )( void )               \
    {                                                     \
        int TEST_NAME_CREATOR( failed_total )        = 0; \
        int TEST_NAME_CREATOR( ran_total )           = 0; \
        const char *TEST_NAME_CREATOR( test_handle ) = #HANDLE;
#endif //UNIT_TESTS_SILENT

#define END_TEST                                                                    \
    printf( COLOR_TEST_TAG "[TEST]" COLOR_DEFAULT " %s: ran %i tests, " PRINT_COLOR \
                           "%i successful" COLOR_DEFAULT ", " PRINT_COLOR           \
                           "%i failed" COLOR_DEFAULT "\n",                          \
            TEST_NAME_CREATOR( test_handle ),                                       \
            TEST_NAME_CREATOR( ran_total ),                                         \
            TEST_NAME_CREATOR( ran_total ) - TEST_NAME_CREATOR( failed_total ) == 0 \
                    ? COLOR_FAIL                                                    \
                    : COLOR_SUCC,                                                   \
            TEST_NAME_CREATOR( ran_total ) - TEST_NAME_CREATOR( failed_total ),     \
            TEST_NAME_CREATOR( failed_total ) == 0 ? COLOR_SUCC : COLOR_FAIL,       \
            TEST_NAME_CREATOR( failed_total ) );                                    \
    TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) += TEST_NAME_CREATOR( failed_total );    \
    TEST_NAME_CREATOR( TOTAL_RAN_UNIT ) += TEST_NAME_CREATOR( ran_total );          \
    return TEST_NAME_CREATOR( failed_total ) == 0 ? 0 : 1;                          \
    }


#define RUN_TEST( HANDLE )                                                  \
    do                                                                      \
    {                                                                       \
        TEST_NAME_CREATOR( TOTAL_FAILED ) += TEST_NAME_CREATOR( HANDLE )(); \
        ++TEST_NAME_CREATOR( TOTAL_RAN );                                   \
    }                                                                       \
    while ( 0 )


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


LibraryDefined bool UNIT_TEST_( const char *cond_str,
                                const bool passed,
                                const char *filename,
                                const int lineno,
                                const bool isCritical )
{
#ifndef UNIT_TESTS_SILENT
    if ( TEST_NAME_CREATOR( YAP ) || !passed || isCritical )
    {
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
            static char buffer[ sizeof( __FILE_NAME__ ) + 20 + 7 + 1 ];
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
    }
#else
    ( void ) cond_str;
    ( void ) filename;
    ( void ) lineno;
#endif //UNIT_TESTS_SILENT

    return passed;
}


/**
 * If condition evaluates to true, "SUCCESS" is printed
 * in the color defined in COLOR_SUCC (green by default).
 * <p>
 * If condition evaluates to false, "FAILURE" is printed
 * in the color defined in COLOR_FAIL (red by default)
 */
#define UNIT_TEST( CONDITION )                                                      \
    do                                                                              \
    {                                                                               \
        if ( !UNIT_TEST_( #CONDITION, CONDITION, __FILE_NAME__, __LINE__, false ) ) \
            ++TEST_NAME_CREATOR( failed_total );                                    \
        ++TEST_NAME_CREATOR( ran_total );                                           \
    }                                                                               \
    while ( 0 )

// clang-format off
#define CRITICAL_TEST( CONDITION )                                                      \
    do                                                                                  \
    {                                                                                   \
        ++TEST_NAME_CREATOR( ran_total );                                               \
        if ( !UNIT_TEST_( #CONDITION, CONDITION, __FILE_NAME__, __LINE__, true ) )      \
        {                                                                               \
            ++TEST_NAME_CREATOR( failed_total );                                        \
            { END_TEST /* the END_TEST macro has a terminating bracket `}' */           \
        }                                                                               \
    }                                                                                   \
    while ( 0 )
// clang-format on


/**
 * If set to false, `UNIT_TEST`s do not show successes.
 *
 * This option is independent from the macro `UNIT_TESTS_SILENT`,
 * which silences all messages.
 *
 * @param verbose true by default
 */
LibraryDefined void SET_UNIT_TEST_VERBOSITY( const bool verbose )
{
    TEST_NAME_CREATOR( YAP ) = verbose;
}

LibraryDefined bool GET_UNIT_TEST_VERBOSITY( void )
{
    return TEST_NAME_CREATOR( YAP );
}

#endif //CLIBS_UNIT_TESTS_H
