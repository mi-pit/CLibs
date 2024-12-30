#ifndef CLIBS_MISC_H
#define CLIBS_MISC_H

#include "extra_types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UNUSED( expr ) ( void ) ( expr )


#define cmpeq( cmp_retval ) ( ( cmp_retval ) == 0 )


#define declare_cmp_function( type ) int cmp_##type( const void *p1, const void *p2 )

declare_cmp_function( char );
declare_cmp_function( int );
declare_cmp_function( size_t );
declare_cmp_function( int64_t );

#ifndef REQUIRE_SEMICOLON
#define REQUIRE_SEMICOLON struct DECLARATION_MACRO_UNUSED_STRUCT
#endif //REQUIRE_SEMICOLON

/**
 * Creates a new comparator function for already comparable numeric types
 */
#define define_cmp_function( type )                  \
    int cmp_##type( const void *p1, const void *p2 ) \
    {                                                \
        type val_1 = *( type * ) p1;                 \
        type val_2 = *( type * ) p2;                 \
                                                     \
        if ( val_1 > val_2 )                         \
            return 1;                                \
        if ( val_1 < val_2 )                         \
            return -1;                               \
        return 0;                                    \
    }                                                \
    REQUIRE_SEMICOLON


int64_t hash_func( const void *, size_t );


/**
 * Size of stack allocated array<br>
 * Apparently might be officially added to C
 */
#define sizeof_arr( array ) ( sizeof( array ) / sizeof *( array ) )

/**
 * Tests if ‹num› is within the specified bounds\n
 * ‹low› should obviously be smaller than ‹high›
 *
 * @param low   lower bound
 * @param num   tested number
 * @param high  upper bound
 * @return true if ‹ low \<= num \<= high ›
 */
bool is_within( int64_t low, int64_t num, int64_t high );

/**
 * Smaller of the two numbers
 */
#define min_m( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )

/**
 * Bigger of the two numbers
 */
#define max_m( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )

/**
 * Sign ( -1, 0, 1 ) of the number
 */
#define sgn_m( n ) ( n == 0 ? 0 : n < 0 ? -1 : 1 )

/**
 * Returns how many digits ‹num› has in base ‹base›
 *
 * @param num   Any natural number
 * @param base  Whole number bigger than 0
 * @return      Number of digits. For (num = 0) the function returns 0; if base < 0,
 * the function returns -1
 */
int64_t digitsof( uint64_t num, int base );
int64_t power( int64_t base, int64_t exp );

int64_t reverse_integer( int64_t n, int base );

#endif //CLIBS_MISC_H
