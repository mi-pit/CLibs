//
// Created by MacBook on 18.10.2024.
//

#ifndef CLIBS_DYNSTRING_H
#define CLIBS_DYNSTRING_H

#include "../headers/attributes.h" /* Constructor */

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>


#define DEFAULT_DYNSTRING_CAP 256


typedef struct dynamic_string DynString;


/* ==== Constructors/Destructors ==== */
/**
 * Heap allocates a new DynamicString with a capacity of DEFAULT_DYNSTRING_CAP
 * @return a new DynamicString or NULL if allocation fails
 */
Constructor struct dynamic_string *dynstr_init( void );
/**
 * Heap allocates a new DynamicString with a capacity of ‹cap›
 * @param cap   capacity of the DynamicString
 * @return      a new DynamicString or NULL if allocation fails
 */
Constructor struct dynamic_string *dynstr_init_cap( size_t cap );
/**
 * Heap allocates a new DynamicString with the data set to the supplied string.
 * Capacity is set to the strings length including the terminating zero ('\0')
 * @return a new DynamicString or NULL if allocation fails
 */
Constructor struct dynamic_string *dynstr_init_as( const char * );

/** Frees all memory owned by the dynamic_string */
void dynstr_destroy( struct dynamic_string * );


/* ==== Modifiers ==== */
/**
 * Appends a string to the end of the DynamicString.
 * <p>
 * `app` may not be NULL
 *
 * @return RV_ERROR on allocation error, else the number of appended chars
 */
ssize_t dynstr_append( struct dynamic_string *, const char *app );
/**
 * Appends at most `len` characters to the end of the dynstr
 * <p>
 * `app` may not be NULL
 * </p>
 *
 * @param app       appended string
 * @param len       maximum number of chars appended
 * @return RV_ERROR on allocation error, else the number of appended chars (should be len)
 */
ssize_t dynstr_appendn( struct dynamic_string *, const char *app, size_t len );
/**
 * Appends a formatted string to the end of a DynamicString
 *
 * @return RV_ERROR on allocation error, else the number of appended chars
 */
PrintfLike( 2, 3 ) ssize_t dynstr_appendf( DynString *, const char *fmt, ... );
/**
 * Appends a formatted string to the end of a DynamicString
 *
 * @return RV_ERROR on alloc fail, else RV_SUCCESS
 */
ssize_t dynstr_vappendf( struct dynamic_string *, const char *fmt, va_list vargs );

/**
 * Adds a string to the start of the DynamicString
 * @return RV_ERROR on allocation error, else the number of pre-pended chars
 */
ssize_t dynstr_prepend( struct dynamic_string *, const char * );
ssize_t dynstr_prependn( struct dynamic_string *dynstr, const char *s, size_t len );
PrintfLike( 2, 3 ) ssize_t dynstr_prependf( DynString *dynstr, const char *fmt, ... );
ssize_t dynstr_vprependf( struct dynamic_string *dynstr, const char *fmt, va_list vargs );

/**
 * Sets the string to be a slice of itself, from ‹start_idx› to ‹end_idx›.
 * If ‹end_idx› < 0, the end index is set as the offset
 * from the '\0' character in the string
 * @example
 * @code
 * DynamicString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 0, -1 ); // => "Hello, world!"
 *
 * DynamicString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 2, -1 ); // => "llo, world!"
 *
 * DynamicString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 0, -3 ); // => "Hello, worl"
 *
 * DynamicString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 0, 4 );  // => "Hell"
 *
 * DynamicString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 2, -3 ); // => "llo, worl"
 *
 * DynamicString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 2, 4 );  // => "ll"
 *
 * DynamicString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 1, 2 );  // => "e"
 *
 * DynamicString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 2, 1 );  // => returns RV_EXCEPTION
 *                                //    (contents of dynstr are unchanged)
 * @endcode
 *
 * @param start_idx index of the first preserved character
 * @param end_idx   index of the last preserved character
 * @return 0 on success, RV_EXCEPTION if index is OOB
 */
int dynstr_slice( struct dynamic_string *, size_t start_idx, ssize_t end_idx );
int dynstr_slice_e( struct dynamic_string *, ssize_t end_idx );
int dynstr_slice_s( struct dynamic_string *, size_t start_idx );

/**
 * Resets the dynstr to an empty string
 * @return RV_ERROR on realloc fail, else RV_SUCCESS
 */
int dynstr_reset( struct dynamic_string * );

/**
 * Sets the dynstr's contents to a new string.
 *
 * @param string    this string is copied
 * @return RV_ERROR if allocation fails, else RV_SUCCESS
 */
int dynstr_set( struct dynamic_string *, const char *string );

/**
 * Sets char @ idx to `c`
 *
 * @return RV_EXCEPTION if idx is OOB
 */
int dynstr_set_at( struct dynamic_string *dynstr, size_t idx, char c );


/* ==== Getters ==== */
/**
 * Allocates a new string with the DynamicString contents
 * @return pointer to heap-allocated data
 */
UseResult char *dynstr_data_copy( const struct dynamic_string * );
/**
 * Returns a `const` view of the string.
 *
 * @return a pointer of the DynamicStrings data
 */
const char *dynstr_data( const struct dynamic_string * );
/**
 * Fetches the length of the string in constant time
 *
 * @return length of the string
 */
size_t dynstr_len( const struct dynamic_string * );

#endif //CLIBS_DYNSTRING_H
