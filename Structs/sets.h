#ifndef CLIBS_SETS_H
#define CLIBS_SETS_H

#include "item_print_functions.h" /* PrintFunction */

#include <stdbool.h>
#include <stddef.h>


typedef struct hash_set *Set;
typedef const struct hash_set *ConstSet;


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
    SETINSERT_INSERTED = 1,
    SETINSERT_WAS_IN   = 2,
};

enum SetRemoveRV {
    SETREMOVE_REMOVED   = 1,
    SETREMOVE_NOT_FOUND = 2,
};


Set set_init( void );
Set set_init_cap( size_t capacity );

int set_insert_f( Set set, const void *data, size_t len, PrintFunction func );
int set_insert( Set set, const void *data, size_t len );
int set_insert_item( Set set, const struct set_item *item );
int set_insert_array( Set set, size_t len, const struct set_item set_item_array[ len ] );

int set_remove( Set set, const void *data, size_t len );
int set_remove_item( Set set, const struct set_item *item );
int set_remove_array( Set set, size_t len, const struct set_item set_item_array[ len ] );

bool set_search( ConstSet set, const void *data, size_t len );
bool set_search_item( ConstSet set, const struct set_item *item );

/**
 *
 * @param set_1
 * @param set_2
 * @param result    if the val at result is NULL, the function creates a new
 * hash set with the proper values. If the val isn't NULL, the function
 * adds the intersecting values to the hash set in result
 * @return -1 on error, else 0
 */
int set_intersection( ConstSet set_1, ConstSet set_2, Set *result );
int set_intersect( Set set, ConstSet intr );

int set_union( ConstSet set_1, ConstSet set_2, Set *result );
int set_unionize( Set set, ConstSet add );

int set_difference( ConstSet, ConstSet set_2, Set *result );
int set_subtract( Set, ConstSet sub );


int set_cmp( ConstSet set_1, ConstSet set_2 );


void set_destroy( Set );
void set_destroy_n( int n, ... );


#ifndef SET_ITEMCOUNT_LINE_LENGTH
#define SET_ITEMCOUNT_LINE_LENGTH 4
#endif //SET_ITEMCOUNT_LINE_LENGTH

#define set_printn( set )          \
    do                             \
    {                              \
        printf( "\"" #set "\" " ); \
        set_print( set );          \
    }                              \
    while ( 0 )

#define set_printn_as( set, print_func ) \
    do                                   \
    {                                    \
        printf( "\"" #set "\" " );       \
        set_print_as( set, print_func ); \
    }                                    \
    while ( 0 )
void set_print( ConstSet );
void set_print_as( ConstSet, PrintFunction );

#endif //CLIBS_SETS_H
