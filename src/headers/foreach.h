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
 * Iterates over the ‹array›<br>
 * Stores each array member in a new variable ‹item› (only
 * visible in the scope of the foreach loop)
 * @param type  C-keyword (or typedef/…) of the type of the array items and the new
 *              variable
 *
 * @param item  name of the new variable
 * @param array array of type 'type[]'
 */
#define foreach_arr( ITEM_TYPE, ITEM_NAME, ARRAY, COUNT )                \
    foreach_helper_init( COUNT, ITEM_NAME )                              \
            foreach_helper_assign( ITEM_TYPE, ITEM_NAME, ( ARRAY )[ 0 ], \
                                   ( ARRAY )[ foreach_index_##ITEM_NAME ] )

/**
 * Iterates over a string
 * <p>
 * Stores each character of the string in a new variable of type char
 * </p>
 *
 * @param item  name of the new variable
 * @param strn string
 */
#define foreach_str( ITEM_NAME, STRING )                                   \
    foreach_helper_init( strlen( STRING ), ITEM_NAME )                     \
            foreach_helper_assign( const char, ITEM_NAME, ( STRING )[ 0 ], \
                                   ( STRING )[ foreach_index_##ITEM_NAME ] )


#ifdef CLIBS_DYNAMIC_ARRAY_H
/**
 * Iterates over the List<br>
 * Stores each list member in a new variable named ‹item› of type char
 *
 * @param type  type of the new variable and items in the list
 * @param item  name of the new variable
 * @param ls string
 */
#define foreach_ls( TYPE, ITEM_NAME, LIST )                                      \
    foreach_helper_init( list_size( LIST ), ITEM_NAME )                          \
            foreach_helper_assign( TYPE, ITEM_NAME, list_fetch( LIST, 0, TYPE ), \
                                   list_fetch( LIST, foreach_index_##ITEM_NAME, TYPE ) )
#endif //CLIBS_DYNAMIC_ARRAY_H


#ifdef CLIBS_SETS_H
/**
 * Iterates over a set, results are set in an SetEnumeratedEntry
 * "Retrieves" a `const SetEnumeratedEntry` struct (see sets.h);
 * in short, `SetEnumeratedEntry` contains
 * - `struct set_item *` -- the desired data
 * - `index` -- for the iterator, doesn't really hold any value for the user
 *
 * @param SET set
 */
#define foreach_set( SET )                                                      \
    foreach_uni ( const SetEnumeratedEntry, entry, set_get_next( ( SET ), -1 ), \
                  set_get_next( ( SET ), entry.index ), set_size( ( SET ) ) )
#endif //CLIBS_SETS_H


#ifdef CLIBS_QUEUE_H
/**
 * Iterates over a set, results are set in an SetEnumeratedEntry
 * "Retrieves" a `const QueueEnumeratedEntry` struct (see queue.h);
 * in short, `QueueEnumeratedEntry` contains
 * - `struct set_item *` -- the desired data
 * - `index` -- for the iterator, doesn't really hold any value for the user
 *
 * @param QUEUE valid fifo_queue *
 *
 * @example
 * @code
 * foreach_que( queue )
 *     const int data = deref_as( int, entry.data );
 * @endcode
 */
#define foreach_que( QUEUE )                                      \
    foreach_uni ( const QueueEnumeratedEntry, entry,              \
                  queue_get_next( ( QUEUE ), NULL, true ),        \
                  queue_get_next( ( QUEUE ), entry.item, false ), \
                  queue_get_size( ( QUEUE ) ) )
// TODO?: remove `queue_get_size` call (for efficiency)
#endif //CLIBS_QUEUE_H


#endif //CLIBS_FOREACH_H
