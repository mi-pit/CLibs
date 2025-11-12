/**
 * Math functions & other number operations.
 */

#ifndef CLIBS_MATH_H
#define CLIBS_MATH_H

#include "headers/attributes.h"
#include "headers/types.h"

// import
#include "headers/simple_math.h"


/**
 * Returns how many digits ‹num› has in a specified base
 *
 * @param num   Any natural number
 * @param base  Whole number bigger than 0
 * @return      Number of digits. For (num = 0) the function returns 0; if base < 0,
 * the function returns -1
 */
Mathematical uint64_t digitsof( uint64_t num, unsigned base );

/**
 * base ^ (to the power of) exp
 * @param base number
 * @param exp  exponent
 * @return base ^ exp
 */
Mathematical int64_t power( int64_t base, unsigned exp );
/**
 * base ^ (to the power of) exp
 * @param base unsigned number
 * @param exp  exponent
 * @return base ^ exp
 */
Mathematical uint64_t upower( uint64_t base, unsigned exp );


/**
 * Reverses the digits of the number `n`.
 * Ignores lead zeroes, works in any base.
 *
 * Example:
 * @code
 * reverse_integer( 123, 10 )   // => 321
 * reverse_integer( 0xFA, 16 )  // => 0xAF
 * reverse_integer( 0b0101, 2 ) // => 0b101
 * @endcode
 *
 * @param n     number to be reversed
 * @param base  numeric base of the number
 * @return digit-wise reverse of the original
 */
Mathematical uint64_t reverse_integer( uint64_t n, unsigned base );

/// @return smallest power of two bigger than the parameter
Mathematical uint64_t get_next_power_of_two( uint64_t );
/// @return biggest power of two smaller than the parameter
Mathematical uint64_t get_prev_power_of_two( uint64_t );


#endif //CLIBS_MATH_H
