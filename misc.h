#ifndef CLIBS_MISC_H
#define CLIBS_MISC_H

#include "Dev/attributes.h" /* LibraryDefined */
#include "extra_types.h"    /* byte, (u)int64_t */

#include <stdbool.h> /* bool (is_within) */
#include <stddef.h>  /* size_t */

/**
 * Stops those pesky `unused variable` compiler errors
 */
#define UNUSED( ... ) ( ( void ) ( ( void ) __VA_ARGS__ ) )


typedef enum {
    SIGN_NEG = -1,
    SIGN_POS = 1,
} sign_t;


/**
 * @return 1 if sign was -1 and vice versa
 * @see @code sign_t @endcode
 */
#define sign_flipped( SIGN ) ( -( SIGN ) )

/**
 * Sign ( -1, 0, 1 ) of the number
 */
LibraryDefined Mathematical inline signed int sgn_64( int64_t n )
{
    return n == 0 ? 0 : n < 0 ? -1 : 1;
}

/**
 * Evaluates to the smaller of the two numbers.
 * Works with both integral and floating-point types with some caveats (see below).
 * @attention constructs with the ++ or -- operators like
 * @code
 * int a = 10, b = 10;
 * int c = min_m( a++, b );
 * @endcode
 * are undefined for this macro.
 * <p>
 * This isn't the case for the (min|max)_[ui]64 functions
 */
#define min_m( NUM_A, NUM_B ) ( ( NUM_A ) < ( NUM_B ) ? ( NUM_A ) : ( NUM_B ) )

/**
 * Evaluates to the bigger of the two numbers.
 * Works with both integral and floating-point types with some caveats (see below).
 * @attention constructs with the ++ or -- operators like
 * @code
 * int a = 10, b = 10;
 * int c = max_m( a++, b );
 * @endcode
 * are undefined for this macro.
 * <p>
 * This isn't the case for the (min|max)_[ui]64 functions
 */
#define max_m( NUM_A, NUM_B ) ( ( NUM_A ) > ( NUM_B ) ? ( NUM_A ) : ( NUM_B ) )

LibraryDefined Mathematical inline int64_t min_i64( int64_t a, int64_t b )
{
    return min_m( a, b );
}
LibraryDefined Mathematical inline uint64_t min_u64( uint64_t a, uint64_t b )
{
    return min_m( a, b );
}

LibraryDefined Mathematical inline int64_t max_i64( int64_t a, int64_t b )
{
    return max_m( a, b );
}
LibraryDefined Mathematical inline uint64_t max_u64( uint64_t a, uint64_t b )
{
    return max_m( a, b );
}


/**
 * Evaluates as true the cmp function as true if the elements are equal
 */
#define cmpeq( cmp_retval ) ( ( cmp_retval ) == 0 )

/**
 * Declares a cmp function of type `TYPE`.
 * `TYPE` has to be a single token since it's used to define the name of the function
 */
#define DECLARE_CMP_FUNCTION( TYPE ) int cmp_##TYPE( const void *p1, const void *p2 )

/**
 * Creates a new comparator function for already comparable numeric types.
 * `TYPE` has to be a single token since it's used to define the name of the function.
 */
#define DEFINE_CMP_FUNCTION( TYPE )                                 \
    LibraryDefined int cmp_##TYPE( const void *p1, const void *p2 ) \
    {                                                               \
        TYPE val_1 = *( TYPE * ) p1;                                \
        TYPE val_2 = *( TYPE * ) p2;                                \
                                                                    \
        if ( val_1 > val_2 )                                        \
            return 1;                                               \
        if ( val_1 < val_2 )                                        \
            return -1;                                              \
        return 0;                                                   \
    }

DEFINE_CMP_FUNCTION( byte )

DEFINE_CMP_FUNCTION( char )

DEFINE_CMP_FUNCTION( int )

DEFINE_CMP_FUNCTION( size_t )

DEFINE_CMP_FUNCTION( int64_t )

DEFINE_CMP_FUNCTION( uint64_t )


/**
 * Creates an arbitrary hash of any data of the specified length length
 * @param len number of bytes (chars) under the pointer
 * @return unsigned 64-bit int hash
 */
uint64_t hash_func( const void *, size_t len );


#define countof( array ) ( sizeof( array ) / sizeof *( array ) )

/**
 * Tests if ‹num› is within the specified bounds\n
 * ‹low› should obviously be smaller than ‹high›
 *
 * @param low   lower bound
 * @param num   tested number
 * @param high  upper bound
 * @return true if ‹ low \<= num \<= high ›
 */
LibraryDefined inline bool is_within( int64_t low, int64_t num, int64_t high )
{
    return ( ( num ) >= ( low ) && ( num ) <= ( high ) );
}


/**
 * Returns how many digits ‹num› has in a specified base
 *
 * @param num   Any natural number
 * @param base  Whole number bigger than 0
 * @return      Number of digits. For (num = 0) the function returns 0; if base &lt 0,
 * the function returns -1
 */
Mathematical uint64_t digitsof( uint64_t num, unsigned base );

/**
 * base ^ (to the power of) exp
 * @param base
 * @param exp
 * @return base ^ exp
 */
Mathematical int64_t power( int64_t base, unsigned exp );
Mathematical uint64_t upower( uint64_t base, unsigned exp );

/**
 * Reverses the digits of the number `n`.
 * Ignores lead zeroes.
 * Works in any base.
 *
 * @example
 * @code
 * reverse_integer( 123, 10 )   // => 321
 * reverse_integer( 0xFA, 16 )  // => 0xAF
 * reverse_integer( 0b0101, 2 ) // => 0b101
 * @endcode
 * @param n     number to be reversed
 * @param base  numeric base of the number
 * @return digit-wise reverse of the original
 */
uint64_t reverse_integer( uint64_t n, unsigned base );

#endif //CLIBS_MISC_H
