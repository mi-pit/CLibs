#ifndef CLIBS_SETS_H
#define CLIBS_SETS_H

#include "../headers/extra_types.h"  /* stddef, stdint, stdbool */
#include "../item_print_functions.h" /* PrintFunction */


typedef struct hash_set Set;


#define SET_DEFAULT_CAP 64

/**
 * func is set to print_byte by default
 */
struct set_item {
    void *data;
    size_t size;

    bool removed;

    PrintFunction func;
};


enum SetInsertRV {
    SETINSERT_INSERTED = 0,
    SETINSERT_WAS_IN   = 1,
};

enum SetRemoveRV {
    SETREMOVE_REMOVED   = 0,
    SETREMOVE_NOT_FOUND = 1,
};


Set *set_init( void );
Set *set_init_cap( size_t capacity );

int set_insert_f( Set *set, const void *data, size_t len, PrintFunction func );
int set_insert( Set *set, const void *data, size_t len );
int set_insert_item( Set *set, const struct set_item *item );
int set_insert_array( Set *set, size_t len, const struct set_item set_item_array[ len ] );

int set_remove( Set *set, const void *data, size_t len );
int set_remove_item( Set *set, const struct set_item *item );
int set_remove_array( Set *set, size_t len, const struct set_item set_item_array[ len ] );

bool set_search( const Set *set, const void *data, size_t len );
bool set_search_item( const Set *set, const struct set_item *item );

/**
 * Creates an intersection of the two sets and shallowly copies the data to
 * a new set, stored under `result`
 *
 * @param result    if the val at result is NULL, the function creates a new
 * hash set with the proper values. If the val isn't NULL, the function
 * adds the intersecting values to the hash set in result
 * @return -1 on error, else 0
 */
int set_intersection( const Set *, const Set *, Set **result );
int set_intersect( Set *set, const Set *intr );

int set_union( const Set *set_1, const Set *set_2, Set **result );
int set_unionize( Set *set, const Set *add );

int set_difference( const Set *, const Set *sub, Set **result );
int set_subtract( Set *, const Set *sub );


/**
 * Compares sets (pretty arbitrarily). First by size, then by items.
 *
 * @return 1, 0, -1
 */
int set_cmp( const Set *, const Set * );


void set_destroy( Set * );
void set_destroy_n( int n, ... );


size_t set_size( const Set *set );
size_t set_capacity( const Set *set );


typedef struct {
    const struct set_item *item;
    const int64_t index;
} SetEnumeratedEntry;

/**
 * Iterator over set
 *
 * @param set set
 * @param index_last index of the last iterated element
 * @return SetEnumeratedEntry (tuple of item* and index)
 */
SetEnumeratedEntry set_get_next( const Set *set, int64_t index_last );


#ifndef SET_ITEMCOUNT_LINE_LENGTH
#define SET_ITEMCOUNT_LINE_LENGTH 4
#endif //SET_ITEMCOUNT_LINE_LENGTH

/**
 *
 */
void set_print( const Set * );
void set_print_as( const Set *, PrintFunction );

#endif //CLIBS_SETS_H
