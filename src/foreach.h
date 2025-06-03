//
// Created by MacBook on 23.10.2024.
//

#ifndef CLIBS_FOREACH_H
#define CLIBS_FOREACH_H

#include <stdlib.h>
#include <string.h>

#define foreach_helper_init( SIZE, ITEM_NAME )                                \
    for ( size_t foreach_keep_##ITEM_NAME = 1, foreach_index_##ITEM_NAME = 0, \
                 foreach_cap_##ITEM_NAME = ( SIZE );                          \
          foreach_keep_##ITEM_NAME &&                                         \
          foreach_index_##ITEM_NAME < foreach_cap_##ITEM_NAME;                \
          foreach_keep_##ITEM_NAME = !foreach_keep_##ITEM_NAME,               \
                 ++foreach_index_##ITEM_NAME )

#define foreach_helper_assign( TYPE, ITEM_NAME, INITIALIZER, ACCESSOR )         \
    for ( TYPE ITEM_NAME = ( foreach_index_##ITEM_NAME == 0 ) ? ( INITIALIZER ) \
                                                              : ( ACCESSOR );   \
          foreach_keep_##ITEM_NAME;                                             \
          foreach_keep_##ITEM_NAME = !foreach_keep_##ITEM_NAME )


#define foreach_uni( TYPE, ITEM_NAME, INITIALIZER, ACCESSOR, SIZE ) \
    foreach_helper_init( SIZE, ITEM_NAME )                          \
            foreach_helper_assign( TYPE, ITEM_NAME, INITIALIZER, ACCESSOR )

/**
 * Iterates over the ‹array›<br>
 * Stores each array member in a new variable ‹item› (only
 * visible in the scope of the foreach loop)
 * @param type  C-keyword (or typedef/…) of the type of the array items and the new
 * variable
 * @param item  name of the new variable
 * @param array array of type 'type[]'
 */
#define foreach_arr( ITEM_TYPE, ITEM_NAME, ARRAY, COUNT )                \
    foreach_helper_init( COUNT, ITEM_NAME )                              \
            foreach_helper_assign( ITEM_TYPE, ITEM_NAME, ( ARRAY )[ 0 ], \
                                   ( ARRAY )[ foreach_index_##ITEM_NAME ] )

/**
 * Iterates over a string<br>
 * Stores each character of the string in a new variable of type char
 * @param item  name of the new variable
 * @param strn string
 */
#define foreach_str( ITEM_NAME, STRING )                                      \
    foreach_helper_init( strlen( STRING ), ITEM_NAME ) foreach_helper_assign( \
            char, ITEM_NAME, ( STRING )[ 0 ], ( STRING )[ foreach_index_##ITEM_NAME ] )

#ifdef CLIBS_DYNAMIC_ARRAY_H
/**
 * Iterates over the List<br>
 * Stores each list member in a new variable named ‹item› of type char
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
 *
 * @param SET set
 */
#define foreach_set( SET )                                                      \
    foreach_uni ( const SetEnumeratedEntry, entry, set_get_next( ( SET ), -1 ), \
                  set_get_next( ( SET ), entry.index ), set_size( ( SET ) ) )
#endif //CLIBS_SETS_H


#endif //CLIBS_FOREACH_H
