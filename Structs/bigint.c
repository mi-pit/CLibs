//
// Created by MacBook on 09.01.2025.
//

#include "bigint.h"

#include "../Errors/errors.h"
#include "../foreach.h"
#include "../misc.h"
#include "../pointer_utils.h"
#include "../string_utils.h"
#include "dynstring.h"

#include <assert.h>
#include <inttypes.h>


Private inline sign_t sign_flip( sign_t s )
{
    return s == SIGN_NEG ? SIGN_POS : SIGN_NEG;
}


int bigint_init_p( struct bigint *bi )
{
    bi->numbers = list_init_type( uint64_t );
    if ( bi->numbers == NULL )
    {
        f_stack_trace();
        return RV_ERROR;
    }
    bi->sign = SIGN_POS;

    const uint64_t ZERO = 0;
    assert( list_append( bi->numbers, &ZERO ) == RV_SUCCESS );
    // Cap must be at least one so no realloc -- therefore this shouldn't be able to fail

    return RV_SUCCESS;
}

struct bigint *bigint_init()
{
    struct bigint *new = new ( struct bigint );
    if ( new == NULL )
        return fwarn_ret_p( NULL, "calloc" );

    bigint_init_p( new );

    return new;
}

struct bigint *bigint_init_as( const int64_t n )
{
    struct bigint *new = bigint_init();
    uint64_t u_n       = n > 0 ? n : ( uint64_t ) llabs( ( long long ) n );
    assert( list_set_at( new->numbers, 0, &u_n ) == RV_SUCCESS );

    sign_t new_sign = sgn_64( n );
    if ( new_sign == 0 )
        new_sign = SIGN_POS;
    new->sign = new_sign;

    return new;
}

