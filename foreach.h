//
// Created by MacBook on 23.10.2024.
//

#ifndef CLIBS_FOREACH_H
#define CLIBS_FOREACH_H


#define foreach_helper_init( SIZE, TOK )                                 \
    for ( int foreach_keep_##TOK  = true,                                \
              foreach_index_##TOK = 0,                                   \
              foreach_cap_##TOK   = ( SIZE );                            \
          foreach_keep_##TOK && foreach_index_##TOK < foreach_cap_##TOK; \
                                                                         \
          foreach_keep_##TOK = !foreach_keep_##TOK, ++foreach_index_##TOK )

#define foreach_helper_assign( type, item, accessor )              \
    for ( type item           = ( accessor ); foreach_keep_##item; \
          foreach_keep_##item = !foreach_keep_##item )


#define foreach_uni( type, item, accessor, size ) \
    foreach_helper_init( size, item ) foreach_helper_assign( type, item, accessor )

/**
 * Iterates over the ‹array›<br>
 * Stores each array member in a new variable ‹item› (only
 * visible in the scope of the foreach loop)
 * @param type  C-keyword (or typedef/…) of the type of the array items and the new
 * variable
 * @param item  name of the new variable
 * @param array array of type 'type[]'
 */
#define foreach_arr( type, item, array )                              \
    foreach_helper_init( sizeof( array ) / sizeof *( array ), item ) \
            foreach_helper_assign( type, item, ( array )[ foreach_index_##item ] )

/**
 * Iterates over the string ‹strn›<br>
 * Stores each character of the string in a new variable named ‹item› of type char
 * @param item  name of the new variable
 * @param strn string
 */
#define foreach_str( item, strn )               \
    foreach_helper_init( strlen( strn ), item ) \
            foreach_helper_assign( char, item, ( strn )[ foreach_index_##item ] )

/**
 * Iterates over the List<br>
 * Stores each list member in a new variable named ‹item› of type char
 * @param type  type of the new variable and items in the list
 * @param item  name of the new variable
 * @param ls string
 */
#define foreach_ls( type, item, ls )                                    \
    foreach_helper_init( list_size( ls ), item ) foreach_helper_assign( \
            type, item, list_access( ls, foreach_index_##item, type ) )


#endif //CLIBS_FOREACH_H
