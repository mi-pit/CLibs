//
// Created by MacBook on 09.01.2025.
//

#ifndef CLIBS_BIGINT_H
#define CLIBS_BIGINT_H

#include "../Dev/attributes.h" /* Constructor, UseResult */
#include "../misc.h"           /* sign_t */
#include "dynarr.h"            /* List */

#include <stdbool.h> /* ... */
#include <stdint.h>  /* (u)int64_t */


struct bigint {
    struct dynamic_array *numbers;
    sign_t sign;
};


#define BIGINT_LIST_MEMBER_MAX        ( UINT64_C( 10000000000000000000 ) - 1 )
#define BIGINT_LIST_MEMBER_MAX_DIGITS ( 19 )


int bigint_init_p( struct bigint * );
Constructor struct bigint *bigint_init( void );
Constructor struct bigint *bigint_init_as( int64_t );

/// Frees all inner memory in the bigint struct
void bigint_destroy_l( struct bigint );
/// Frees all memory owned by the struct, including itself
void bigint_destroy( struct bigint * );

/**
 * Returns the size of the number
 * @return Number of base-‹UINT64_MAX› digits
 */
size_t bigint_sizeof( const struct bigint * );

sign_t bigint_sign( const struct bigint * );
void bigint_flip_sign( struct bigint * );


UseResult char *bigint_to_string( const struct bigint * );
int bigint_from_string( const char *, struct bigint **cont );


///// All of these functions return RV_SUCCESS / RV_ERROR

int bigint_add_i( struct bigint *, int64_t );
/**
 * Adds
 * @code
 * (10000000000000000000 ^ power) * n
 * @endcode
 * to the bigint
 * @param bi    valid bi struct
 * @param n     any number -- may be negative
 * @param power positive integer; might act strange when nearing SIZE_MAX
 * @return RV_ERROR in case list_append() fails, else RV_SUCCESS
 */
int bigint_add_power( struct bigint *, int64_t n, uint64_t power );
int bigint_add_b( struct bigint *, const struct bigint * );

int bigint_mul_i( struct bigint *, long double );
int bigint_mul_b( struct bigint *, const struct bigint * );

int bigint_div_b( struct bigint *, const struct bigint * );


#define BIGINT_LT ( -1 )
#define BIGINT_EQ ( 0 )
#define BIGINT_GT ( +1 )

int bigint_cmp_i( const struct bigint *, int64_t );
int bigint_cmp_b( const struct bigint *, const struct bigint * );


#endif //CLIBS_BIGINT_H
