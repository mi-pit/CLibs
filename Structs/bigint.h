//
// Created by MacBook on 09.01.2025.
//

#ifndef CLIBS_BIGINT_H
#define CLIBS_BIGINT_H

#include "../Dev/attributes.h"
#include "dynarr.h"

#include <stdbool.h>
#include <stdint.h> /* uint64_t */


typedef enum {
    SIGN_NEG = -1,
    SIGN_POS = 1,
} sign_t;

struct bigint {
    struct dynamic_array *numbers;
    sign_t sign;
};


int bigint_init_p( struct bigint * );
Constructor struct bigint *bigint_init();
Constructor struct bigint *bigint_init_as( int64_t );

/// Frees all inner memory in the bigint struct
void bigint_destroy_l( struct bigint );
/// Frees all memory owned by the struct, including itself
void bigint_destroy( struct bigint * );

size_t bigint_sizeof( const struct bigint * );

UseResult char *bigint_to_string( const struct bigint * );
UseResult List *bigint_get_number_array( const struct bigint * );


int bigint_add_i( struct bigint *, int64_t );
int bigint_add_b( struct bigint *, const struct bigint * );

int bigint_mul_i( struct bigint *, int64_t );

void bigint_div_i( struct bigint *, int64_t );


#define BIGINT_LT 0x01
#define BIGINT_EQ 0x02
#define BIGINT_GT 0x04

int bigint_cmp_i( const struct bigint *, int64_t );
int bigint_cmp_b( const struct bigint *, const struct bigint * );


#endif //CLIBS_BIGINT_H
