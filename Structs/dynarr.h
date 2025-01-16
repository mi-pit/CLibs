#ifndef CLIBS_DYNAMIC_ARRAY_H
#define CLIBS_DYNAMIC_ARRAY_H

#include "../array_printf.h"   /* for list_printf */
#include "../Dev/attributes.h" /* Private */

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>


struct dynamic_array; // defined in dynarr.c
typedef struct dynamic_array List;


#define LIST_DEF_SIZE 256

#define LIST_CAP_SIZE_RATIO 4


enum ListPrinters {
    LS_PRINT_NOFORMAT = ( -1 ), /* Lets each list to choose its own print method */
    LS_PRINT_BYTE     = 0,      /* default;
                                 * prints any data as pairs of hexadecimal digits */
    LS_PRINT_STR      = 1,      /* List contents are strings */
    LS_PRINT_INT      = 2,      /* List contents are whole numbers */
    LS_PRINT_DEC      = 3,      /* List contents are rational numbers */
    LS_PRINT_PTR      = 4,      /* List contents are pointers */
    LS_PRINT_LIST     = 5,      /* List contents are other lists */
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
UseResult struct dynamic_array *list_init_size( size_t el_size );
/**
 * Initializes the lists items to be of size ‹el_size›.\n
 * \n
 * Sets the print_mode to ‹mode›
 */
UseResult struct dynamic_array *list_init_size_p( size_t el_size, int mode );

/**
 * @returns The element of type ‹type› at the specified index
 */
const void *list_see( const struct dynamic_array *ls, size_t idx );
/**
 *
 * @return Returns the address of the last element
 */
const void *list_peek( const struct dynamic_array *ls );

#define list_access( list, idx, type ) ( *( ( type * ) list_see( list, idx ) ) )

/**
 * @return An address of ‹idx›'th element in the List
 */
void *list_at( struct dynamic_array *, size_t idx );
void *list_at_last( struct dynamic_array * );

/**
 * Changes element at ‹index› to ‹data›
 * @return Can return RV_EXCEPTION if index is OOB
 */
int list_set_at( struct dynamic_array *, size_t index, const void *data );

/**
 * Copies all elements from ‹array› of length ‹len› to the end of the List.
 * @param ls    a List to be extended
 * @param array array of ‹len› elements of size equal to ‹ls.el_size›
 * @param len   the number of elements in the array
 * @return -1 if realloc fails while creating space for the new elements,
 * otherwise returns 0
 */
int list_extend( struct dynamic_array *, const void *array, size_t array_len );
/**
 *
 * @param ls    a List to be extended
 * @param app   a List to extend ‹ls›
 * @return -1 if realloc fails while creating space for the new elements,
 * otherwise returns 0
 */
int list_extend_list( struct dynamic_array *ls, const struct dynamic_array *app );
/**
 * Appends an element to the end of the List.\n
 * The function assumes ‹data›
 * is of the same type as the List
 * @return -1 if reallocation fails, 0 otherwise
 * @example @code
 * List ls = list_init( int );
 * int number = 123;
 * list_append( ls, &number );
 *
 * List ls_outer = list_init( List );
 * List ls_inner = list_init( char );
 * list_append( ls_outer, &ls_inner );
 * @endcode
 */
int list_append( struct dynamic_array *, const void *datap );
/**
 * Inserts data to list at specified index, all following data is moved one place forward
 * @param index index of new element
 * @param data  pointer to data of size ls::el_size
 * @return RV_SUCCESS
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
int list_pop( struct dynamic_array *, void *container );
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


const void *list_bsearch_p( const struct dynamic_array *,
                            const void *needle,
                            int ( *cmp )( const void *, const void * ) );
int64_t list_bsearch_i( const struct dynamic_array *,
                        const void *needle,
                        int ( *cmp )( const void *, const void * ) );

const void *list_lsearch_p( const struct dynamic_array *, const void *needle );
int64_t list_lsearch_i( const struct dynamic_array *, const void *needle );

/**
 * qsort
 * @param ls    List to be sorted
 * @param cmp   compare function for the elements of the List
 */
void list_sort( struct dynamic_array *, int ( *cmp )( const void *, const void * ) );


/**
 * Creates a copy of the List and stores it in ‹new_ls_cont›
 * @param ls            old List to be copied
 * @param new_ls_cont   pointer to your place in memory to store a List pointer
 * @return RV_ERROR if an error occurs, else RV_SUCCESS
 */
int list_copy( const struct dynamic_array *, struct dynamic_array **new_ls_container );

/**
 * Returns a copy of the original List, if an error occurs while
 * creating it, the function exits
 */
UseResult struct dynamic_array *list_copy_p( const struct dynamic_array * );

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
 * Frees all memory owned by the List pointed to in *lsp and sets *lsp to NULL
 */
void list_destroy_p( struct dynamic_array **lsp );


/* ––––––––––––––––––––––––––––––– PRINTERS ––––––––––––––––––––––––––––––– */

///@see \code array_printf_d
#define list_printf_d( ls, type, format, delim ) \
    array_printf_d( list_items( ls ), list_size( ls ), type, format, delim )

///@see \code array_printf_d
#define list_printf( ls, type, format ) list_printf_d( ls, type, format, ", " )


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
void list_print( const struct dynamic_array * );
void list_printm( const struct dynamic_array *, int print_mode );
/**
 * Works like list_printm(), but also prints the List size
 */
void list_prints( const struct dynamic_array *, int print_mode );

/* ––––– GETTERS ––––– */

size_t list_size( const struct dynamic_array * );

size_t list_el_size( const struct dynamic_array * );

const void *list_items( const struct dynamic_array *ls );
UseResult void *list_items_copy( const struct dynamic_array *ls );

void list_set_print_mode( struct dynamic_array *ls, int fmt );


#endif //CLIBS_DYNAMIC_ARRAY_H
