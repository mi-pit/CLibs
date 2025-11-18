/*
 * Assert with a message
 *
 * Created by MacBook on 25.12.2024.
 */

#ifndef CLIBS_ASSERT_THAT_H
#define CLIBS_ASSERT_THAT_H

/* for this header */
#include "errors.h" /* warn */

#include <stdlib.h> /* exit */

/* includes */
#include <assert.h>

#ifndef CLIBS_ASSERT_THAT_EXIT_VAL
#define CLIBS_ASSERT_THAT_EXIT_VAL RV_EXCEPTION
#endif // CLIBS_ASSERT_THAT_EXIT_VAL


/**
 * @brief Asserts a given expression and exits with an error message if the assertion fails.
 *
 * @param EXPRESSION    The condition to be evaluated. If false, the assertion fails.
 * @param ...           Additional arguments for the formatted error message (format string + varargs).
 *                      These are passed to `fflwarnx_ret` for constructing the error message.
 *
 * Example:
 * @code
 * int x = 5;
 * assert_that(x > 10, "x must be greater than 10, but got %d", x);
 * @endcode
 *
 * In this example, if `x > 10` evaluates to false,
 * the program will exit and print an error message
 * (the expression is printed in `COLOR_DEFAULT`, the rest in `COLOR_WARNING`):
 * @code
 * "Assertion error: x > 10: x must be greater than 10, but got 5".
 * @endcode
 *
 * @note
 * The error code returned is – by default – `RV_EXCEPTION` from `errors.h`,
 * but may be redefined. Either
 * @code
 * // like this:
 * #define CLIBS_ASSERT_THAT_EXIT_VAL 123
 * #include "assert_that.h"
 * @endcode
 * Or `-DCLIBS_ASSERT_THAT_EXIT_VAL=123` during compilation
 */
#define assert_that( EXPRESSION, ... )                                          \
    do                                                                          \
    {                                                                           \
        if ( !( EXPRESSION ) )                                                  \
            exit( ( int ) fflwarnx_ret(                                         \
                    CLIBS_ASSERT_THAT_EXIT_VAL,                                 \
                    "Assertion error: " COLOR_DEFAULT #EXPRESSION COLOR_WARNING \
                    ": " __VA_ARGS__ ) );                                       \
    }                                                                           \
    while ( 0 )

#endif //CLIBS_ASSERT_THAT_H
