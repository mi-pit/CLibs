#include "dictionary.h"

#include "errors.h"
#include "misc.h"
#include "pointer_utils.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct key_value_pair_set {
    struct key_value_pair *items;
    size_t size;
    size_t capacity;
};


int item_key_cmp( const void *i1, const void *i2 )
{
    const struct key_value_pair *item1 = i1;
    const struct key_value_pair *item2 = i2;

    if ( item1->key_size != item2->key_size )
        return cmp_size_t( &item1->key_size, &item2->key_size );
    return memcmp( item1->key, item2->key, item1->key_size );
}

int item_val_cmp( const void *i1, const void *i2 )
{
    const struct key_value_pair *item1 = i1;
    const struct key_value_pair *item2 = i2;

    if ( item1->val_size != item2->val_size )
        return cmp_size_t( &item1->val_size, &item2->val_size );
    return memcmp( item1->val, item2->val, item1->val_size );
}

int item_cmp( const void *i1, const void *i2 )
{
    int rv = item_key_cmp( i1, i2 );
    if ( rv != 0 )
        return rv;
    return item_val_cmp( i1, i2 );
}


Dict dict_init( void )
{
    struct key_value_pair_set *dict = calloc( 1, sizeof( struct key_value_pair_set ) );
    if ( dict == NULL )
    {
        warn( __func__ );
        return NULL;
    }

    dict->items = calloc( DICT_DEF_SIZE, sizeof( struct key_value_pair ) );
    if ( dict->items == NULL )
    {
        warn( __func__ );
        return NULL;
    }
    dict->capacity = DICT_DEF_SIZE;

    return dict;
}

int dict_insert_f( struct key_value_pair_set *dict,
                   const void *key,
                   size_t key_size,
                   const void *val,
                   size_t val_size,
                   PrintFunction key_print,
                   PrintFunction val_print )
{
    // todo: resize
    ssize_t hash = hash_func( key, key_size );

    const struct key_value_pair new_item = {
        .key_size = key_size,
        .val_size = val_size,
        .key      = ( void      *) key, // still const
        .val      = ( void      *) val,
    };

    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        size_t index                = ( hash + i ) % dict->capacity;
        struct key_value_pair *item = dict->items + index;
        if ( item_key_cmp( &new_item, item ) == 0 )
            return DICTINSERT_KEY_WAS_IN;

        if ( item->key != NULL )
            continue;

        assert( item->val == NULL );
        assert( item->key_size == 0 );
        assert( item->val_size == 0 );

        if ( ( item->key = malloc( key_size ) ) == NULL )
            return ffwarn_ret( RV_ERROR, "%s", "malloc" );
        if ( ( item->val = malloc( val_size ) ) == NULL )
            return ffwarn_ret( RV_ERROR, "%s", "malloc" );

        memcpy( item->key, key, key_size );
        memcpy( item->val, val, val_size );
        item->key_size  = key_size;
        item->val_size  = val_size;
        item->key_print = key_print;
        item->val_print = val_print;
        item->removed   = false;

        ++dict->size;

        return DICTINSERT_INSERTED;
    }

    return RV_ERROR;
}

int dict_insert( struct key_value_pair_set *dict,
                 const void *key,
                 size_t key_size,
                 const void *val,
                 size_t val_size )
{
    return dict_insert_f( dict, key, key_size, val, val_size, print_byte, print_byte );
}

static struct key_value_pair *dict_get_non_const( ConstDict dict,
                                                  const void *data,
                                                  size_t nbytes )
{
    ssize_t hash = hash_func( data, nbytes );

    const struct key_value_pair search_for = {
        .key      = ( void      *) data,
        .key_size = nbytes,
    };
    struct key_value_pair *item;

    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        size_t index = ( hash + i ) % dict->capacity;
        item         = dict->items + index;

        if ( item->key == NULL )
            continue;
        if ( item_key_cmp( item, &search_for ) == 0 )
            return item;
    }

    return NULL;
}

const struct key_value_pair *dict_get( ConstDict dict, const void *data, size_t nbytes )
{
    return dict_get_non_const( dict, data, nbytes );
}

