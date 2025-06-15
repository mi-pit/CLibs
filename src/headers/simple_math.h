//
// Created by Michal Pitner on 13.06.2025.
//

#ifndef SIMPLE_MATH_H
#define SIMPLE_MATH_H

#include "attributes.h"
#include "extra_types.h"


typedef enum {
    SIGN_NEG = -1,
    SIGN_POS = 1,
} sign_t;


/**
 * @return 1 if sign was -1 and vice versa
 * @see @code sign_t@endcode
 */
#define sign_flipped( SIGN ) ( -( SIGN ) )


/**
 * Sign ( -1, 0, 1 ) of the number
 */
LibraryDefined Mathematical inline signed int sgn_64( const int64_t n )
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


LibraryDefined Mathematical inline int64_t min_i64( const int64_t a, const int64_t b )
{
    return min_m( a, b );
}
LibraryDefined Mathematical inline uint64_t min_u64( const uint64_t a, const uint64_t b )
{
    return min_m( a, b );
}

LibraryDefined Mathematical inline int64_t max_i64( const int64_t a, const int64_t b )
{
    return max_m( a, b );
}
LibraryDefined Mathematical inline uint64_t max_u64( const uint64_t a, const uint64_t b )
{
    return max_m( a, b );
}


/**
 * Tests if ‹num› is within the specified bounds\n
 * ‹low› should obviously be smaller than ‹high›
 *
 * @param low   lower bound
 * @param num   tested number
 * @param high  upper bound
 * @return true if ‹ low <= num <= high ›
 */
LibraryDefined Mathematical inline bool is_within( const int64_t low, const int64_t num,
                                                   const int64_t high )
{
    return num >= low && num <= high;
}


#endif //SIMPLE_MATH_H
