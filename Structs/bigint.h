//
// Created by MacBook on 09.01.2025.
//

#ifndef CLIBS_BIGINT_H
#define CLIBS_BIGINT_H


#include "dynarr.h"

#include <stdbool.h>
#include <stdint.h> /* uint64_t */


typedef enum {
    SIGN_NEG = -1,
    SIGN_POS = 1,
} sign_t;

struct bigint {
    List /* of unsigned 64-bit ints */ numbers;
    sign_t sign;
};


struct bigint *bigint_init();
struct bigint *bigint_init_as( int64_t );
void bigint_destroy( struct bigint * );

size_t bigint_sizeof( const struct bigint * );

char *bigint_to_string( const struct bigint * );


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
