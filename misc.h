#ifndef CLIBS_MISC_H
#define CLIBS_MISC_H

#include "Dev/attributes.h" /* LibraryDefined */

#include <stdbool.h> /* bool (is_within) */
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* [u]?int64_t */

/**
 * Stops those pesky `unused variable` compiler errors
 */
#define UNUSED( ... ) ( ( void ) ( ( void ) __VA_ARGS__ ) ) /* Why does this work? */


typedef enum {
    SIGN_NEG = -1,
    SIGN_POS = 1,
} sign_t;


/**
 * @return 1 if sign was -1 and vice versa
 * @see @code sign_t @endcode
 */
#define sign_flipped( SIGN ) ( -SIGN )

/**
 * Sign ( -1, 0, 1 ) of the number
 */
LibraryDefined Mathematical inline int64_t sgn_64( int64_t n )
{
    return n == 0 ? 0 : n < 0 ? -1 : 1;
}

/**
 * Smaller of the two numbers
 */
LibraryDefined Mathematical inline int64_t min_64( int64_t a, int64_t b )
{
    return ( ( a ) < ( b ) ? ( a ) : ( b ) );
}

/**
 * Bigger of the two numbers
 */
LibraryDefined Mathematical inline int64_t max_64( int64_t a, int64_t b )
{
    return ( ( a ) > ( b ) ? ( a ) : ( b ) );
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

DECLARE_CMP_FUNCTION( char );
DECLARE_CMP_FUNCTION( int );
DECLARE_CMP_FUNCTION( size_t );
DECLARE_CMP_FUNCTION( int64_t );

#ifndef REQUIRE_SEMICOLON
/**
 * Declares an unused (and undefined) struct for the sake of requiring semicolons
 * at the ends of macros
 */
#define REQUIRE_SEMICOLON struct DECLARATION_MACRO_UNUSED_STRUCT
#endif //REQUIRE_SEMICOLON

/**
 * Creates a new comparator function for already comparable numeric types.
 * `TYPE` has to be a single token since it's used to define the name of the function.
 */
#define DEFINE_CMP_FUNCTION( TYPE )                  \
    int cmp_##TYPE( const void *p1, const void *p2 ) \
    {                                                \
        TYPE val_1 = *( TYPE * ) p1;                 \
        TYPE val_2 = *( TYPE * ) p2;                 \
                                                     \
        if ( val_1 > val_2 )                         \
            return 1;                                \
        if ( val_1 < val_2 )                         \
            return -1;                               \
        return 0;                                    \
    }                                                \
    REQUIRE_SEMICOLON


/**
 * Creates an arbitrary hash of any data of the specified length length
 * @param len number of bytes (chars) under the pointer
 * @return unsigned 64-bit int hash
 */
uint64_t hash_func( const void *, size_t len );


/**
 * Size of stack allocated array<br>
 * Apparently might be officially added to C
 */
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
 * Returns how many digits ‹num› has in base ‹base›
 *
 * @param num   Any natural number
 * @param base  Whole number bigger than 0
 * @return      Number of digits. For (num = 0) the function returns 0; if base &lt 0,
 * the function returns -1
 */
Mathematical int64_t digitsof( uint64_t num, unsigned base );

/**
 * base ^ (to the power of) exp
 * @param base
 * @param exp
 * @return `base` ^ `exp`
 */
Mathematical int64_t power( int64_t base, uint64_t exp );

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
