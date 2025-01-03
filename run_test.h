//
// Created by MacBook on 03.01.2025.
//

#ifndef CLIBS_RUN_TEST_H
#define CLIBS_RUN_TEST_H


#ifndef COLUMN_MAX_LENGHT
#define COLUMN_MAX_LENGHT 164
#endif //COLUMN_MAX_LENGHT


/**
 * @example
 * @code
 * TEST( test_example )
 *     UNIT_TEST( true );
 *     UNIT_TEST( true );
 *     UNIT_TEST( false );
 *     UNIT_TEST( true );
 * END_TEST()
 *
 * int main( void )
 * {
 *     RUN( test_example );
 * }
 *
 * @endcode
 * <p>
 * prints:
 * @code "
 * [TEST] test_example
 *     [UNIT TEST] true .................... SUCCESS
 *     [UNIT TEST] true .................... SUCCESS
 *     [UNIT TEST] false ................... FAILURE
 *     [UNIT TEST] true .................... SUCCESS
 * [TEST] test_example: ran 4 tests, 3 successful, 1 failed
 * "
 * @endcode
 */

#define COLOR_DEFAULT "\033[0m"

#define COLOR_RED    "\033[31m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_YELLOW "\033[33m"

#define PRINT_COLOR "%s"


#define TEST( HANDLE )                                          \
    void TEST_##HANDLE( void )                                  \
    {                                                           \
        int TEST_failed_total   = 0;                            \
        int TEST_ran_total      = 0;                            \
        const char *test_handle = #HANDLE;                      \
        printf( PRINT_COLOR "[TEST] " PRINT_COLOR #HANDLE "\n", \
                COLOR_YELLOW,                                   \
                COLOR_DEFAULT );

#define END_TEST                                                                  \
    printf( COLOR_YELLOW "[TEST] " COLOR_DEFAULT "%s: ran %i tests, " PRINT_COLOR \
                         "%i successful" COLOR_DEFAULT ", " PRINT_COLOR           \
                         "%i failed\n" COLOR_DEFAULT,                             \
            test_handle,                                                          \
            TEST_ran_total,                                                       \
            TEST_ran_total - TEST_failed_total == 0 ? COLOR_RED : COLOR_GREEN,    \
            TEST_ran_total - TEST_failed_total,                                   \
            TEST_failed_total == 0 ? COLOR_GREEN : COLOR_RED,                     \
            TEST_failed_total );                                                  \
    }


#define RUN( HANDLE ) TEST_##HANDLE()


#ifndef MAX
#define MAX( A, B ) ( ( ( A ) > ( B ) ) ? ( A ) : ( B ) )
#endif //MAX

#ifndef MIN
#define MIN( A, B ) ( ( ( A ) < ( B ) ) ? ( A ) : ( B ) )
#endif //MIN

#define UNIT_TEST( CONDITION )                                                          \
    do                                                                                  \
    {                                                                                   \
        printf( PRINT_COLOR "    [UNIT TEST] " PRINT_COLOR #CONDITION " ",              \
                COLOR_YELLOW,                                                           \
                COLOR_DEFAULT );                                                        \
        size_t run_test_test_name_str_len = strlen( #CONDITION );                       \
        bool run_test_success             = CONDITION;                                  \
        const size_t LIMIT_ =                                                           \
                MIN( 300 - ( 32 + run_test_test_name_str_len ),                         \
                     MAX( 0,                                                            \
                          COLUMN_MAX_LENGHT - ( 32 + run_test_test_name_str_len ) ) ) + \
                1;                                                                      \
        for ( size_t run_test_idx = 0; run_test_idx < LIMIT_; ++run_test_idx )          \
            printf( "." );                                                              \
        printf( " " PRINT_COLOR "%s" PRINT_COLOR "\n",                                  \
                run_test_success ? COLOR_GREEN : COLOR_RED,                             \
                run_test_success ? "SUCCESS" : "FAILURE",                               \
                COLOR_DEFAULT );                                                        \
        if ( !run_test_success )                                                        \
            ++TEST_failed_total;                                                        \
        ++TEST_ran_total;                                                               \
    }                                                                                   \
    while ( 0 )

#endif //CLIBS_RUN_TEST_H
