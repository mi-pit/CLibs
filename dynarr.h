#ifndef CLIBS_DYNAMIC_ARRAY_H
#define CLIBS_DYNAMIC_ARRAY_H


#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>


#ifndef __result_use_check
#define __result_use_check
#endif //__result_use_check


/* Dynamic array */
typedef struct dynamic_array *List;
/* Immutable List */
typedef const struct dynamic_array *ConstList;


#define LIST_DEF_SIZE 256

#define LIST_CAP_SIZE_RATIO 4

#define LS_PRINT_NOFORMAT ( -1 ) /* Lets each list to choose its own print method */
#define LS_PRINT_BYTE     0 /* default; prints any data as pairs of hexadecimal digits */
#define LS_PRINT_STR      1 /* List contents are strings */
#define LS_PRINT_INT      2 /* List contents are whole numbers */
#define LS_PRINT_DEC      3 /* List contents are rational numbers */
#define LS_PRINT_PTR      4 /* List contents are pointers */
#define LS_PRINT_LIST     5 /* List contents are other lists */


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
List list_init_size( int el_size ) __result_use_check;

/**
 * Initializes the lists items to be of size ‹el_size›.\n
 * \n
 * Sets the print_mode to ‹mode›
 */
List list_init_size_p( int el_size, int mode ) __result_use_check;

/**
 * @returns The element of type ‹type› at the specified index
 */
const void *list_get( ConstList ls, size_t idx );

/**
 *
 * @return Returns the address of the last element
 */
const void *list_peek( ConstList ls );

#define list_access( list, idx, type ) ( *( ( type * ) list_get( list, idx ) ) )

/**
 * @return An address of ‹idx›'th element in the List
 */
void *list_at( List, size_t idx );
void *list_at_last( List );

/**
 * Changes element at ‹index› to ‹data›
 * @return Can return RV_EXCEPTION if index is OOB
 */
int list_set_at( List, size_t index, const void *data );

/**
 * Copies all elements from ‹array› of length ‹len› to the end of the List.
 * @param ls    a List to be extended
 * @param array array of ‹len› elements of size equal to ‹ls.el_size›
 * @param len   the number of elements in the array
 * @return -1 if realloc fails while creating space for the new elements,
 * otherwise returns 0
 */
int list_extend_array( List, const void *array, size_t len );

/**
 *
 * @param ls    a List to be extended
 * @param app   a List to extend ‹ls›
 * @return -1 if realloc fails while creating space for the new elements,
 * otherwise returns 0
 */
int list_extend_list( List ls, ConstList app );

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
 * list_append( ls_inner, &ls_outer );
 * @endcode
 */
int list_append( List, const void *datap );

int list_insert( List, size_t index, const void *data );


/**
 * Removes the last element from the List, puts the result as_new ‹container›.\n
 * If ‹container› is NULL, list_pop() discards the last element
 * @param ls pointer to a valid List struct
 * @param container pointer to valid space in memory
 * of at least ‹ls.el_size› bytes
 * @return 1 if List is empty, -1 if malloc fails; else 0
 */
int list_pop( List, void *container );

/**
 * Removes the element at ‹index› by swapping it with
 * the last element and popping
 */
int list_remove_fast( List, size_t index, void *container );

/**
 * Removes the element at ‹index› by moving the following elements
 * back by one space
 */
int list_remove( List, size_t index, void *container );


const void *list_bsearch_p( ConstList,
                            const void *needle,
                            int ( *cmp )( const void *, const void * ) );

int64_t list_bsearch_i( ConstList,
                        const void *needle,
                        int ( *cmp )( const void *, const void * ) );

const void *list_lsearch_p( ConstList, const void *needle );

int64_t list_lsearch_i( ConstList, const void *needle );

/**
 * qsort
 * @param ls    List to be sorted
 * @param cmp   compare function for the elements of the List
 */
void list_sort( List, int ( *cmp )( const void *, const void * ) );


/**
 * Creates a copy of the List and stores it in ‹new_ls_cont›
 * @param ls old List to be copied
 * @param new_ls_cont pointer to your place in memory to store a List pointer
 * @return  (-1) if an error occurs, else 0
 */
int list_copy( ConstList, List *new_ls_container );

/**
 * Returns a copy of the original List, if an error occurs while
 * creating it, the function exits
 */
