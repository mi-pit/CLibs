/**
 * @file dynarr.h
 * @brief
 * A dynamic array. (also called `List`, `Vector` or `ArrayList`)
 *
 * The `List` can store any elements of arbitrary size.
 *
 * A new `List` can (and should) be created with `list_init_*` functions
 *
 * `list_init_type()` function takes as its `type` argument the C keyword
 * e.g. `char`
 * <br>
 * `list_init_size()` function takes as its `el_size` argument
 * the number of bytes (acquired by `sizeof`) for the desired type to be stored
 *
 * `list_init_*` functions allocate and initialize a List
 * with these default values:
 * @code
 * List::capacity = LIST_DEF_CAP
 * List::size     = 0
 * List::items    = pointer to heap allocated memory of width ‹capacity›, set to 0
 * List::el_size  = width of a single element
 * @endcode
 *
 * Finalization (destruction) of a List is done with `list_destroy()`.
 *
 * @attention All elements must be the same number of bytes long
 */

#ifndef CLIBS_DYNAMIC_ARRAY_H
#define CLIBS_DYNAMIC_ARRAY_H

#include "../headers/attributes.h"  /* Private */
#include "../headers/extra_types.h" /* size_t, int*_t */

#include <stdbool.h>


struct dynamic_array; // defined in dynarr.c
typedef struct dynamic_array List;


/**
 * Initializes the List to be of type `TYPE`
 */
#define list_init_type( TYPE ) list_init_size( sizeof( TYPE ) )
/**
 * Initializes the lists elements to be of size `el_size`
 *
 * @param el_size `sizeof()` a single element
 * @return pointer to a new empty List, or `NULL` if allocation fails
 */
Constructor struct dynamic_array *list_init_size( size_t el_size );
/**
 * Creates a new list
 *
 * @param el_size   sizeof a single element
 * @param init_cap  initial capacity in terms of elements (rather than bytes)
 * @return pointer to a new empty List, or `NULL` if allocation fails
 */
Constructor struct dynamic_array *list_init_cap_size( size_t el_size, size_t init_cap );

/**
 * Creates a copy of the List and stores it in `new_ls_container`
 * @param old               old List to be copied
 * @param new_ls_container  pointer to your place in memory to store a List pointer
 * @return RV_ERROR if an error occurs, else RV_SUCCESS
 */
int list_copy( const struct dynamic_array *old, struct dynamic_array **new_ls_container );
/**
 * Creates a copy of the original List
 * @return newly allocated list pointer (see `list_init_*`)
 *         with a shallow copy of the data inside.
 *         This list may be passed to `list_destroy()`
 */
Constructor struct dynamic_array *list_copy_of( const struct dynamic_array * );


#if defined( CLIBS_SETS_H ) || defined( CLIBS_STRUCT_CONVERSIONS )
// fixme?
#include "set.h"
Constructor List *list_from_set( const Set *set );
#endif


/**
 * Gets a non-mutable look of the element at the specified index
 *
 * @returns `const void *` to the element
 */
const void *list_see( const struct dynamic_array *ls, size_t idx );
/**
 * Gets a non-mutable look of the last element
 *
 * @return `const void *` to the last element
 */
const void *list_peek( const struct dynamic_array *ls );

/**
 * Fetches an item at the specified index.
 * Type must be the same as the list element type.
 *
 * OOB index dereferences a NULL pointer (so don't do that).
 */
