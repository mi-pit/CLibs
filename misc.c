#include "misc.h"

#include "Dev/assert_that.h" /* assert_that */
#include "extra_types.h"


uint64_t hash_func( const void *data, size_t nbytes )
{
    uint64_t hash = 5381 * nbytes;

    const byte *data_byte = ( byte * ) data;
    for ( size_t i = 0; i < nbytes; ++i )
        hash = ( ( hash << 5 ) + hash ) + data_byte[ i ];

    return hash;
}


uint64_t digitsof( uint64_t num, unsigned base )
{
    uint64_t ndigs = 0;
    for ( ; num > 0; num /= base, ++ndigs )
        if ( ndigs == UINT64_MAX )
            assert_that( false, "integer overflow shouldn't be possible here" );

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

uint64_t reverse_integer( uint64_t n, unsigned base )
{
    uint64_t digits = digitsof( n, base );

    int64_t lp = 1, hp = power( base, digits - 1 );
    uint8_t ld, hd;
    for ( uint64_t i = 0; i < digits / 2; ++i )
    {
        ld = n / lp % base;
        hd = n / hp % base;

        n = n - ( ld * lp ) + ( hd * lp );
        n = n - ( hd * hp ) + ( ld * hp );

        lp *= base;
        hp /= base;
    }

    return n;
}
