//
// Created by Michal Pitner on 13.06.2025.
//

#ifndef CLIBS_MATH_H
#define CLIBS_MATH_H

#include "Dev/attributes.h"
#include "Dev/extra_types.h"

// import
#include "Dev/simple_math.h"


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
Mathematical uint64_t reverse_integer( uint64_t n, unsigned base );

Mathematical uint64_t get_next_power_of_two( uint64_t );
Mathematical uint64_t get_prev_power_of_two( uint64_t );


#endif //CLIBS_MATH_H