#define list_fetch( LIST, IDX, TYPE ) \
    ( *( ( TYPE const * ) list_see( ( LIST ), ( IDX ) ) ) )

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
 * Accesses the list as if it was an array – like `ARRAY[IDX]` => assignment is possible.
 * Type must be the same as the list element type.
 *
 * OOB index dereferences a NULL pointer (so don't do that).
 */
#define list_access( LIST, IDX, TYPE ) ( *( ( TYPE * ) list_at( ( LIST ), ( IDX ) ) ) )

/**
 * Changes element at `index` to `data`
 *
 * @return Can return `RV_EXCEPTION` if index is OOB, else `RV_SUCCESS`
 */
int list_set_at( struct dynamic_array *, size_t index, const void *data );

/**
 * Copies all elements from `array` of length `array_len` to the end of the List.
 *
 * @param ls        a List to be extended
 * @param array     array of `array_len` elements of size equal to `ls::el_size`
 * @param array_len the number of elements in the array
 * @return `RV_ERROR` if realloc fails while creating space for the new elements,
 * otherwise returns `RV_SUCCESS`
 */
int list_extend( struct dynamic_array *ls, const void *array, size_t array_len );
/**
 * Copies all elements from `app` to `ls`
 *
 * @param ls    a List to be extended
 * @param app   a List to extend `ls`
 * @return `RV_ERROR` if realloc fails while creating space for the new elements,
 * otherwise returns `RV_SUCCESS`
 */
int list_extend_list( struct dynamic_array *ls, const struct dynamic_array *app );
/**
 * Appends an element to the end of the List.
 *
 * The function assumes `data` is of the same type as the List.
 *
 * Example:
 * @code
 * List ls = list_init_type( int );
 * int number = 123;
 * list_append( ls, &number ); // note the `&`
 * // or
 * List ls_outer = list_init_type( List );
 * List ls_inner = list_init_type( char );
 * list_append( ls_outer, &ls_inner );
 * @endcode
 *
 * @return `RV_ERROR` if reallocation fails, `RV_SUCCESS` otherwise
 */
int list_append( struct dynamic_array *, const void *datap );
/**
 * Inserts data to list at specified index, all following data is moved one place forward
 * @param index index of new element
 * @param data  pointer to data of size `ls::el_size`
 * @return
 * - if index is OOB  -> `RV_EXCEPTION`
 * - if realloc fails -> `RV_ERROR`
 * - otherwise        -> `RV_SUCCESS`
 */
int list_insert( struct dynamic_array *, size_t index, const void *data );

/**
 * Removes the last element from the List, puts the result into `container`.
 *
 * If `container` is NULL, this method discards the last element.
 *
 * @param container pointer to valid space in memory
 * of at least `ls::el_size` bytes
 * @return :
 * - `RV_EXCEPTION` if List is empty
 * - `RV_ERROR` if malloc fails
 * - else `RV_SUCCESS`
 */
int list_pop( struct dynamic_array *, void *container );
/**
 * Removes the element at `index` by swapping it with
 * the last element and popping.
 *
 * If `container` is NULL, this method discards the last element.
 *
 * @param index
 * @param container pointer to valid space in memory
 * of at least `ls::el_size` bytes
 * @return
 * - `RV_EXCEPTION` if `index` is OOB
 * - `RV_ERROR` if alloc fails
 * - else RV_SUCCESS
 */
int list_remove_fast( struct dynamic_array *, size_t index, void *container );
/**
 * Removes the element at `index` by moving the following elements
 * back by one space.
 *
 * If `container` is NULL, this method discards the last element.
 *
 * @param index
 * @param container pointer to valid space in memory
 * of at least `ls::el_size` bytes
 * @return
 * - `RV_EXCEPTION` if `index` is OOB
 * - `RV_ERROR` if alloc fails
 * - else RV_SUCCESS
 */
int list_remove( struct dynamic_array *, size_t index, void *container );

/**
 * Binary search, returns pointer (NULL if not found)
 */
const void *list_bsearch_p( const struct dynamic_array *,
                            const void *needle,
                            int ( *cmp )( const void *, const void * ) );
/**
 * Binary search, returns index (-1 if not found)
 */
int64_t list_bsearch_i( const struct dynamic_array *,
                        const void *needle,
                        int ( *cmp )( const void *, const void * ) );

/**
 * Linear search, returns pointer (NULL if not found)
 */
const void *list_lsearch_p( const struct dynamic_array *, const void *needle );
/**
 * Linear search, returns index (-1 if not found)
 */
int64_t list_lsearch_i( const struct dynamic_array *, const void *needle );

/**
 * Uses stdlib's qsort.
 *
 * @param ls    List to be sorted
 * @param cmp   compare function for the elements of the List
 *
 * @see `qsort` from `<stdlib.h>`
 */
void list_sort( struct dynamic_array *ls, int ( *cmp )( const void *, const void * ) );

/**
 * Reverses the List – in place
 * @return RV_ERROR if an error occurs, else RV_SUCCESS
 */
int list_reverse( struct dynamic_array * );

/**
 * Creates a new List
 *
 * @return a new List with the same items as the original, in a reverse order
 */
Constructor struct dynamic_array *list_reversed( const struct dynamic_array * );

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
int list_clear( struct dynamic_array *ls );


/* ––––––––––––––––––––––––––––––– PRINTERS ––––––––––––––––––––––––––––––– */

///@see `array_fprintf_sde`
#define list_printf_sde( LIST, ITEM_TYPE, FORMAT, START_STR, DELIM, END_STR ) \
    array_printf_sde( list_items( LIST ),                                     \
                      list_size( LIST ),                                      \
                      ITEM_TYPE,                                              \
                      FORMAT,                                                 \
                      START_STR,                                              \
                      DELIM,                                                  \
                      END_STR )

///@see `array_fprintf_sde`
#define list_printf_d( LIST, ITEM_TYPE, FORMAT, DELIM ) \
    array_printf_d( list_items( LIST ), list_size( LIST ), ITEM_TYPE, FORMAT, DELIM )

///@see `array_fprintf_sde`
#define list_printf( LIST, ITEM_TYPE, FORMAT ) \
    array_printf( list_items( LIST ), list_size( LIST ), ITEM_TYPE, FORMAT )


/// @see `array_sprintf_sde`
#define list_sprintf_d( STRING, LIST, TYPE, FMTSTR, DELIM ) \
    array_sprintf_d( STRING, list_items( LIST ), list_size( LIST ), TYPE, FMTSTR, DELIM )

/// @see `array_sprintf_sde`
#define list_sprintf( STRING, LIST, TYPE, FMTSTR ) \
    array_sprintf( STRING, list_items( LIST ), list_size( LIST ), TYPE, FMTSTR )


/* ––––– GETTERS ––––– */

/// Returns true if list is empty
bool list_is_empty( const struct dynamic_array * );
/// Returns number of elements in the list
size_t list_size( const struct dynamic_array * );
/// Returns sizeof elements (e.g. if list element type is `char`, returns 1)
size_t list_el_size( const struct dynamic_array * );

/**
 * Gets a `const` view of the items.
 *
 * @return A pointer to the lists items array.
 */
const void *list_items( const struct dynamic_array * );
/**
 * Gets the lists items as an array. This pointer should be `free`d
 *
 * @return A pointer to a copy of the lists items array.
 */
UseResult void *list_items_copy( const struct dynamic_array * );


#endif //CLIBS_DYNAMIC_ARRAY_H
