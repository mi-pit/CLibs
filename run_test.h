//
// Created by MacBook on 03.01.2025.
//

#ifndef CLIBS_RUN_TEST_H
#define CLIBS_RUN_TEST_H

#include <stdbool.h>
#include <stdio.h>

#ifndef LINE_MAX_WIDTH
#define LINE_MAX_WIDTH 165
#endif


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

#define COLOR_DEFAULT "\033[0m"

#define COLOR_RED    "\033[31m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_YELLOW "\033[33m"


#ifndef COLOR_FAIL
#define COLOR_FAIL COLOR_RED
#endif //COLOR_FAIL

#ifndef COLOR_SUCC
#define COLOR_SUCC COLOR_GREEN
#endif //COLOR_SUCC

#ifndef COLOR_NOTE
#define COLOR_NOTE COLOR_YELLOW
#endif //COLOR_NOTE


#define PRINT_COLOR "%s"


#define TEST_NAME_CREATOR( TOK ) RUNTEST_##TOK


static int TEST_NAME_CREATOR( TOTAL_FAILED ) = 0;


#define TEST( HANDLE )                                          \
    int TEST_NAME_CREATOR( HANDLE )( void )                     \
    {                                                           \
        int TEST_NAME_CREATOR( failed_total )        = 0;       \
        int TEST_NAME_CREATOR( ran_total )           = 0;       \
        const char *TEST_NAME_CREATOR( test_handle ) = #HANDLE; \
        printf( PRINT_COLOR "[TEST] " PRINT_COLOR #HANDLE "\n", \
                COLOR_YELLOW,                                   \
                COLOR_DEFAULT );

#define END_TEST                                                                    \
    printf( COLOR_YELLOW "[TEST] " COLOR_DEFAULT "%s: ran %i tests, " PRINT_COLOR   \
                         "%i successful" COLOR_DEFAULT ", " PRINT_COLOR             \
                         "%i failed\n" COLOR_DEFAULT,                               \
            TEST_NAME_CREATOR( test_handle ),                                       \
            TEST_NAME_CREATOR( ran_total ),                                         \
            TEST_NAME_CREATOR( ran_total ) - TEST_NAME_CREATOR( failed_total ) == 0 \
                    ? COLOR_FAIL                                                    \
                    : COLOR_SUCC,                                                   \
            TEST_NAME_CREATOR( ran_total ) - TEST_NAME_CREATOR( failed_total ),     \
            TEST_NAME_CREATOR( failed_total ) == 0 ? COLOR_SUCC : COLOR_FAIL,       \
            TEST_NAME_CREATOR( failed_total ) );                                    \
    return TEST_NAME_CREATOR( failed_total ) == 0 ? 0 : 1;                          \
    }


#define RUN( HANDLE ) TEST_NAME_CREATOR( TOTAL_FAILED ) += TEST_NAME_CREATOR( HANDLE )()

#define FINISH_TESTING()                                                                \
    ( ( void ) printf( COLOR_NOTE "[SUMMARY]" COLOR_DEFAULT                             \
                                  " total tests failed: " COLOR_NOTE "%i" COLOR_DEFAULT \
                                  "\n",                                                 \
                       TEST_NAME_CREATOR( TOTAL_FAILED ) ) )


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
#define UNIT_TEST( CONDITION )                                                      \
    do                                                                              \
    {                                                                               \
        printf( PRINT_COLOR "    [UNIT TEST] " PRINT_COLOR #CONDITION " ",          \
                COLOR_YELLOW,                                                       \
                COLOR_DEFAULT );                                                    \
        size_t TEST_NAME_CREATOR( test_name_len ) = sizeof #CONDITION;              \
        bool TEST_NAME_CREATOR( success )         = CONDITION;                      \
        const size_t TEST_NAME_CREATOR( LIMIT ) =                                   \
                MIN( 300 - ( 32 + TEST_NAME_CREATOR( test_name_len ) ),             \
                     MAX( 0,                                                        \
                          LINE_MAX_WIDTH -                                          \
                                  ( 32 + TEST_NAME_CREATOR( test_name_len ) ) ) ) + \
                1;                                                                  \
        for ( size_t TEST_NAME_CREATOR( index ) = 0;                                \
              TEST_NAME_CREATOR( index ) < TEST_NAME_CREATOR( LIMIT );              \
              ++TEST_NAME_CREATOR( index ) )                                        \
            printf( "." );                                                          \
        printf( " " PRINT_COLOR "%s" PRINT_COLOR "\n",                              \
                TEST_NAME_CREATOR( success ) ? COLOR_SUCC : COLOR_FAIL,             \
                TEST_NAME_CREATOR( success ) ? "SUCCESS" : "FAILURE",               \
                COLOR_DEFAULT );                                                    \
        if ( !TEST_NAME_CREATOR( success ) )                                        \
            ++TEST_NAME_CREATOR( failed_total );                                    \
        ++TEST_NAME_CREATOR( ran_total );                                           \
    }                                                                               \
    while ( 0 )

#endif //CLIBS_RUN_TEST_H
