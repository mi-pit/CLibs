//
// Created by Michal Pitner on 13.06.2025.
//

#include "math.h"


uint64_t digitsof( uint64_t num, const unsigned base )
{
    uint64_t ndigs = 0;
    for ( ; num > 0; num /= base, ++ndigs )
        ;

    return ndigs;
}

int64_t power( int64_t base, unsigned exp )
{
    int64_t res = 1;
    while ( exp > 0 )
    {
        if ( exp % 2 == 1 )
            res *= base;
        base *= base;
        exp /= 2;
    }

    return res;
}

uint64_t upower( uint64_t base, unsigned exp )
{
    uint64_t res = 1;
    while ( exp > 0 )
    {
        if ( exp % 2 == 1 )
            res *= base;
        base *= base;
        exp /= 2;
    }

    return res;
}

uint64_t reverse_integer( uint64_t n, const unsigned base )
{
    const uint64_t digits = n == 0 ? 0 : digitsof( n, base );
    uint64_t lp = 1, hp = upower( base, digits - 1 ); // powers
    for ( uint64_t i = 0; i < digits / 2; ++i )
    {
        const uint8_t ld = n / lp % base;
        const uint8_t hd = n / hp % base;

        n = n - ld * lp + hd * lp;
        n = n - hd * hp + ld * hp;

        lp *= base;
        hp /= base;
    }

    return n;
}

uint64_t get_next_power_of_two( const uint64_t n )
{
    uint64_t pwr = 1;
    while ( pwr <= n )
        pwr *= 2;

    return pwr;
}

uint64_t get_prev_power_of_two( const uint64_t n )
{
    uint64_t pwr = INT64_MAX + UINT64_C( 1 );
    while ( pwr >= n )
        pwr /= 2;

    return pwr;
}
