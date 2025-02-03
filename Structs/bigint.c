//
// Created by MacBook on 09.01.2025.
//

#include "bigint.h"

#include "../Dev/assert_that.h"   /* includes "errors.h", <assert.h> */
#include "../Dev/pointer_utils.h" /* new */
#include "../foreach.h"           /* foreach_ls */
#include "../string_utils.h"      /* str_t, string_reversed() */
#include "./dynstring.h"          /* dynstr */

#include <ctype.h>    /* isdigit() */
#include <inttypes.h> /* PRI macros */


sign_t bigint_sign( const struct bigint *bi )
{
    return bi->sign;
}

void bigint_flip_sign( struct bigint *bi )
{
    bi->sign = sign_flipped( bi->sign );
}

size_t bigint_sizeof( const struct bigint *bi )
{
    return bi->numbers->len;
}


int bigint_init_p( struct bigint *bi )
{
    if ( ( bi->numbers = numls_init() ) == NULL )
        return f_stack_trace( RV_ERROR );

    bi->sign = SIGN_POS;

    assert_that( numls_append( bi->numbers, 0 ) == RV_SUCCESS,
                 "Cap must be at least one so no realloc,"
                 "therefore this shouldn't be able to fail" );

    return RV_SUCCESS;
}

struct bigint *bigint_init( void )
{
    struct bigint *new = new ( struct bigint );
    if ( new == NULL )
        return ( void * ) fwarn_ret( NULL, "calloc" );

    bigint_init_p( new );

    return new;
}

struct bigint *bigint_init_as( const int64_t n )
{
    struct bigint *new = bigint_init();

    const uint64_t u_n         = n > 0 ? n : ( uint64_t ) llabs( ( long long ) n );
    new->numbers->numbers[ 0 ] = u_n;
    new->sign                  = n == 0 ? SIGN_POS : sgn_64( n );

    return new;
}


str_t bigint_to_string( const struct bigint *const bi )
{
    DynamicString dynstr = dynstr_init_cap( bi->numbers->len * sizeof( uint64_t ) * 2 );
    if ( dynstr == NULL )
        return ( void * ) f_stack_trace( NULL );

    ssize_t idx_last = ( ssize_t ) bi->numbers->len - 1;
    for ( ssize_t idx = idx_last; idx >= 0; --idx )
    {
        char fmt_buf[ 1 + 1 + 20 + 3 + 1 ];
        /*            %   0   i   llu \0 */
        /* i is number of digits. Maximum is 20, but the actual value is usually lower */
        snprintf(
                fmt_buf, sizeof fmt_buf, "%%0%i" PRIu64, BIGINT_LIST_MEMBER_MAX_DIGITS );

        uint64_t num = bi->numbers->numbers[ idx ];
        if ( dynstr_appendf( dynstr, ( idx == idx_last ? "%" PRIu64 : fmt_buf ), num )
             != RV_SUCCESS )
            goto ERROR;
    }

    if ( bi->sign == SIGN_NEG )
        dynstr_prependn( dynstr, "-", 1 );

    str_t ret = dynstr_data_copy( dynstr );
    dynstr_destroy( dynstr );
    return ret;

ERROR:
    dynstr_destroy( dynstr );
    return ( void * ) f_stack_trace( NULL );
}


Private bool validate_input_string( string_t str, size_t len )
{
    bool has_digits = false;

    for ( size_t i = 0; i < len; ++i )
    {
        if ( i == 0 && !( isdigit( str[ i ] ) || str[ i ] == '+' || str[ i ] == '-' ) )
            return false;

        if ( i != 0 && !isdigit( str[ i ] ) )
            return false;

        has_digits = isdigit( str[ i ] );
    }

    return has_digits;
}

int bigint_from_string( string_t str, struct bigint **cont )
{
    size_t len = strlen( str );
    if ( !validate_input_string( str, len ) )
        return RV_EXCEPTION;

    struct bigint *bi = bigint_init();
    if ( bi == NULL )
        return f_stack_trace( RV_ERROR );

    if ( str[ 0 ] == '+' || str[ 0 ] == '-' )
    {
        bi->sign = str[ 0 ] == '+' ? SIGN_POS : SIGN_NEG;
        str++;
        len--;
    }

    const size_t chunk_size = BIGINT_LIST_MEMBER_MAX_DIGITS;
    const size_t n_chunks   = ( len / chunk_size ) + 1;

    for ( size_t i = 0; i < len; i += chunk_size )
    {
        unsigned n_lead_zeroes   = 0;
        const uint64_t chunk_idx = n_chunks - i - 1;
        uint64_t app             = 0;
        // TODO? one loop
        for ( size_t j = 0; j < chunk_size; ++j )
        {
            const size_t char_index = len - ( i + j ) - 1;
            if ( char_index >= len )
                break;

            app *= 10;
            app += str[ char_index ] - '0';

            if ( str[ char_index ] == '0' && app == 0 )
                ++n_lead_zeroes;
        }
        app = reverse_integer( app, 10 ) * upower( 10, n_lead_zeroes );

        if ( chunk_idx == n_chunks - 1 )
            bi->numbers->numbers[ 0 ] = app;
        else
            goto_on_fail( ERROR, numls_append( bi->numbers, app ) );
    }

    *cont = bi;
    return RV_SUCCESS;

ERROR:
    bigint_destroy( bi );
    return f_stack_trace( RV_ERROR );
}


