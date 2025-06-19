/*
 * List
 *
 * For more info see `docs/dynarr.md`
 */

#ifndef CLIBS_DYNAMIC_ARRAY_H
#define CLIBS_DYNAMIC_ARRAY_H

#include "../headers/array_printf.h" /* for list_printf */
#include "../headers/attributes.h"   /* Private */

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>


struct dynamic_array; // defined in dynarr.c
typedef struct dynamic_array List;


#define LIST_DEF_SIZE 64

#define LIST_CAP_SIZE_RATIO 2


enum ListPrinters {
    LS_PRINT_NOFORMAT = -1, /* Lets each list to choose its own print method */
    LS_PRINT_BYTE     = 0,  /* default;
                             * prints any data as pairs of hexadecimal digits */
    LS_PRINT_STR,           /* List contents are strings */
    LS_PRINT_SIGNED,        /* List contents are signed whole numbers */
    LS_PRINT_UNSIGNED,      /* List contents are unsigned whole numbers */
    LS_PRINT_DEC,           /* List contents are rational numbers */
    LS_PRINT_PTR,           /* List contents are pointers */
    LS_PRINT_LIST,          /* List contents are other lists */
};

/**
 * Initializes the List to be of type ‹type›
 */
#define list_init_type( type ) list_init_size( sizeof( type ) )
/**
 * Initializes the List to be of type ‹type›\n
 * \n
 * Sets the print_mode to ‹pt›
 */
#define list_init_type_p( type, print_mode ) \
    list_init_size_p( sizeof( type ), print_mode )
/**
 * Initializes the lists items to be of size ‹el_size›.\n
 */
Constructor struct dynamic_array *list_init_size( size_t el_size );
/**
 * Initializes the lists items to be of size ‹el_size›.\n
 * \n
 * Sets the print_mode to ‹mode›
 */
Constructor struct dynamic_array *list_init_size_p( size_t el_size, int mode );

/**
 * Creates a copy of the List and stores it in ‹new_ls_cont›
 * @param old               old List to be copied
 * @param new_ls_container  pointer to your place in memory to store a List pointer
 * @return RV_ERROR if an error occurs, else RV_SUCCESS
 */
int list_copy( const struct dynamic_array *old, struct dynamic_array **new_ls_container );
/**
 * Creates a copy of the original List
 * @return newly allocated list pointer (see `list_init_*`)
 *         with a shallow copy of the data inside.
 *         This list may be passed to `free()`
 */
Constructor struct dynamic_array *list_copy_p( const struct dynamic_array * );


/**
 * Gets a non-mutable look of the element at the specified index
 *
 * @returns \code const void *\endcode to the element
 */
const void *list_see( const struct dynamic_array *ls, size_t idx );
/**
 * Gets a non-mutable look of the last element
 *
 * @return \code const void *\endcode to the last element
 */
const void *list_peek( const struct dynamic_array *ls );

/**
 * Fetches an item at the specified index.
 * OOB index dereferences a NULL pointer (so don't do that).
 * Type must be the same as the list element type.
 */
#define list_fetch( LIST, IDX, TYPE ) \
    ( *( ( const TYPE * ) list_see( ( LIST ), ( IDX ) ) ) )

/**
 * Gets a mutable look of the element at the specified index
 *
 * @return pointer to the element
 */
void *list_at( struct dynamic_array *, size_t idx );
/**
 * Gets a mutable look of the last element
 *
 * @return pointer to the last element
 */
void *list_at_last( struct dynamic_array * );

/**
 * Accesses the list as if it was an array – like ARRAY[IDX] => assignment is possible.
 * OOB index dereferences a NULL pointer (so don't do that).
 * Type must be the same as the list element type.
 */
#define list_access( LIST, IDX, TYPE ) ( *( ( TYPE * ) list_at( ( LIST ), ( IDX ) ) ) )

/**
 * Changes element at ‹index› to ‹data›
 * @return Can return RV_EXCEPTION if index is OOB
 */
int list_set_at( struct dynamic_array *, size_t index, const void *data );

/**
 * Copies all elements from ‹array› of length ‹len› to the end of the List.
 *
 * @param ls    a List to be extended
 * @param array array of ‹len› elements of size equal to ‹ls.el_size›
 * @param array_len   the number of elements in the array
 * @return -1 if realloc fails while creating space for the new elements,
 * otherwise returns 0
 */
int list_extend( struct dynamic_array *ls, const void *array, size_t array_len );
/**
 * Copies all elements from `app` to `ls`
 *
 * @param ls    a List to be extended
 * @param app   a List to extend ‹ls›
 * @return RV_ERROR if realloc fails while creating space for the new elements,
 * otherwise returns 0
 */
int list_extend_list( struct dynamic_array *ls, const struct dynamic_array *app );
/**
 * Appends an element to the end of the List.\n
 * The function assumes ‹data›
 * is of the same type as the List
 * @return -1 if reallocation fails, 0 otherwise
 *
 * @example
 * @code
 * List ls = list_init_type( int );
 * int number = 123;
 * list_append( ls, &number ); // note the `&`
 * // or
 * List ls_outer = list_init_type( List );
 * List ls_inner = list_init_type( char );
 * list_append( ls_outer, &ls_inner );
 * @endcode
 */
int list_append( struct dynamic_array *, const void *datap );
/**
 * Inserts data to list at specified index, all following data is moved one place forward
 * @param index index of new element
 * @param data  pointer to data of size ls::el_size
 * @return  if index is OOB  -> RV_EXCEPTION,
 *          if realloc fails -> RV_ERROR
 *          otherwise        -> RV_SUCCESS
 */
int list_insert( struct dynamic_array *, size_t index, const void *data );

