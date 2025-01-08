#include "misc.h"

#include "extra_types.h"

#include <ctype.h>


uint64_t hash_func( const void *data, size_t nbytes )
{
    uint64_t hash = 5381 * nbytes;

    const byte *gen_data = ( byte * ) data;
    for ( size_t i = 0; i < nbytes; ++i )
        hash = ( ( hash << 5 ) + hash ) + gen_data[ i ];

    return hash;
}


DEFINE_CMP_FUNCTION( char );

DEFINE_CMP_FUNCTION( int );

DEFINE_CMP_FUNCTION( size_t );

DEFINE_CMP_FUNCTION( int64_t );

int64_t min_64( int64_t a, int64_t b )
{
    return ( ( a ) < ( b ) ? ( a ) : ( b ) );
}

int64_t max_64( int64_t a, int64_t b )
{
    return ( ( a ) > ( b ) ? ( a ) : ( b ) );
}


int64_t digitsof( uint64_t num, unsigned base )
{
    int64_t ndigs = 0;
    for ( ; num > 0; num /= base, ++ndigs )
        ;

    return ndigs;
}

int64_t power( int64_t base, uint64_t exp )
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

inline bool is_within( int64_t low, int64_t num, int64_t high )
{
    return ( ( num ) >= ( low ) && ( num ) <= ( high ) );
}

int64_t reverse_integer( int64_t n, unsigned base )
{
    int64_t digits = digitsof( n, base );

    int64_t lp, hp;
    uint8_t ld, hd;
    for ( int i = 0; i < digits / 2; ++i )
    {
        lp = power( base, i );
        hp = power( base, digits - i - 1 );

        ld = n % ( lp * base ) / lp;
        hd = n / hp % ( hp / base );

        n = n - ( ld * lp ) + ( hd * lp );
        n = n - ( hd * hp ) + ( ld * hp );
    }

    return n;
}
