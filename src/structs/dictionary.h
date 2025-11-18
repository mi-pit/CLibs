/**
 * For more information, see `structs/set.h`.
 * Lots of relevant information is shared between the two structures.
 */

#ifndef CLIBS_DICTIONARY_H
#define CLIBS_DICTIONARY_H

#include "../headers/util/hash.h"
#include "../util/item_print_functions.h"


typedef struct dictionary Dictionary;


enum DictRemoveRV {
    DICTREMOVE_REMOVED   = 0,
    DICTREMOVE_NOT_FOUND = 1,
};

enum DictInsertRV {
    DICTINSERT_INSERTED = 0,
    DICTINSERT_WAS_IN   = 1,
};


struct dictionary *dict_init( void );


/**
 * Only compares the data in `key` and `key_size`
 *
 * @return  -1, 0, 1
 */
int dict_item_key_cmp( const void *, const void * );
/**
 * Only compares the data in `val` and `val_size`
 *
 * @return  -1, 0, 1
 */
int dict_item_val_cmp( const void *, const void * );
/**
 * Compares the keys first, then values
 *
 * @return  -1, 0, 1
 */
int dict_item_cmp( const void *, const void * );

int dict_insert_f( struct dictionary *,
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
int dict_insert( struct dictionary *,
                 const void *key,
                 size_t key_size,
                 const void *val,
                 size_t val_size );

bool dict_has_key( const struct dictionary *, const void *key, size_t key_size );

const void *dict_get_val( const struct dictionary *, const void *key, size_t key_size );

int dict_set_val( struct dictionary *,
                  const void *key,
                  size_t key_size,
                  const void *val,
                  size_t val_size );

enum DictRemoveRV dict_remove( struct dictionary *,
                               const void *key_data,
                               size_t key_size );


/* -------- SIZE/CAP -------- */

size_t dict_size( const struct dictionary * );

/* -------- DESTRUCTOR -------- */

void dict_destroy( struct dictionary * );


/* -------- PRINT -------- */

#define dict_printn( DICTIONARY )         \
    do                                    \
    {                                     \
        printf( "\"" #DICTIONARY "\" " ); \
        dict_print( DICTIONARY );         \
    }                                     \
    while ( 0 )

#define dict_printn_as( DICTIONARY, KEY_PRINT_FUNC, VAL_PRINT_FUNC ) \
    do                                                               \
    {                                                                \
        printf( "\"" #DICTIONARY "\" " );                            \
        dict_print_as( DICTIONARY, KEY_PRINT_FUNC, VAL_PRINT_FUNC ); \
    }                                                                \
    while ( 0 )

/**
 * Prints the contents of the dictionary as "‹key›: ‹val›"
 */
void dict_print( const struct dictionary * );
void dict_print_as( const struct dictionary *,
                    PrintFunction key_print,
                    PrintFunction val_print );

#endif //CLIBS_DICTIONARY_H