const void *dict_get_val( ConstDict dict, const void *key, size_t key_size )
{
    const struct key_value_pair *item = dict_get_non_const( dict, key, key_size );
    return item == NULL ? NULL : item->val;
}

int dict_set_val( Dict dict,
                  const void *key,
                  size_t key_size,
                  const void *val,
                  size_t val_size )
{
    struct key_value_pair *item = dict_get_non_const( dict, key, key_size );
    if ( item == NULL )
        return RV_EXCEPTION;

    free( item->val );
    item->val = malloc( val_size );
    if ( item->val == NULL )
        return ffwarn_ret( RV_ERROR, "%s", "malloc" );

    item->val_size = val_size;
    memcpy( item->val, val, val_size );
    return 0;
}


int dict_remove( struct key_value_pair_set *dict, const void *key_data, size_t key_size )
{
    ssize_t hash = hash_func( key_data, key_size );

    struct key_value_pair search = {
        .key      = ( void      *) key_data,
        .key_size = key_size,
    };

    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        size_t index                = ( hash + i ) % dict->capacity;
        struct key_value_pair *item = dict->items + index;

        if ( item->key == NULL && !item->removed )
            return DICTREMOVE_NOT_FOUND;

        if ( !cmpeq( item_key_cmp( &search, item ) ) )
            continue;

        free_n( item->key );
        free_n( item->val );
        item->key_size  = 0;
        item->val_size  = 0;
        item->removed   = true;
        item->key_print = item->val_print = print_byte;
        return DICTREMOVE_REMOVED;
    }

    return DICTREMOVE_NOT_FOUND;
}


size_t dict_size( ConstDict dict )
{
    return dict->size;
}

size_t dict_cap( ConstDict dict )
{
    return dict->capacity;
}

struct key_value_pair *dict_items_as_array( ConstDict dict )
{
    struct key_value_pair *new_data = malloc( dict->size );

    size_t count = 0;

    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        if ( dict->items[ i ].key == NULL )
            continue;

        memcpy( new_data + count++, dict->items + i, sizeof( struct key_value_pair ) );
        new_data[ count++ ] = dict->items[ i ];
    }

    return new_data;
}


void kvp_print( const struct key_value_pair *item, const char *kv_sep )
{
    kvp_print_as( item, false, NULL, NULL, kv_sep );
}

void kvp_print_as( const struct key_value_pair *item,
                   bool user_func,
                   PrintFunction key_print,
                   PrintFunction val_print,
                   const char *kv_sep )
{
    ( user_func ? key_print : item->key_print )( item->key, item->key_size );
    printf( "%s", kv_sep );
    ( user_func ? val_print : item->val_print )( item->val, item->val_size );
}

static void dict_print_d( const struct key_value_pair_set *dict,
                          bool user_func,
                          PrintFunction key_print,
                          PrintFunction val_print )
{
    printf( "dictionary (size=%zu): {", dict->size );

    const char *delim;
    size_t n = 0;
    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        struct key_value_pair *item = dict->items + i;
        if ( item->key == NULL )
            continue;

        delim = ( item->key_size > 16 || n % 5 == 0 ) ? ",\n\t" : ", ";
        if ( n == 0 )
            delim = dict->size > 5 ? "\n\t" : " ";
        printf( "%s", delim );

        kvp_print_as( item, user_func, key_print, val_print, ": " );

        ++n;
    }

    if ( dict->size > 5 )
        printf( "\n" );
    else if ( !strchr( delim, '\n' ) )
        printf( " " );
    printf( "}\n" );
}

void dict_print( const struct key_value_pair_set *dict )
{
    dict_print_d( dict, false, NULL, NULL );
}

void dict_print_as( const struct key_value_pair_set *dict,
                    PrintFunction key_print,
                    PrintFunction val_print )
{
    dict_print_d( dict, true, key_print, val_print );
}


void dict_destroy( struct key_value_pair_set *dict )
{
    for ( size_t i = 0; i < dict->capacity; ++i )
    {
        free( dict->items[ i ].val );
        free( dict->items[ i ].key );
    }
    free( dict->items );
    free( dict );
}
