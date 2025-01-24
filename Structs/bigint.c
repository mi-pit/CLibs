//
// Created by MacBook on 09.01.2025.
//

#include "bigint.h"

#include "../Dev/assert_that.h" /* includes errors.h */
#include "../extra_types.h"     /* byte */
#include "../foreach.h"         /* foreach_ls */
#include "../pointer_utils.h"   /* new */
#include "../string_utils.h"    /* str_t, string_reversed() */
#include "dynstring.h"          /* dynstr */

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
    return list_size( bi->numbers );
}


str_t mul_uint_strings( string_t str_1, string_t str_2 )
{
    typedef uint8_t digit_t;

    size_t len_1      = strlen( str_1 );
    size_t len_2      = strlen( str_2 );
    size_t result_len = len_1 + len_2;

    char *result = malloc( result_len + 1 );
    if ( result == NULL )
        return ( void * ) fwarn_ret( NULL, "malloc" );
    memset( result, '0', result_len );
    result[ result_len ] = '\0';

    // Perform multiplication using the "long multiplication" algorithm
    for ( ssize_t i = ( ssize_t ) len_1 - 1; i >= 0; --i )
    {
        digit_t carry = 0;
        for ( ssize_t j = ( ssize_t ) len_2 - 1; j >= 0; --j )
        {
            digit_t prod = ( str_1[ i ] - '0' ) * ( str_2[ j ] - '0' ) + carry
                           + ( result[ i + j + 1 ] - '0' );
            result[ i + j + 1 ] = ( char ) ( ( prod % 10 ) + '0' );
            carry               = prod / 10;
        }
        result[ i ] = ( char ) ( result[ i ] + carry );
    }

    // Remove leading zeros
    size_t start = 0;
    while ( start < result_len && result[ start ] == '0' )
        ++start;

    char *final_result =
            ( start == result_len ) ? strdup( "0" ) : strdup( result + start );
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
    for ( size_t idx = 0; idx < max_u64( str_1_len, str_2_len ); ++idx )
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


int bigint_init_p( struct bigint *bi )
{
    if ( ( bi->numbers = list_init_type( uint64_t ) ) == NULL )
    {
        f_stack_trace();
        return RV_ERROR;
    }
    bi->sign = SIGN_POS; // fixme? maybe this should be SIGN_ZERO

    static const uint64_t ZERO = 0;
    assert_that( list_append( bi->numbers, &ZERO ) == RV_SUCCESS,
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

    const uint64_t u_n = n > 0 ? n : ( uint64_t ) llabs( ( long long ) n );
    assert_that( list_set_at( new->numbers, 0, &u_n ) == RV_SUCCESS,
                 "bigint.numbers.capacity should always be at least one" );

    new->sign = n == 0 ? SIGN_POS : sgn_64( n );

    return new;
}


str_t bigint_to_string( const struct bigint *const bi )
{
    DynamicString dynstr =
            dynstr_init_cap( list_size( bi->numbers ) * sizeof( uint64_t ) * 2 );
    if ( dynstr == NULL )
    {
        f_stack_trace();
        return NULL;
    }

    ssize_t idx_last = ( ssize_t ) list_size( bi->numbers ) - 1;
    for ( ssize_t idx = idx_last; idx >= 0; --idx )
    {
        char fmt_buf[ 1 + 1 + 20 + 3 + 1 ];
        /*            %   0   i   llu \0 */
        snprintf(
                fmt_buf, sizeof fmt_buf, "%%0%i" PRIu64, BIGINT_LIST_MEMBER_MAX_DIGITS );

        uint64_t num = list_access( bi->numbers, idx, uint64_t );
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
    f_stack_trace();
    dynstr_destroy( dynstr );
    return NULL;
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


Private inline int handle_overflow( struct bigint *const bi, size_t level )
{
    if ( list_size( bi->numbers ) == level + 1 ) // higher digit not available
    {
        uint64_t high = 1;
        return list_append( bi->numbers, &high );
    }

    return bigint_add_power( bi, bi->sign, level + 1 );
}

Private inline int handle_underflow( struct bigint *const bi, size_t level )
{
    if ( list_size( bi->numbers ) == level + 1 ) // higher digit not available
    {
        uint64_t high = 1;
        return list_append( bi->numbers, &high );
    }

    return bigint_add_power( bi, sign_flipped( bi->sign ), level + 1 );
}

int bigint_add_power( struct bigint *const bi, const int64_t n, uint64_t level )
{
    if ( n == 0 )
        return RV_SUCCESS;

    if ( list_size( bi->numbers ) <= level )
    {
        const size_t len = level - list_size( bi->numbers ) + 1;
        uint64_t *ZEROES = calloc( len, sizeof( uint64_t ) );
        return_on_fail( list_extend( bi->numbers, ZEROES, len ) );
        free( ZEROES );
    }

    uint64_t low = list_access( bi->numbers, level, uint64_t );

    // int64_t norm_n isn't able to hold this value
    // => subtract it after converting to uint64_t
    bool sub_one_more = sgn_64( n ) == bi->sign && n == INT64_MIN;

    int64_t norm_n = sub_one_more ? INT64_MAX : n * bi->sign;

    bool overflow  = norm_n > 0 && low > BIGINT_LIST_MEMBER_MAX - ( uint64_t ) norm_n;
    bool underflow = norm_n < 0 && low < ( uint64_t ) llabs( norm_n );
    assert_that( !overflow || !underflow, "cannot both be set at once" );

    bool flip_bi_sign = underflow && list_size( bi->numbers ) == level + 1;

    const uint64_t final =
            ( flip_bi_sign ? low - norm_n
              : underflow  ? BIGINT_LIST_MEMBER_MAX + 1 + ( ( int64_t ) low + norm_n )
                           : ( low + norm_n ) % ( BIGINT_LIST_MEMBER_MAX + 1 ) )
            - ( int ) sub_one_more;
    assert_that( list_set_at( bi->numbers, level, &final ) == RV_SUCCESS,
                 "couldn't set number list element; index=%" PRIu64 ", size=%zu",
                 level,
                 list_size( bi->numbers ) );

    if ( flip_bi_sign )
        bigint_flip_sign( bi );
    else if ( underflow )
        return_on_fail( handle_underflow( bi, level ) );
    else if ( overflow )
        return_on_fail( handle_overflow( bi, level ) );

    if ( final == 0 && level == list_size( bi->numbers ) - 1 )
        return list_pop( bi->numbers, NULL );

    return RV_SUCCESS;
}

int bigint_add_i( struct bigint *bi, int64_t n )
{
    return bigint_add_power( bi, n, 0 );
}