void bigint_destroy_l( struct bigint bi )
{
    numls_destroy( bi.numbers );
}

void bigint_destroy( struct bigint *bp )
{
    numls_destroy( bp->numbers );
    free( bp );
}


int bigint_add_i( struct bigint *bi, int64_t n )
{
    return bigint_add_power( bi, n, 0 );
}

Private inline int handle_overunderflow( struct bigint *const bi,
                                         size_t level,
                                         sign_t sign )
{
    if ( bi->numbers->len == level + 1 ) // higher digit not available
        return numls_append( bi->numbers, 1 );

    return bigint_add_power( bi, sign, level + 1 );
}

#define handle_overflow( BIGINT, LEVEL ) \
    handle_overunderflow( ( BIGINT ), ( LEVEL ), ( BIGINT )->sign )

#define handle_underflow( BIGINT, LEVEL ) \
    handle_overunderflow( ( BIGINT ), ( LEVEL ), sign_flipped( ( BIGINT )->sign ) )

int bigint_add_power( struct bigint *const bi, const int64_t n, const uint64_t level )
{
    if ( n == 0 )
        return RV_SUCCESS;

    if ( bi->numbers->len <= level )
        return_on_fail(
                numls_extend_zeroes( bi->numbers, level - bi->numbers->len + 1 ) );

    uint64_t low = bi->numbers->numbers[ level ];

    // int64_t norm_n isn't able to hold this value
    // => subtract it after converting to uint64_t
    bool sub_one_more = sgn_64( n ) == bi->sign && n == INT64_MIN;

    int64_t norm_n = sub_one_more ? INT64_MAX : n * bi->sign;

    bool overflow  = norm_n > 0 && low > BIGINT_LIST_MEMBER_MAX - ( uint64_t ) norm_n;
    bool underflow = norm_n < 0 && low < ( uint64_t ) llabs( norm_n );
    assert_that( !overflow || !underflow, "cannot both be set at once" );

    bool flip_bi_sign = underflow && bi->numbers->len == level + 1;

    const uint64_t final =
            ( flip_bi_sign ? low - norm_n
              : underflow  ? BIGINT_LIST_MEMBER_MAX + 1 + ( ( int64_t ) low + norm_n )
                           : ( low + norm_n ) % ( BIGINT_LIST_MEMBER_MAX + 1 ) )
            - ( int ) sub_one_more;

    bi->numbers->numbers[ level ] = final;

    if ( flip_bi_sign )
        bigint_flip_sign( bi );
    else if ( underflow )
        return_on_fail( handle_underflow( bi, level ) );
    else if ( overflow )
        return_on_fail( handle_overflow( bi, level ) );

    if ( final == 0 && level != 0 && level == bi->numbers->len - 1 )
        return numls_pop( bi->numbers );

    return RV_SUCCESS;
}


int bigint_add_b( struct bigint *bi, const struct bigint *add )
{
    // FIXME: subtraction "underflow"
    for ( size_t i = 0; i < add->numbers->len; ++i )
    {
        const size_t index = add->numbers->len - i - 1;
        const uint64_t num = add->numbers->numbers[ index ];

        assert_that( num <= BIGINT_LIST_MEMBER_MAX,
                     "bigint list member value out of bounds: %" PRIu64,
                     num );

        if ( num >= INT64_MAX )
        {
            const int64_t main = ( int64_t ) ( num - INT64_MAX ) * add->sign;
            const int64_t rem  = ( int64_t ) ( num - main ) * add->sign;
            return_on_fail( bigint_add_power( bi, main, index ) );
            return_on_fail( bigint_add_power( bi, rem, index ) );
        }
        else
            return_on_fail( bigint_add_power( bi, num * add->sign, index ) );
    }

    return RV_SUCCESS;
}


int bigint_cmp_i( const struct bigint *bi, int64_t n )
{
    if ( bigint_sizeof( bi ) > 1 )
        return BIGINT_GT;

    const uint64_t bi_low = bi->numbers->numbers[ 0 ];
    if ( bi->sign != sgn_64( n ) )
        return bi->sign;

    if ( bi_low > INT64_MAX )
        return BIGINT_GT;

    const uint64_t norm_n = ( uint64_t ) n;
    return cmp_uint64_t( &bi_low, &norm_n );
}

int bigint_cmp_b( const struct bigint *bi1, const struct bigint *bi2 )
{
    const size_t s1 = bi1->numbers->len;
    const size_t s2 = bi2->numbers->len;

    if ( s1 != s2 )
        return cmp_size_t( &s1, &s2 );

    for ( size_t i = 0; i < s1; ++i )
    {
        const size_t idx  = s1 - i - 1;
        const uint64_t n1 = bi1->numbers->numbers[ idx ];
        const uint64_t n2 = bi2->numbers->numbers[ idx ];
        if ( n1 != n2 )
            return cmp_uint64_t( &n1, &n2 );
    }

    return BIGINT_EQ;
}
