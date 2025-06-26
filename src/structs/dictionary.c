#include "dictionary.h"

#include "../headers/assert_that.h"
#include "../headers/errors.h"        /* includes misc.h */
#include "../headers/misc.h"          /* hash_func(), cmp_size_t(), cmpeq() */
#include "../headers/pointer_utils.h" /* free_n() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct dictionary {
    struct key_value_pair *items;
    size_t size;
    size_t capacity;
};


struct key_value_pair {
    void *key;
    size_t key_size;
    PrintFunction key_print;

    void *val;
    size_t val_size;
    PrintFunction val_print;

    bool removed;
};


int dict_item_key_cmp( const void *i1, const void *i2 )
{
    const struct key_value_pair *item1 = i1;
    const struct key_value_pair *item2 = i2;

    if ( item1->key_size != item2->key_size )
        return cmp_size_t( &item1->key_size, &item2->key_size );
    return memcmp( item1->key, item2->key, item1->key_size );
}

int dict_item_val_cmp( const void *i1, const void *i2 )
{
    const struct key_value_pair *item1 = i1;
    const struct key_value_pair *item2 = i2;

    if ( item1->val_size != item2->val_size )
        return cmp_size_t( &item1->val_size, &item2->val_size );
    return memcmp( item1->val, item2->val, item1->val_size );
}

int dict_item_cmp( const void *i1, const void *i2 )
{
    const int rv = dict_item_key_cmp( i1, i2 );
    if ( rv != 0 )
        return rv;
    return dict_item_val_cmp( i1, i2 );
}


#define DICT_DEF_CAP 64


struct dictionary *dict_init( void )
{
    struct dictionary *dict = calloc( 1, sizeof( struct dictionary ) );
    if ( dict == NULL )
        return ( void * ) fwarn_ret( NULL, "calloc" );

    dict->items = calloc( DICT_DEF_CAP, sizeof( struct key_value_pair ) );
    if ( dict->items == NULL )
    {
        free( dict );
        return ( void * ) fwarn_ret( NULL, "calloc" );
    }

    dict->capacity = DICT_DEF_CAP;

    return dict;
}

int dict_insert_f( struct dictionary *dict,
                   const void *key,
                   size_t key_size,
                   const void *val,
                   size_t val_size,
                   const PrintFunction key_print,
                   const PrintFunction val_print )
{
    // todo: resize
    const uint64_t hash = hash_func( key, key_size );

    const struct key_value_pair new_item = {
        .key_size = key_size,
        .val_size = val_size,
        .key      = ( void * ) key,
        .val      = ( void * ) val,
    };

    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        const size_t index          = ( hash + i ) % dict->capacity;
        struct key_value_pair *item = dict->items + index;

        if ( item->key != NULL )
        {
            if ( dict_item_key_cmp( &new_item, item ) == 0 )
                return DICTINSERT_WAS_IN;
            continue;
        }

        if ( item->key == NULL )
            dict->size++;

        free( item->key );
        free( item->val );

        item->key = malloc( key_size );
        if ( item->key == NULL )
            return fwarn_ret( RV_ERROR, "malloc" );

        item->val = malloc( val_size );
        if ( item->val == NULL )
        {
            free( item->key );
            return fwarn_ret( RV_ERROR, "malloc" );
        }

        memcpy( item->key, key, key_size );
        memcpy( item->val, val, val_size );
        item->key_size  = key_size;
        item->val_size  = val_size;
        item->key_print = key_print;
        item->val_print = val_print;
        item->removed   = false;

        return DICTINSERT_INSERTED;
    }

    return RV_ERROR;
}

int dict_insert( struct dictionary *dict,
                 const void *key,
                 const size_t key_size,
                 const void *val,
                 const size_t val_size )
{
    return dict_insert_f( dict, key, key_size, val, val_size, print_byte, print_byte );
}