str_t mul_uint_strings( string_t str_1, string_t str_2 )
{
    typedef unsigned char digit_t;

    size_t len_1      = strlen( str_1 );
    size_t len_2      = strlen( str_2 );
    size_t result_len = len_1 + len_2;

    // Allocate memory for the result (zero-initialized)
    char *result = calloc( result_len + 1, 1 );
    if ( !result )
        return fwarn_ret_p( NULL, "calloc" );
    memset( result, '0', result_len );

    // Perform multiplication using the "long multiplication" algorithm
    for ( ssize_t i = ( ssize_t ) len_1 - 1; i >= 0; --i )
    {
        digit_t carry = 0;
        for ( ssize_t j = ( ssize_t ) len_2 - 1; j >= 0; --j )
        {
            digit_t prod = ( str_1[ i ] - '0' ) * ( str_2[ j ] - '0' ) + carry
                           + ( result[ i + j + 1 ] - '0' );
            result[ i + j + 1 ] = ( char ) ( ( prod % 10 ) + '0' ); // Store the digit
            carry               = prod / 10;                        // Carry over
        }
        result[ i ] = ( char ) ( result[ i ] + carry ); // Add any remaining carry
    }

    // Remove leading zeros
    size_t start = 0;
    while ( start < result_len && result[ start ] == '0' )
        ++start;

    // If the result is all zeros (e.g., "0 * 0"), return "0"
    if ( start == result_len )
    {
        free( result );
        return strdup( "0" );
    }

    // Shift the result to remove leading zeros
    char *final_result = strdup( result + start );
    free( result );

    return final_result;
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


#define STRING_2_TO_64 "18446744073709551616"

#define N_POWER64_STRINGS 8

/// Lookup table for `get_listelem_power_str()`
Private string_t const POWER64_STRINGS[ N_POWER64_STRINGS ] = {
    "1",
    STRING_2_TO_64,
    "340282366920938463463374607431768211456",
    "6277101735386680763835789423207666416102355444464034512896",
    "115792089237316195423570985008687907853269984665640564039457584007913129639936",
    "213598703592091008239502170616955211460270452235665276994704160782221972578064055002"
    "2962086936576",
    "394020061963944792122790401001436138050797392704654466679482934042457217714972106114"
    "14266254884915640806627990306816",
    "726838724295606890549323807888004534353641360687318060281490199180639288113397923326"
    "191050713763565560762521606266177933534601628614656"
};

static str_t get_listelem_power_str( uint64_t n, size_t power )
{
    str_t buffer;
    if ( power < N_POWER64_STRINGS )
    {
        buffer = strdup( POWER64_STRINGS[ power ] );
        if ( buffer == NULL )
            return fwarn_ret_p( NULL, "strdup" );
    }
    else
    {
        buffer = strdup( POWER64_STRINGS[ N_POWER64_STRINGS - 1 ] );
        if ( buffer == NULL )
            return fwarn_ret_p( NULL, "strdup" );

        for ( size_t i = N_POWER64_STRINGS - 1; i < power; ++i )
        {
            str_t tmp = mul_uint_strings( buffer, STRING_2_TO_64 );
            free( buffer );
            if ( tmp == NULL )
            {
                f_stack_trace();
                return NULL;
            }
            buffer = tmp;
        }
    }

    str_t n_str;
    if ( asprintf( &n_str, "%" PRIu64, n ) == -1 )
        return fwarn_ret_p( NULL, "asprintf" );

    str_t result = mul_uint_strings( buffer, n_str );
    free( buffer );
    free( n_str );

    return result;
}

str_t bigint_to_string( const struct bigint *const bi )
{
    str_t sum;
    foreach_ls( uint64_t, n, bi->numbers )
    {
        if ( foreach_index_n == 0 )
        {
            // init
            if ( asprintf( &sum, "%" PRIu64, n ) == -1 )
                return fwarn_ret_p( NULL, "asprintf" );

            continue;
        }

        str_t buffer = get_listelem_power_str( n, foreach_index_n );
        if ( buffer == NULL )
        {
            f_stack_trace();
            free( sum );
            return NULL;
        }

        str_t tmp = add_uint_strings( sum, buffer );
        free( buffer );
        free( sum );
        if ( tmp == NULL )
        {
            f_stack_trace();
            return NULL;
        }
        sum = tmp;
    }

    if ( bi->sign == SIGN_NEG )
    {
        str_t tmp;
        asprintf( &tmp, "-%s", sum );
        free( sum );

        return tmp;
    }

    return sum;
}

List *bigint_get_number_array( const struct bigint *bi )
{
    List *ls;
    int rv = list_copy( bi->numbers, &ls );
    on_fail( rv )
    {
        f_stack_trace();
        return NULL;
    }
    return ls;
}


void bigint_destroy_l( struct bigint bi )
{
    list_destroy( bi.numbers );
}

void bigint_destroy( struct bigint *bp )
{
    list_destroy( bp->numbers );
    free( bp );
}


Private int bigint_add_level( struct bigint *const bi,
                              const int64_t n,
                              const size_t level )
{
    if ( n == 0 )
        return RV_SUCCESS;

    uint64_t low = list_access( bi->numbers, 0, uint64_t );

    sign_t n_sign = sgn_64( n );
    assert( n_sign == 1 || n_sign == -1 );

    int64_t norm_n = bi->sign * n;
    bool overflow  = INT64_MAX < low || ( int64_t ) low > norm_n;
    //TODO:
    // bool underflow = norm_n < 0 && low < -norm_n;
    if ( overflow )
    {
        if ( list_size( bi->numbers ) == level + 1 )
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
            uint64_t old = list_access( bi->numbers, level + 1, uint64_t );
            list_access( bi->numbers, level + 1, uint64_t ) += 1;

            if ( old == UINT64_MAX )
                return_on_fail( bigint_add_level( bi, 1, level + 1 ) );
        }
    }
    low += norm_n;
    assert( list_set_at( bi->numbers, 0, &low ) == RV_SUCCESS );
    return RV_SUCCESS;
}

int bigint_add_i( struct bigint *bi, int64_t n )
{
    return bigint_add_level( bi, n, 0 );
}