/**
 * Removes the last element from the List, puts the result as_new ‹container›.\n
 * If ‹container› is NULL, list_pop() discards the last element
 * @param ls pointer to a valid List struct
 * @param container pointer to valid space in memory
 * of at least ‹ls.el_size› bytes
 * @return RV_EXCEPTION if List is empty, RV_ERROR if malloc fails; else RV_SUCCESS (0)
 */
int list_pop( struct dynamic_array *ls, void *container );
/**
 * Removes the element at ‹index› by swapping it with
 * the last element and popping
 */
int list_remove_fast( struct dynamic_array *, size_t index, void *container );
/**
 * Removes the element at ‹index› by moving the following elements
 * back by one space
 */
int list_remove( struct dynamic_array *, size_t index, void *container );

/// Binary search, returns pointer (NULL if not found)
const void *list_bsearch_p( const struct dynamic_array *,
                            const void *needle,
                            int ( *cmp )( const void *, const void * ) );
/// Binary search, returns index (-1 if not found)
int64_t list_bsearch_i( const struct dynamic_array *,
                        const void *needle,
                        int ( *cmp )( const void *, const void * ) );

/// Linear search, returns pointer (NULL if not found)
const void *list_lsearch_p( const struct dynamic_array *, const void *needle );
/// Linear search, returns index (-1 if not found)
int64_t list_lsearch_i( const struct dynamic_array *, const void *needle );

/**
 * Uses stdlib's qsort
 *
 * @param ls    List to be sorted
 * @param cmp   compare function for the elements of the List
 */
void list_sort( struct dynamic_array *ls, int ( *cmp )( const void *, const void * ) );

/**
 * Reverses the List – in place
 * @return RV_ERROR if an error occurs, else RV_SUCCESS
 */
int list_reverse( struct dynamic_array * );

/**
 * Returns a new List with the same items as the original, in a reverse order
 */
UseResult struct dynamic_array *list_reversed( const struct dynamic_array * );

/**
 * Compares the lists based on the number of elements,
 * similar to memcmp/strcmp
 */
int list_cmp_size( const void *l1, const void *l2 );

/**
 * Compares the lists based on the size of elements,
 * similar to memcmp/strcmp
 */
int list_cmp_elsize( const void *l1, const void *l2 );

/**
 * Frees all memory owned by the List, including itself
 */
void list_destroy( struct dynamic_array * );

/**
 * Resets the list to the defaults
 */
void list_clear( struct dynamic_array *ls );


/* ––––––––––––––––––––––––––––––– PRINTERS ––––––––––––––––––––––––––––––– */

///@see \code array_printf_d\endcode
#define list_printf_sde( LIST, ITEM_TYPE, FORMAT, START_STR, DELIM, END_STR ) \
    array_printf_sde( list_items( LIST ),                                     \
                      list_size( LIST ),                                      \
                      ITEM_TYPE,                                              \
                      FORMAT,                                                 \
                      START_STR,                                              \
                      DELIM,                                                  \
                      END_STR )

///@see \code array_printf_d\endcode
#define list_printf_d( LIST, ITEM_TYPE, FORMAT, DELIM ) \
    array_printf_d( list_items( LIST ), list_size( LIST ), ITEM_TYPE, FORMAT, DELIM )

///@see \code array_printf_d\endcode
#define list_printf( LIST, ITEM_TYPE, FORMAT ) \
    array_printf( list_items( LIST ), list_size( LIST ), ITEM_TYPE, FORMAT )


#define list_sprintf_d( STRING, LIST, TYPE, FMTSTR, DELIM ) \
    array_sprintf_d( STRING, list_items( LIST ), list_size( LIST ), TYPE, FMTSTR, DELIM )

#define list_sprintf( STRING, LIST, TYPE, FMTSTR ) \
    array_sprintf( STRING, list_items( LIST ), list_size( LIST ), TYPE, FMTSTR )


/* ––––– Pre–Formatted ––––– */

/**
 * Prints the contents of the List in one of the following formats:
 * @code
 * LS_PRINT_NOFORMAT = ( -1 ), // Lets each list to choose its own print method
 * LS_PRINT_BYTE     = 0,      // default;
 *                             // prints any data as pairs of hexadecimal digits
 * LS_PRINT_STR      = 1,      // List contents are strings
 * LS_PRINT_INT      = 2,      // List contents are whole numbers
 * LS_PRINT_DEC      = 3,      // List contents are rational numbers
 * LS_PRINT_PTR      = 4,      // List contents are pointers
 * LS_PRINT_LIST     = 5,      // List contents are other lists
 * @endcode
 *
 * @deprecated
 * list_print, list_printm, list_prints
 * are bad, just use list_printf\n
 * \n
 * This function is sort of an afterthought and
 * probably doesn't work very well\n
 */
Deprecated void list_print( const struct dynamic_array * );
Deprecated void list_printm( const struct dynamic_array *, int print_mode );
/**
 * Works like list_printm(), but also prints the List size
 */
Deprecated void list_prints( const struct dynamic_array *, int print_mode );

/* ––––– GETTERS ––––– */

/// Returns true if list is empty
bool list_is_empty( const struct dynamic_array * );
/// Returns number of elements in the list
size_t list_size( const struct dynamic_array * );
/// Returns sizeof elements (e.g. if list element type is char, returns 1)
size_t list_el_size( const struct dynamic_array * );

const void *list_items( const struct dynamic_array *ls );
UseResult void *list_items_copy( const struct dynamic_array *ls );

void list_set_print_mode( struct dynamic_array *ls, int fmt );


#endif //CLIBS_DYNAMIC_ARRAY_H
