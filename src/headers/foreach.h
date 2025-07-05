/*
 * Macros for iterating over different structures (arrays, strings, sets, ...).
 *
 * To define `foreach` for a data structure in `src/structs`,
 * first include the struct, then this header.
 *
 *
 * Created by MacBook on 23.10.2024.
 */

#ifndef CLIBS_FOREACH_H
#define CLIBS_FOREACH_H

#include <stdlib.h>
#include <string.h>


/**
 * Universal foreach macro
 *
 * @param TYPE          type of the item
 * @param ITEM_NAME     name of the variable created and assigned in the loop
 * @param INITIALIZER   runs on the first pass through the loop
 * @param ACCESSOR      runs every other time (index variable is foreach_index_ITEM_NAME)
 * @param SIZE          number of elements in the structure
 */
#define foreach_uni( TYPE, ITEM_NAME, INITIALIZER, ACCESSOR, SIZE ) \
    foreach_helper_init( SIZE, ITEM_NAME )                          \
            foreach_helper_assign( TYPE, ITEM_NAME, INITIALIZER, ACCESSOR )

/**
 * Iterates over an array.
 *
 * Stores each array member in a new variable (only
 * visible in the scope of the foreach loop).
 *
 * @param ITEM_TYPE     C-keyword (or typedef/…) of the type of the array
 *                      items and the new variable
 * @param ITEM_NAME     name of the new variable
 * @param ARRAY         array of type `TYPE[ COUNT ]`
 * @param COUNT         number of elements in array
 */
#define foreach_arr( ITEM_TYPE, ITEM_NAME, ARRAY, COUNT )                \
    foreach_helper_init( COUNT, ITEM_NAME )                              \
            foreach_helper_assign( ITEM_TYPE, ITEM_NAME, ( ARRAY )[ 0 ], \
                                   ( ARRAY )[ foreach_index_##ITEM_NAME ] )

/**
 * Iterates over a string.
 *
 * Stores each character of the string in a new variable of type `const char`.
 *
 * @param ITEM_NAME name of the new variable
 * @param STRING    string
 */
#define foreach_str( ITEM_NAME, STRING )                                   \
    foreach_helper_init( strlen( STRING ), ITEM_NAME )                     \
            foreach_helper_assign( const char, ITEM_NAME, ( STRING )[ 0 ], \
                                   ( STRING )[ foreach_index_##ITEM_NAME ] )


#if defined( CLIBS_FOREACH_LIST ) || defined( CLIBS_DYNAMIC_ARRAY_H )
/**
 * Iterates over a list.
 *
 * Stores each list member in a new variable.
 *
 * @param TYPE      type of the new variable and items in the list
 * @param ITEM_NAME name of the new variable
 * @param LIST      a valid `List *`
 *
 * @attention
 * Requires previous definition of `CLIBS_FOREACH_LIST` or `CLIBS_DYNAMIC_ARRAY_H`.
 * The latter is defined when including `src/structs/dynarr.h`.
*/
#define foreach_ls( TYPE, ITEM_NAME, LIST )                                      \
    foreach_helper_init( list_size( LIST ), ITEM_NAME )                          \
            foreach_helper_assign( TYPE, ITEM_NAME, list_fetch( LIST, 0, TYPE ), \
                                   list_fetch( LIST, foreach_index_##ITEM_NAME, TYPE ) )
#endif // List


#if defined( CLIBS_FOREACH_SET ) || defined( CLIBS_SETS_H )
/**
 * Iterates over a set.
 *
 * Retrieves a `const SetEnumeratedEntry` struct (see `src/structs/set.h`);
 * in short, `SetEnumeratedEntry` contains a field `struct set_item *item`
 * with the desired data
 *
 * @param SET a valid `Set *`
 *
 * @attention
 * Requires previous definition of `CLIBS_FOREACH_SET` or `CLIBS_SETS_H`.
 * The latter is defined when including `src/structs/set.h`.
 */
#define foreach_set( SET )                                                      \
    foreach_uni ( const SetEnumeratedEntry, entry, set_get_next( ( SET ), -1 ), \
                  set_get_next( ( SET ), entry.index ), set_size( ( SET ) ) )
#endif // Set


#if defined( CLIBS_FOREACH_QUEUE ) || defined( CLIBS_QUEUE_H )
/**
 * Iterates over a queue.
 *
 * "Retrieves" a `const QueueEnumeratedEntry` struct (see `src/structs/queue.h`);
 * in short, `QueueEnumeratedEntry` contains a field
 * `void *const data` -- data from the item
 *
 * Example
 * @code
 * foreach_que( queue )
 *     const int data = deref_as( int, entry.data );
 * @endcode
 *
 * @param QUEUE valid `struct fifo_queue *`
 *
 * @attention
 * Requires previous definition of `CLIBS_FOREACH_QUEUE` or `CLIBS_QUEUE_H`.
 * The latter is defined when including `src/structs/queue.h`.
 */
#define foreach_que( QUEUE )                                      \
    foreach_uni ( const QueueEnumeratedEntry, entry,              \
                  queue_get_next( ( QUEUE ), NULL, true ),        \
                  queue_get_next( ( QUEUE ), entry.item, false ), \
                  queue_get_size( ( QUEUE ) ) )
// TODO?: remove `queue_get_size` call (for efficiency)
#endif // Queue


/** @cond INTERNAL */
#define foreach_helper_init( SIZE, ITEM_NAME )                                \
    for ( size_t foreach_keep_##ITEM_NAME = 1, foreach_index_##ITEM_NAME = 0, \
                 foreach_cap_##ITEM_NAME = ( SIZE );                          \
          foreach_keep_##ITEM_NAME                                            \
          && foreach_index_##ITEM_NAME < foreach_cap_##ITEM_NAME;             \
          foreach_keep_##ITEM_NAME = !foreach_keep_##ITEM_NAME,               \
                 ++foreach_index_##ITEM_NAME )

#define foreach_helper_assign( TYPE, ITEM_NAME, INITIALIZER, ACCESSOR )         \
    for ( TYPE ITEM_NAME = ( foreach_index_##ITEM_NAME == 0 ) ? ( INITIALIZER ) \
                                                              : ( ACCESSOR );   \
          foreach_keep_##ITEM_NAME;                                             \
          foreach_keep_##ITEM_NAME = !foreach_keep_##ITEM_NAME )
/** @endcond */


#endif //CLIBS_FOREACH_H
