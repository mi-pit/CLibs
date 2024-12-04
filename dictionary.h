#ifndef CLIBS_DICTIONARY_H
#define CLIBS_DICTIONARY_H

#include "print_functions.h"

#include <stdbool.h>


typedef struct key_value_pair_set *Dict;
typedef const struct key_value_pair_set *ConstDict;


struct key_value_pair {
    void *key;
    size_t key_size;
    PrintFunction key_print;

    void *val;
    size_t val_size;
    PrintFunction val_print;

    bool removed;
};


#define DICT_DEF_SIZE 64


enum DictInsertRV {
    DICTINSERT_INSERTED   = 1,
    DICTINSERT_KEY_WAS_IN = 2,
};

enum DictRemoveRV {
    DICTREMOVE_REMOVED   = 1,
    DICTREMOVE_NOT_FOUND = 2,
};


Dict dict_init( void );


/**
 * Only compares the data in key and key_size
 * @return  -1, 0, 1 like memcmp
 */
int item_key_cmp( const void *, const void * );
int item_val_cmp( const void *, const void * );
int item_cmp( const void *, const void * );

// clang-format off
int dict_insert_f( Dict,
                   const void *key, size_t key_size,
                   const void *val, size_t val_size,
                   PrintFunction key_print, PrintFunction val_print );

int dict_insert( Dict,
                 const void *key, size_t key_size,
                 const void *val, size_t val_size );

const struct key_value_pair *dict_get( ConstDict, const void *key, size_t key_size );

const void * dict_get_val( ConstDict, const void *key, size_t key_size );
int dict_set_val( Dict,
                  const void *key, size_t key_size,
                  const void *val, size_t val_size );
// clang-format on

int dict_remove( Dict, const void *data, size_t nbytes );


/* -------- SIZE/CAP -------- */

size_t dict_size( ConstDict );
size_t dict_cap( ConstDict );
struct key_value_pair *dict_items_as_array( ConstDict dict );

/* -------- DESTRUCTOR -------- */

void dict_destroy( Dict );


/* -------- PRINT -------- */

#define dict_printn( dict )         \
    do                              \
    {                               \
        printf( "\"" #dict "\" " ); \
        dict_print( dict );         \
    }                               \
    while ( 0 )

#define dict_printn_as( dict, key_func, val_func ) \
    do                                             \
    {                                              \
        printf( "\"" #dict "\" " );                \
        dict_print_as( dict, key_func, val_func ); \
    }                                              \
    while ( 0 )

void dict_print( ConstDict );
void dict_print_as( ConstDict, PrintFunction key_print, PrintFunction val_print );

void kvp_print( const struct key_value_pair *item, const char *kv_sep );
void kvp_print_as( const struct key_value_pair *item,
                   bool user_func,
                   PrintFunction key_print,
                   PrintFunction val_print,
                   const char *kv_sep );

#endif //CLIBS_DICTIONARY_H
