//
// Created by MacBook on 09.01.2025.
//

#include "bigint.h"

#include "../errors.h"
#include "../foreach.h"
#include "../misc.h"
#include "../string_utils.h"
#include "dynstring.h"

#include <assert.h>
#include <inttypes.h>


__unused static inline sign_t sign_flip( sign_t s )
{
    return s == SIGN_NEG ? SIGN_POS : SIGN_NEG;
}


struct bigint *bigint_init()
{
    struct bigint *new = calloc( 1, sizeof( struct bigint ) );
    if ( new == NULL )
        return fwarn_ret_p( NULL, "calloc" );

    new->numbers = list_init_type( uint64_t );
    if ( new->numbers == NULL )
    {
        f_stack_trace();
        free( new );
        return NULL;
    }
    new->sign = SIGN_POS;

    const uint64_t ZERO = 0;
    assert( list_append( new->numbers, &ZERO ) == RV_SUCCESS );
    // Cap must be at least one so no realloc -- therefore this shouldn't be able to fail

    return new;
}

struct bigint *bigint_init_as( const int64_t n )
{
    struct bigint *new = bigint_init();
    uint64_t u_n       = n > 0 ? n : abs_64( n );
    if ( list_append( new->numbers, &u_n ) != RV_SUCCESS )
    {
        f_stack_trace();
        free( new );
        return NULL;
    }
    new->sign = sgn_64( n );
    if ( new->sign == 0 )
        new->sign = SIGN_POS;
    return new;
}


str_t add_uint_strings( string_t str_1, string_t str_2 )
{
    typedef uint8_t digit_t;

    DynamicString result = dynstr_init();
    if ( result == NULL )
    {
        f_stack_trace();
        return NULL;
    }

    size_t str_1_len = strlen( str_1 );
    size_t str_2_len = strlen( str_2 );

    str_t ret = NULL;

    digit_t carry = 0;
    for ( size_t idx = 0;
          idx < ( size_t ) max_64( ( int64_t ) str_1_len, ( int64_t ) str_2_len );
          ++idx )
    {
        digit_t str_1_digit = idx < str_1_len ? str_1[ str_1_len - idx - 1 ] - '0' : 0;
        digit_t str_2_digit = idx < str_2_len ? str_2[ str_2_len - idx - 1 ] - '0' : 0;

        digit_t val = ( str_1_digit + str_2_digit + carry );

        carry       = val >= 10;
        digit_t dig = val % 10;

        char app = ( char ) ( dig + '0' );
        if ( dynstr_appendn( result, &app, 1 ) != RV_SUCCESS )
        {
            f_stack_trace();
            goto CLEANUP;
        }
    }
    if ( carry )
    {
        char app = '1';
        if ( dynstr_appendn( result, &app, 1 ) != RV_SUCCESS )
        {
            f_stack_trace();
            goto CLEANUP;
        }
    }

    ret = string_reversed( dynstr_data( result ) );

CLEANUP:
    dynstr_destroy( result );
    return ret;
}

#define UINT64_MAX_STRING "18446744073709551615"

str_t bigint_to_string( const struct bigint *const bi )
{
    str_t sum = malloc( 2 );
    sum[ 0 ]  = '0';
    sum[ 1 ]  = '\0';

    foreach_ls( uint64_t, n, bi->numbers )
    {
        char buffer[ sizeof UINT64_MAX_STRING * 2 ] = { 0 };

        if ( foreach_index_n == list_size( bi->numbers ) - 1 )
            // last number is biggest, therefore has no leading zeroes
            snprintf( buffer, sizeof UINT64_MAX_STRING, "%" PRIu64, n );
        else
        {
            snprintf( buffer, sizeof UINT64_MAX_STRING, "%020" PRIu64, n );
            str_t tmp = add_uint_strings( buffer, UINT64_MAX_STRING );
            memcpy( buffer, tmp, strlen( tmp ) );
            free( tmp );
        }

        str_t tmp = add_uint_strings( sum, buffer );
        free( sum );
        sum = tmp;
        if ( sum == NULL )
        {
            f_stack_trace();
            free( sum );
            return NULL;
        }
    }

    if ( bi->sign == SIGN_NEG )
    {
        size_t new_strlen = 1 + strlen( sum );
        str_t tmp         = malloc( new_strlen + 1 );
        tmp[ 0 ]          = '-';
        strcpy( tmp + 1, sum );
        tmp[ new_strlen ] = '\0';

        free( sum );

        return tmp;
    }

    return sum;
}


void bigint_destroy( struct bigint *bp )
{
    list_destroy( bp->numbers );
    free( bp );
}


int bigint_add_i( struct bigint *bi, int64_t n )
{
    if ( n == 0 )
        return RV_SUCCESS;

    uint64_t low = list_access( bi->numbers, 0, uint64_t );

    sign_t n_sign = sgn_64( n );
    assert( n_sign == 1 || n_sign == -1 );

    int64_t norm_n = bi->sign * n;
    bool overflow  = low > INT64_MAX || ( int64_t ) low > norm_n;
    //bool underflow = norm_n < 0 && low < -norm_n;
    if ( overflow )
    {
        if ( list_size( bi->numbers ) == 1 )
        {
            uint64_t high = 1;
            if ( list_append( bi->numbers, &high ) != RV_SUCCESS )
            {
                f_stack_trace();
                return RV_ERROR;
            }
        }
        else
        {
            *( uint64_t * ) list_at( bi->numbers, 1 ) += 1;
        }
    }
    low += norm_n;
    assert( list_set_at( bi->numbers, 0, &low ) == RV_SUCCESS );
    return RV_SUCCESS;
}
