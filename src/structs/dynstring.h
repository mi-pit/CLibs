/**
 * @file dynstring.h
 * @brief A dynamic string.
 *
 * A string structure which handles re-allocation for the user.
 * Can append to the end, pre-pend to the start (linear complexity),
 * append formatted, clear, slice, and more.
 */

// Created by MacBook on 18.10.2024.


#ifndef CLIBS_DYNSTRING_H
#define CLIBS_DYNSTRING_H

#include "../headers/core/attributes.h" /* Constructor */

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>


typedef struct dynamic_string DynString;


/* ==== Constructors/Destructors ==== */
/**
 * Heap allocates a new `DynString` with a capacity of `DEFAULT_DYNSTRING_CAP`
 * @return a new `DynString` or `NULL` if allocation fails
 */
Constructor DynString *dynstr_init( void );
/**
 * Heap allocates a new `DynString` with a capacity of `cap`
 * @param cap   capacity of the `DynString`
 * @return      a new `DynString` or `NULL` if allocation fails
 */
Constructor DynString *dynstr_init_cap( size_t cap );
/**
 * Heap allocates a new `DynString` with the data set to the supplied string.
 * Capacity is set to the strings length including the terminating zero (`\0`)
 * @return a new `DynString` or `NULL` if allocation fails
 */
Constructor DynString *dynstr_init_as( const char * );

/** Frees all memory owned by the `DynString` */
void dynstr_destroy( DynString * );


/* ==== Modifiers ==== */
/**
 * Appends a string to the end of the `DynString`.
 *
 * `app` may not be `NULL`
 *
 * @return `RV_ERROR` on allocation error, else the number of appended chars
 */
ssize_t dynstr_append( DynString *, const char *app );
/**
 * Appends at most `len` characters to the end of the `DynString`
 *
 * `app` may not be `NULL`
 *
 * @param app       appended string
 * @param len       maximum number of chars appended
 * @return `RV_ERROR` on allocation error, else the number of appended chars (should be len)
 */
ssize_t dynstr_appendn( DynString *, const char *app, size_t len );
/**
 * Appends a formatted string to the end of a `DynString`
 *
 * @return `RV_ERROR` on allocation error, else the number of appended chars
 */
PrintfLike( 2, 3 ) ssize_t dynstr_appendf( DynString *, const char *fmt, ... );
/**
 * Appends a formatted string to the end of a `DynString`
 *
 * @return `RV_ERROR` on allocation error, else the number of appended chars
 */
ssize_t dynstr_vappendf( DynString *, const char *fmt, va_list vargs );

/**
 * Adds a string to the start of the `DynString`
 * @return `RV_ERROR` on allocation error, else the number of pre-pended chars
 */
ssize_t dynstr_prepend( DynString *, const char * );
/**
 * Prepends at most `len` characters to the start of the dynstr
 *
 * `s` may not be `NULL`
 *
 * @param s     prepended string
 * @param len   maximum number of chars prepended
 * @return `RV_ERROR` on allocation error, else the number of added chars (should be len)
 */
ssize_t dynstr_prependn( DynString *, const char *s, size_t len );
/**
 * Prepends a formatted string to the start of a `DynString`
 *
 * @return `RV_ERROR` on allocation error, else the number of appended chars
 */
PrintfLike( 2, 3 ) ssize_t dynstr_prependf( DynString *, const char *fmt, ... );
/**
 * Prepends a formatted string to the start of a `DynString`
 *
 * @return `RV_ERROR` on allocation error, else the number of appended chars
 */
ssize_t dynstr_vprependf( DynString *, const char *fmt, va_list vargs );

/**
 * Sets the string to be a slice of itself, from `start_idx` to `end_idx`.
 * If `end_idx` < 0, the end index is set as the offset
 * from the `\0` character in the string
 *
 * Examples:
 * @code
 * DynString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 0, -1 ); // => "Hello, world!"
 *
 * DynString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 2, -1 ); // => "llo, world!"
 *
 * DynString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 0, -3 ); // => "Hello, worl"
 *
 * DynString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 0, 4 );  // => "Hell"
 *
 * DynString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 2, -3 ); // => "llo, worl"
 *
 * DynString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 2, 4 );  // => "ll"
 *
 * DynString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 1, 2 );  // => "e"
 *
 * DynString dynstr = dynstr_init_as( "Hello, world!" );
 * dynstr_slice( dynstr, 2, 1 );  // => returns RV_EXCEPTION
 *                                //    (contents of dynstr are unchanged)
 * @endcode
 *
 * @param start_idx index of the first preserved character
 * @param end_idx   index of the last preserved character
 * @return `RV_SUCCESS` on success, `RV_EXCEPTION` if index is OOB
 */
int dynstr_slice( DynString *, size_t start_idx, ssize_t end_idx );
/** @see `dynstr_slice()` */
int dynstr_slice_e( DynString *, ssize_t end_idx );
/** @see `dynstr_slice()` */
int dynstr_slice_s( DynString *, size_t start_idx );

/**
 * Resets the dynstr to an empty string
 * @return `RV_ERROR` on realloc fail, else `RV_SUCCESS`
 */
int dynstr_reset( DynString * );

/**
 * Sets the dynstr's contents to a new string.
 *
 * @param string    this string is copied
 * @return `RV_ERROR` if allocation fails, else `RV_SUCCESS`
 */
int dynstr_set( DynString *, const char *string );

/**
 * Sets char at idx to `c` (with bounds checking).
 *
 * As if
 * @code
 * string[ idx ] = c
 * @endcode
 *
 * @return `RV_EXCEPTION` if idx is OOB
 */
int dynstr_set_at( DynString *dynstr, size_t idx, char c );


/* ==== Getters ==== */
/**
 * Allocates a new string with the `DynString` contents
 * @return pointer to heap-allocated data
 */
UseResult char *dynstr_data_copy( const DynString * );
/**
 * Returns a `const` view of the string.
 *
 * @return a pointer of the DynStrings data
 */
const char *dynstr_data( const DynString * );
/**
 * Fetches the length of the string in constant time
 *
 * @return length of the string
 */
size_t dynstr_len( const DynString * );


/**
 * Supplies `string_map` with the `DynString`'s data and
 * sets the new string as the `DynString`'s contents while preserving its invariants.
 * <p>
 * `string_map` MUST return a pointer that may be passed to `free(3)`.
 * </p>
 * <p>
 * If an string_map returns `NULL` --
 * no change is done in the `DynString`,
 * RV_ERROR is returned, and an error message is printed.
 * </p>
 *
 * @code
 * char *string_append_A( const char *str, size_t len )
 * {
 *     const size_t new_len = len + 1;
 *     char *const new_str = malloc( new_len + 1 );
 *     if ( new_str == NULL )
 *         return NULL;
 *
 *     strncpy( new_str, str, len );
 *     new_str[ new_len - 1 ] = 'A';
 *     new_str[ new_len ] = '\0';
 *
 *     return new_str;
 * }
 *
 * // Elsewhere
 * DynString *dynstr = ...;
 * const int rv = dynstr_map( dynstr, string_append_A ); // appends 'A'
 * @endcode
 *
 * @return `RV_ERROR` if `string_map` returns null, else `RV_SUCCESS`
 */
#define dynstr_map( DYNSTRING, MAPPER ) DynString__map( DYNSTRING, MAPPER, #MAPPER )

int DynString__map( DynString *dynstr,
                    char *( *string_map )( const char *string, size_t string_length ),
                    const char *mapper_name );


#endif //CLIBS_DYNSTRING_H
