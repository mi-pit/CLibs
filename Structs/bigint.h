//
// Created by MacBook on 09.01.2025.
//

#ifndef CLIBS_BIGINT_H
#define CLIBS_BIGINT_H

#include "../Dev/attributes.h" /* Constructor */
#include "../misc.h"           /* sign */
#include "dynarr.h"            /* List */

#include <stdbool.h> /* ... */
#include <stdint.h>  /* uint64_t */


struct bigint {
    struct dynamic_array *numbers;
    sign_t sign;
};


UseResult char *add_uint_strings( const char *str_1, const char *str_2 );
UseResult char *mul_uint_strings( const char *str_1, const char *str_2 );


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
UseResult List *bigint_get_number_array( const struct bigint * );


int bigint_add_i( struct bigint *, int64_t );
/**
 * Adds
 * @code
 * (UINT64_MAX ^ power) * n // UINT64_MAX == 2^64 - 1
 * @endcode
 * to the bigint
 * @param bi    valid bi struct
 * @param n     any number -- may be negative
 * @param power
 * @return
 */
int bigint_add_power( struct bigint *bi, int64_t n, uint64_t power );
int bigint_add_b( struct bigint *, const struct bigint * );

int bigint_sub_b( struct bigint *, const struct bigint * );

int bigint_mul_i( struct bigint *, long double );
int bigint_mul_b( struct bigint *, const struct bigint * );

int bigint_div_b( struct bigint *, const struct bigint * );


#define BIGINT_LT 0x01
#define BIGINT_EQ 0x02
#define BIGINT_GT 0x04

int bigint_cmp_i( const struct bigint *, int64_t );
int bigint_cmp_b( const struct bigint *, const struct bigint * );


#endif //CLIBS_BIGINT_H
