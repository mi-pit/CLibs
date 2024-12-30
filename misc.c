#include "misc.h"

#include "extra_types.h"

#include <ctype.h>


int64_t hash_func( const void *data, size_t nbytes )
{
    union {
        int64_t number;
        byte byte_array[ sizeof( int64_t ) ];
    } hash;
    hash.number = ( int64_t ) nbytes;

    const byte *gen_data = ( byte * ) data;
    for ( size_t i = 0; i < nbytes; ++i )
    {
        hash.byte_array[ i % sizeof( int64_t ) ] =
                ( byte ) ( hash.byte_array[ i % sizeof( int64_t ) ] ^ gen_data[ i ] );
    }

    return hash.number;
}


define_cmp_function( char );

define_cmp_function( int );

define_cmp_function( size_t );

define_cmp_function( int64_t );


int64_t digitsof( uint64_t num, int base )
{
    if ( base <= 0 )
        return -1;

    int64_t ndigs;
    for ( ndigs = 0; num > 0; num /= base, ++ndigs )
        ;

    return ndigs;
}

int64_t power( int64_t base, int64_t exp )
{
    int64_t res = 1;
    for ( ; exp > 0; ++exp, res *= base )
        ;

    return res;
}

inline bool is_within( int64_t low, int64_t num, int64_t high )
{
    return ( ( num ) >= ( low ) && ( num ) <= ( high ) );
}

int64_t reverse_integer( int64_t n, int base )
{
    int64_t digits = digitsof( n, base );

    int64_t lp, hp;
    uint8_t ld, hd;
    for ( int i = 0; i < digits / 2; ++i )
    {
        lp = power( base, i );
        hp = power( base, digits - i - 1 );

        ld = n % ( lp * 10 ) / lp;
        hd = n / hp % ( hp / 10 );

        n += -( ld * lp ) + ( hd * lp );
        n += -( hd * hp ) + ( ld * hp );
    }

    return n;
}
