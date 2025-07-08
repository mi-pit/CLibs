/**
 * @file set.h
 * @brief
 * Sets implemented via a hash table.
 *
 * Sets contain at most one copy of an item.
 * Items can be of any type -- they are treated as arrays of bytes.
 * Their keys are a combination of the number of bytes and each byte of the data.
 *
 * @param n_items   : `size_t`; number of currently held items in the Set
 * @param capacity  : `size_t`; size of items array
 * @param items     : `struct set_item *`; array of type set_item
 */

#ifndef CLIBS_SETS_H
#define CLIBS_SETS_H

#include "../headers/extra_types.h"  /* stddef, stdint, stdbool */
#include "../item_print_functions.h" /* PrintFunction */


struct hash_set;
typedef struct hash_set Set;


/**
 * `func` is set to `print_byte()` by default
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


/**
 * Initializes a `Set`.
 *
 * @return pointer to a new `Set`
 */
Constructor Set *set_init( void );
/**
 * Initializes a `Set` with a custom capacity.
 *
 * @param capacity initial capacity
 * @return pointer to a new `Set`
 */
Constructor Set *set_init_cap( size_t capacity );

/**
 * Inserts a value into the set.
 *
 * @param data pointer to data
 * @param len number of bytes in data
 * @param func `PrintFunction`
 * @return `RV_ERROR` | `RV_EXCEPTION` | `enum SetInsertRV`
 */
int set_insert_f( Set *, const void *data, size_t len, PrintFunction func );
/**
 * Inserts a value into the set.
 *
 * @param data pointer to data
 * @param len number of bytes in data
 * @return `RV_ERROR` | `RV_EXCEPTION` | `enum SetInsertRV`
 */
int set_insert( Set *, const void *data, size_t len );

/**
 * Removes a value from the set.
 *
 * @param data pointer to data
 * @param len sizeof data
 * @return `RV_ERROR` | `RV_EXCEPTION` | `enum SetRemoveRV`
 */
int set_remove( Set *, const void *data, size_t len );

/**
 * Searches the set for an element.
 *
 * @param data data
 * @param len size of data
 * @return true if present
 */
bool set_search( const Set *, const void *data, size_t len );

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
/**
 * Leaves the set with only the items which are in `intr`.
 *
 * @param intr  Second set
 * @return `RV_ERROR` if `set_remove_item()` fails
 */
int set_intersect( Set *, const Set *intr );

/**
 * Creates a union of the two sets, containing all items from both.
 *
 * @param result pointer to a `Set *`
 * @return `RV_ERROR` on fatal error
 */
int set_union( const Set *, const Set *, Set **result );
/**
 * Adds all items from `add` to the set.
 *
 * @param add second `Set`
 * @return `RV_ERROR` on fatal error
 */
int set_unionize( Set *, const Set *add );

/**
 * Creates a new `Set *` (saved into `result`)
 * with the items which are in `set` but not in `sub`
 *
 * @param set set to be subtracted from
 * @param sub set to be subtracted
 * @param result pointer to a `Set *` (container)
 * @return `RV_ERROR` on error, else `RV_SUCCESS`
 */
int set_difference( const Set *set, const Set *sub, Set **result );
/**
 * `set` is left with the items which are not in `sub`
 *
 * @param set set to be subtracted from
 * @param sub set to be subtracted
 * @return `RV_ERROR` on error, else `RV_SUCCESS`
 */
int set_subtract( Set *set, const Set *sub );


/**
 * Compares sets (pretty arbitrarily). First by size, then by items.
 *
 * @return 1, 0, -1
 */
int set_cmp( const Set *, const Set * );


/** Frees all memory owned by the `Set`. */
void set_destroy( Set * );


/// @return number of items in the set
Const size_t set_size( const Set *set );


typedef struct {
    const struct set_item *item;
    int64_t index;
} SetEnumeratedEntry;
/**
 * Iterator over set.
 *
 * @param index_last index of the last iterated element
 * @return SetEnumeratedEntry (tuple of item* and index)
 */
SetEnumeratedEntry set_get_next( const Set *, int64_t index_last );


#ifndef SET_ITEMCOUNT_LINE_LENGTH
/**
 * Max number of items that can be printed on one line in a set.
 *
 * This may be redefined.
 */
#define SET_ITEMCOUNT_LINE_LENGTH 4
#endif //SET_ITEMCOUNT_LINE_LENGTH

/** Prints each item using the `PrintFunction`s they have saved in them. */
void set_print( const Set * );
/** Prints each item using the supplied `PrintFunction`. */
void set_print_as( const Set *, PrintFunction );

#endif //CLIBS_SETS_H
