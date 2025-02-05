//
// Created by MacBook on 03.01.2025.
//
/**
 * @example
 * @code
 * TEST( test_example )
 *     UNIT_TEST( 1 == 1 );
 *     UNIT_TEST( 1 == 2 );
 *     UNIT_TEST( 2 == 1 );
 *     UNIT_TEST( 2 == 2 );
 * END_TEST()
 *
 * int main( void )
 * {
 *     RUN( test_example );
 *     FINISH_TESTING();
 * }
 *
 * @endcode
 * <p>
 * prints:
 * @code
 * "[TEST] test_example
 *     [UNIT TEST] 1 == 1 ................... SUCCESS
 *     [UNIT TEST] 1 == 2 ................... FAILURE
 *     [UNIT TEST] 2 == 1 ................... FAILURE
 *     [UNIT TEST] 2 == 2 ................... SUCCESS
 * [TEST] test_example: ran 4 tests, 2 successful, 2 failed
 * [SUMMARY] total tests failed: 1
 * "
 * @endcode
 */

#ifndef CLIBS_UNIT_TESTS_H
#define CLIBS_UNIT_TESTS_H

#include "terminal_colors.h" /* colors */

#include <stdbool.h> /* * */
#include <stdio.h>   /* printf */
#include <stdlib.h>  /* exit */

#define Tester static bool
// todo?: #define TESTER( NAME ) static bool test_one_##NAME


#define MSG_CONST_PART_LEN 25

#if !defined( LINE_PREF_WIDTH )
#define LINE_PREF_WIDTH 158
#endif

#if LINE_PREF_WIDTH < MSG_CONST_PART_LEN
#define TESTS_LINE_WIDTH ( MSG_CONST_PART_LEN + 1 )
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


#define TEST( HANDLE )                                          \
    int TEST_NAME_CREATOR( HANDLE )( void )                     \
    {                                                           \
        int TEST_NAME_CREATOR( failed_total )        = 0;       \
        int TEST_NAME_CREATOR( ran_total )           = 0;       \
        const char *TEST_NAME_CREATOR( test_handle ) = #HANDLE; \
        printf( COLOR_TEST_TAG "[TEST]" COLOR_DEFAULT " " #HANDLE "\n" );

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
    printf( COLOR_NOTE "\n[SUMMARY]" COLOR_DEFAULT " total unit tests ran: " COLOR_NOTE
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


LibraryDefined bool UNIT_TEST_( const char *cond_str, bool passed )
{
    printf( "    " COLOR_TEST_TAG "[UNIT TEST]" COLOR_DEFAULT " %s ", cond_str );
    const size_t TEST_NAME_LEN = strlen( cond_str );
    ssize_t NDOTS = TESTS_LINE_WIDTH - ( MSG_CONST_PART_LEN + TEST_NAME_LEN );
    if ( NDOTS < 0 )
    {
        printf( "\n                 " );
        NDOTS = TESTS_LINE_WIDTH - MSG_CONST_PART_LEN;
    }

    for ( size_t index = 0; index < ( size_t ) NDOTS; ++index )
        printf( "." );
    printf( " " PRINT_COLOR "%s" COLOR_DEFAULT "\n",
            passed ? COLOR_SUCC : COLOR_FAIL,
            passed ? "SUCCESS" : "FAILURE" );

    return passed;
}

/**
 * If condition evaluates to true, "SUCCESS" is printed
 * in the color defined in COLOR_SUCC (green by default).
 * <p>
 * If condition evaluates to false, "FAILURE" is printed
 * in the color defined in COLOR_FAIL (red by default)
 */
#define UNIT_TEST( CONDITION )                      \
    do                                              \
    {                                               \
        if ( !UNIT_TEST_( #CONDITION, CONDITION ) ) \
            ++TEST_NAME_CREATOR( failed_total );    \
        ++TEST_NAME_CREATOR( ran_total );           \
    }                                               \
    while ( 0 )

#endif //CLIBS_UNIT_TESTS_H
