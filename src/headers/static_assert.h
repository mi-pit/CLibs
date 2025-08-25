#ifndef STATIC_ASSERT_H
#define STATIC_ASSERT_H

#include "assert_that.h"
#include "attributes.h"

#if __STDC_VERSION__ >= 201112L

/// C11's static assert
#define STATIC_ASSERT( cond, msg ) _Static_assert( cond, msg )

#elif HAS_ATTRIBUTE( constructor ) // BeforeMain

/**
 * @warning Not **actually** static,
 * just that the asserts happen before `main` is entered.
 *
 * Otherwise, this should function similarly to C11+ `_Static_assert` keyword.
 *
 * If the version
 */
#define STATIC_ASSERT( cond, msg )                       \
    BeforeMain void _Static_assert_##__COUNTER__( void ) \
    {                                                    \
        assert_that( cond, msg );                        \
    }

#else

#error "Can't use static assert. To use, either: -set C standard to C11 or newer" \
    "-use a compiler which supports __attribute__((constructor))"

#endif // C11

#endif //STATIC_ASSERT_H
