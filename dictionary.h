#ifndef CLIBS_DICTIONARY_H
#define CLIBS_DICTIONARY_H

#include "item_print_functions.h"

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

enum DictRemoveRV {
    DICTREMOVE_REMOVED   = 1,
    DICTREMOVE_NOT_FOUND = 2,
};


Dict dict_init( void );


/**
 * Only compares the data in key and key_size
 * @param item_1 const struct key_value_pair *
 * @param item_2 const struct key_value_pair *
 * @return  -1, 0, 1 like memcmp
 */
int item_key_cmp( const void *item_1, const void *item_2 );
int item_val_cmp( const void *, const void * );
int item_cmp( const void *, const void * );

int dict_insert_f( Dict,
                   const void *key,
                   size_t key_size,
                   const void *val,
                   size_t val_size,
                   PrintFunction key_print,
                   PrintFunction val_print );

/**
 * Inserts the value pointed to by ‹val› into the dictionary
 * with the key pointed to by ‹key›
 *
 * @param key key pointer
 * @param key_size sizeof key
 * @param val val pointer
 * @param val_size sizeof val
 * @return
 */
int dict_insert( Dict,
                 const void *key,
                 size_t key_size,
                 const void *val,
                 size_t val_size );

const struct key_value_pair *dict_get( ConstDict, const void *key, size_t key_size );

const void *dict_get_val( ConstDict, const void *key, size_t key_size );
int dict_set_val( Dict,
                  const void *key,
                  size_t key_size,
                  const void *val,
                  size_t val_size );

int dict_remove( Dict, const void *data, size_t nbytes );


/* -------- SIZE/CAP -------- */

size_t dict_size( ConstDict );
size_t dict_cap( ConstDict );
struct key_value_pair *dict_items_as_array( ConstDict dict );

/* -------- DESTRUCTOR -------- */

void dict_destroy( Dict );


/* -------- PRINT -------- */

/** Maximum items printed on one line */
#define DICT_PRINT_LINE_MAX_ITEMS 4

#define dict_printn( DICTIONARY )         \
    do                                    \
    {                                     \
        printf( "\"" #DICTIONARY "\" " ); \
        dict_print( DICTIONARY );         \
    }                                     \
    while ( 0 )

#define dict_printn_as( DICTIONARY, key_func, val_func ) \
    do                                                   \
    {                                                    \
        printf( "\"" #DICTIONARY "\" " );                \
        dict_print_as( DICTIONARY, key_func, val_func ); \
    }                                                    \
    while ( 0 )

/**
 * Prints the contents of the dictionary as "‹key›: ‹val›"
 */
void dict_print( ConstDict );
void dict_print_as( ConstDict, PrintFunction key_print, PrintFunction val_print );

void kvp_print( const struct key_value_pair *item, const char *kv_sep );
void kvp_print_as( const struct key_value_pair *item,
                   PrintFunction key_print,
                   PrintFunction val_print,
                   const char *kv_sep );

#endif //CLIBS_DICTIONARY_H
