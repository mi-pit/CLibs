//
// Created by MacBook on 03.02.2025.
//

#ifndef CLIBS_NUMBERS_LIST_H
#define CLIBS_NUMBERS_LIST_H

#include <stddef.h> /* size_t */
#include <stdint.h> /* uint64_t */

struct numbers_list {
    uint64_t *numbers;
    size_t len;
    size_t cap;
};

struct numbers_list *numls_init( void );
struct numbers_list *numls_init_cap( size_t cap );

void numls_destroy( struct numbers_list * );

int numls_append( struct numbers_list *, uint64_t num );
int numls_pop( struct numbers_list * );
int numls_extend_zeroes( struct numbers_list *, size_t new_len );

#endif //CLIBS_NUMBERS_LIST_H