List list_copy_p( ConstList ) __result_use_check;

/**
 * Reverses the List – in place
 * @return  (-1) if an error occurs, else 0
 */
int list_reverse( List );

/**
 * Returns a new List with the same items as the original, in a reverse order
 */
List list_reversed( ConstList ) __result_use_check;

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
 * Frees all memory owned by the List
 */
void list_destroy( List );


/* ––––––––––––––––––––––––––––––– PRINTERS ––––––––––––––––––––––––––––––– */

/* ––––– Array Print ––––– */

/**
 * Prints the array of a hash_set length and type to a specified file descriptor.\n
 * \n
 * Each item is printed according to the ‹format_str› (as in printf)\n
 * Array starts with '[' and ends with ']\\n'\n
 * Items in array are separated by ‹delim›\n
 * \n
 * If the format string contains "%s", array_printf() prints the whole string,
 * without any delims also formatted according to the format string.\n
 * However, it doesn't work with variable length specifiers. (e.g. "%.*s")\n
 */
#define array_printf_d( array, len, type, format_str, delim )                      \
    do                                                                             \
    {                                                                              \
        if ( strstr( format_str, "\\%s" ) )                                        \
        {                                                                          \
            const char *print_array_format__ = format_str;                         \
            printf( print_array_format__, ( char * ) array );                      \
        }                                                                          \
        else                                                                       \
        {                                                                          \
            printf( "[ " );                                                        \
            for ( size_t __print_array_idx__ = 0; __print_array_idx__ < len;       \
                  ++__print_array_idx__ )                                          \
            {                                                                      \
                printf( format_str, ( ( type * ) array )[ __print_array_idx__ ] ); \
                if ( __print_array_idx__ != len - 1 )                              \
                    printf( "%s", delim );                                         \
            }                                                                      \
            printf( " ]\n" );                                                      \
        }                                                                          \
    }                                                                              \
    while ( 0 )

/**
 * Prints the array of a set length and type.\n
 * \n
 * Each item is printed according to the ‹format_str› (as in printf)\n
 * Array starts with '[' and ends with ']\\n', items being separated by ", "\n
 * \n
 * If the format string contains "%s", array_printf() prints the whole string,
 * without any delims also formatted according to the format string.\n
 * However, it doesn't work with variable length specifiers. (e.g. "%.*s")\n
*/
#define array_printf( array, len, type, format ) \
    array_printf_d( array, len, type, format, ", " )


/* ––––– List Print –––––*/

#define list_printf_d( ls, type, format, delim ) \
    array_printf_d( list_items( ls ), list_size( ls ), type, format, delim )

#define list_printf( ls, type, format ) list_printf_d( ls, type, format, ", " )


/* ––––– Pre–Formatted ––––– */

/**
 * Prints the contents of the List in one of the following formats:\n
 * \n
 * LS_PRINT_NOFORMAT  (-1)-- prints each item according to its def_print_mode field\n
 * LS_PRINT_BYTE        0 -- prints two hex digits per byte\n
 * LS_PRINT_CHAR        1 -- prints contents as a string\n
 * LS_PRINT_INT         2 -- prints a whole number (ssize_t)\n
 * LS_PRINT_DEC         3 -- prints a decimal number (double)\n
 * LS_PRINT_PTR         4 -- prints an address (void *) as `@%p'\n
 * LS_PRINT_LIST        5 -- recursively calls list_printm()
 *                                   on each sub-List\n
 * \n
 * In debug mode, this prints out the whole capacity
 * along with header information.\n
 * @deprecated
 * list_print, list_printm, list_prints
 * are bad, just use list_printf\n
 * \n
 * This function is sort of an afterthought and
 * probably doesn't work very well\n
 */
void list_print( ConstList ) __deprecated;

void list_printm( ConstList, int print_mode );

/**
 * Works like list_printm(), but also prints the List size
 */
void list_prints( ConstList, int print_mode );

/* ––––– GETTERS ––––– */

size_t list_size( ConstList );

size_t list_el_size( ConstList );

const void *list_items( ConstList ls );
void *list_items_copy( ConstList ls ) __result_use_check;

void list_set_print_mode( List ls, int fmt );


#endif //CLIBS_DYNAMIC_ARRAY_H
