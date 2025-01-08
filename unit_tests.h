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

#include <stdbool.h> /* * */
#include <stdio.h>   /* printf */
#include <stdlib.h>  /* exit */

#define Tester static bool
// todo?: #define TESTER( NAME ) static bool test_one_##NAME


#ifndef LINE_MAX_WIDTH
#define LINE_MAX_WIDTH 165
#endif


/**
 * When supplied with the correct ANSI color code,
 * this macro creates the escape code for the corresponding color
 */
#define COLOR_CREATOR( NUM ) "\033[" #NUM "m"

#define COLOR_DEFAULT COLOR_CREATOR( 0 )

#define COLOR_BLACK   COLOR_CREATOR( 30 )
#define COLOR_RED     COLOR_CREATOR( 31 )
#define COLOR_GREEN   COLOR_CREATOR( 32 )
#define COLOR_YELLOW  COLOR_CREATOR( 33 )
#define COLOR_BLUE    COLOR_CREATOR( 34 )
#define COLOR_MAGENTA COLOR_CREATOR( 35 )
#define COLOR_CYAN    COLOR_CREATOR( 36 )
#define COLOR_WHITE   COLOR_CREATOR( 37 )


#ifndef COLOR_FAIL
#define COLOR_FAIL COLOR_RED
#endif //COLOR_FAIL

#ifndef COLOR_SUCC
#define COLOR_SUCC COLOR_GREEN
#endif //COLOR_SUCC

#ifndef COLOR_NOTE
#define COLOR_NOTE COLOR_CYAN
#endif //COLOR_NOTE

#ifndef COLOR_TEST_TAG
#define COLOR_TEST_TAG COLOR_YELLOW
#endif //COLOR_TEST_TAG


#define PRINT_COLOR "%s"


#define TEST_NAME_CREATOR( TOK ) CLIBS_UNIT_TESTS_##TOK


static int TEST_NAME_CREATOR( TOTAL_FAILED ) = 0;
static int TEST_NAME_CREATOR( TOTAL_RAN )    = 0;

static int TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) = 0;
static int TEST_NAME_CREATOR( TOTAL_RAN_UNIT )    = 0;


#define DECLARE_TEST( HANDLE ) int TEST_NAME_CREATOR( HANDLE )( void )

#define TEST( HANDLE )                                          \
    int TEST_NAME_CREATOR( HANDLE )( void )                     \
    {                                                           \
        int TEST_NAME_CREATOR( failed_total )        = 0;       \
        int TEST_NAME_CREATOR( ran_total )           = 0;       \
        const char *TEST_NAME_CREATOR( test_handle ) = #HANDLE; \
        printf( COLOR_TEST_TAG "[TEST] " COLOR_DEFAULT #HANDLE "\n" );

#define END_TEST                                                                    \
    printf( COLOR_TEST_TAG "[TEST] " COLOR_DEFAULT "%s: ran %i tests, " PRINT_COLOR \
                           "%i successful" COLOR_DEFAULT ", " PRINT_COLOR           \
                           "%i failed\n" COLOR_DEFAULT,                             \
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


#define FINISH_TESTING()                                                                \
    do                                                                                  \
    {                                                                                   \
        puts( "" );                                                                     \
        printf( COLOR_NOTE "[SUMMARY]" COLOR_DEFAULT                                    \
                           " total unit tests ran: " COLOR_NOTE "%i" COLOR_DEFAULT      \
                           ", total unit tests failed: " PRINT_COLOR "%i" COLOR_DEFAULT \
                           "\n",                                                        \
                TEST_NAME_CREATOR( TOTAL_RAN_UNIT ),                                    \
                TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) == 0 ? COLOR_SUCC : COLOR_FAIL,  \
                TEST_NAME_CREATOR( TOTAL_FAILED_UNIT ) );                               \
                                                                                        \
        printf( COLOR_NOTE "[SUMMARY]" COLOR_DEFAULT " total tests ran: " COLOR_NOTE    \
                           "%i" COLOR_DEFAULT ", total tests failed: " PRINT_COLOR      \
                           "%i" COLOR_DEFAULT "\n",                                     \
                TEST_NAME_CREATOR( TOTAL_RAN ),                                         \
                TEST_NAME_CREATOR( TOTAL_FAILED ) == 0 ? COLOR_SUCC : COLOR_FAIL,       \
                TEST_NAME_CREATOR( TOTAL_FAILED ) );                                    \
        exit( TEST_NAME_CREATOR( TOTAL_FAILED ) == 0 ? EXIT_SUCCESS : EXIT_FAILURE );   \
    }                                                                                   \
    while ( 0 )

#define MAX( A, B ) ( ( ( A ) > ( B ) ) ? ( A ) : ( B ) )
#define MIN( A, B ) ( ( ( A ) < ( B ) ) ? ( A ) : ( B ) )

/**
 * If condition evaluates to true, "SUCCESS" is printed
 * in the color defined in COLOR_SUCC (green by default).
 * <p>
 * If condition evaluates to false, "FAILURE" is printed
 * in the color defined in COLOR_FAIL (red by default)
 * and an optional message is printed (has to be string literal)
 */
#define UNIT_TEST( CONDITION )                                                       \
    do                                                                               \
    {                                                                                \
        printf( COLOR_TEST_TAG "    [UNIT TEST] " COLOR_DEFAULT "%s ", #CONDITION ); \
        size_t TEST_NAME_CREATOR( test_name_len ) = sizeof #CONDITION;               \
        bool TEST_NAME_CREATOR( success )         = CONDITION;                       \
        const size_t TEST_NAME_CREATOR( LIMIT ) =                                    \
                MIN( 300 - ( 32 + TEST_NAME_CREATOR( test_name_len ) ),              \
                     MAX( 0,                                                         \
                          LINE_MAX_WIDTH -                                           \
                                  ( 32 + TEST_NAME_CREATOR( test_name_len ) ) ) ) +  \
                1;                                                                   \
        for ( size_t TEST_NAME_CREATOR( index ) = 0;                                 \
              TEST_NAME_CREATOR( index ) < TEST_NAME_CREATOR( LIMIT );               \
              ++TEST_NAME_CREATOR( index ) )                                         \
            printf( "." );                                                           \
        printf( " " PRINT_COLOR "%s" COLOR_DEFAULT "\n",                             \
                TEST_NAME_CREATOR( success ) ? COLOR_SUCC : COLOR_FAIL,              \
                TEST_NAME_CREATOR( success ) ? "SUCCESS" : "FAILURE" );              \
        if ( !TEST_NAME_CREATOR( success ) )                                         \
            ++TEST_NAME_CREATOR( failed_total );                                     \
        ++TEST_NAME_CREATOR( ran_total );                                            \
    }                                                                                \
    while ( 0 )

#endif //CLIBS_UNIT_TESTS_H