Private struct key_value_pair *dict_get_non_const( const struct dictionary *const dict,
                                                   const void *data,
                                                   const size_t nbytes )
{
    const uint64_t hash = hash_func( data, nbytes );

    const struct key_value_pair search_for = {
        .key      = ( void * ) data,
        .key_size = nbytes,
    };

    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        const size_t index          = ( hash + i ) % dict->capacity;
        struct key_value_pair *item = dict->items + index;

        if ( item->key == NULL )
            continue;
        if ( dict_item_key_cmp( item, &search_for ) == 0 )
            return item;
    }

    return NULL;
}

bool dict_has_key( const struct dictionary *dict, const void *key, size_t key_size )
{
    return dict_get_non_const( dict, key, key_size ) != NULL;
}

const void *dict_get_val( const struct dictionary *dict,
                          const void *key,
                          const size_t key_size )
{
    const struct key_value_pair *item = dict_get_non_const( dict, key, key_size );
    return item == NULL ? NULL : item->val;
}

int dict_set_val( struct dictionary *dict,
                  const void *key,
                  const size_t key_size,
                  const void *val,
                  const size_t val_size )
{
    struct key_value_pair *item = dict_get_non_const( dict, key, key_size );
    if ( item == NULL )
        return fwarnx_ret( RV_EXCEPTION, "couldn't find key" );

    free( item->val );
    item->val = malloc( val_size );
    if ( item->val == NULL )
        return fwarn_ret( RV_ERROR, "malloc" );

    item->val_size = val_size;
    memcpy( item->val, val, val_size );
    return RV_SUCCESS;
}


enum DictRemoveRV dict_remove( struct dictionary *dict,
                               const void *key_data,
                               const size_t key_size )
{
    const uint64_t hash = hash_func( key_data, key_size );

    const struct key_value_pair search = {
        .key      = ( void * ) key_data,
        .key_size = key_size,
    };

    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        const size_t index          = ( hash + i ) % dict->capacity;
        struct key_value_pair *item = dict->items + index;

        if ( item->key == NULL && !item->removed )
            return DICTREMOVE_NOT_FOUND;

        if ( !cmpeq( dict_item_key_cmp( &search, item ) ) )
            continue;

        free_n( item->key );
        free_n( item->val );
        item->key_size  = 0;
        item->val_size  = 0;
        item->removed   = true;
        item->key_print = item->val_print = print_byte;
        dict->size--;
        return DICTREMOVE_REMOVED;
    }

    return DICTREMOVE_NOT_FOUND;
}


size_t dict_size( const struct dictionary *dict )
{
    return dict->size;
}

Private inline void kvp_print_as( const struct key_value_pair *item,
                                  const PrintFunction key_print,
                                  const PrintFunction val_print,
                                  const char *kv_sep )
{
    assert_that( ( item->key_print != NULL || key_print != NULL )
                         && ( item->val_print != NULL || val_print != NULL ),
                 "no way to print values, this shouldn't happen" );

    printf( "\"" );
    ( key_print != NULL ? key_print : item->key_print )( item->key, item->key_size );
    printf( "%s", kv_sep );
    ( val_print != NULL ? val_print : item->val_print )( item->val, item->val_size );
    printf( "\"" );
}


void dict_print_as( const struct dictionary *dict,
                    const PrintFunction key_print,
                    const PrintFunction val_print )
{
    /** Maximum items printed on one line */
    static const size_t line_max_items = 4;

    printf( "{" );

    const char *delim = "";
    size_t n          = 0;
    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        const struct key_value_pair *item = dict->items + i;
        if ( item->key == NULL )
            continue;

        delim = item->key_size > 16 || n % line_max_items == 0 ? ",\n\t" : ", ";
        if ( n == 0 )
            delim = dict->size > line_max_items ? "\n\t" : " ";
        printf( "%s", delim );

        kvp_print_as( item, key_print, val_print, ": " );

        ++n;
    }

    if ( dict->size > line_max_items )
        printf( "\n" );
    else if ( !strchr( delim, '\n' ) )
        printf( " " );
    printf( "}\n" );
}

void dict_print( const struct dictionary *dict )
{
    dict_print_as( dict, NULL, NULL );
}


void dict_destroy( struct dictionary *dict )
{
    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        free( dict->items[ i ].val );
        free( dict->items[ i ].key );
    }
    free( dict->items );
    free( dict );
}
