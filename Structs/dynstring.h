//
// Created by MacBook on 18.10.2024.
//

#ifndef CLIBS_DYNSTRING_H
#define CLIBS_DYNSTRING_H

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

#ifndef __result_use_check
#define __result_use_check
#endif

#ifndef __printflike
#define __printflike( a, b )
#endif


typedef struct dynamic_string *DynamicString;
typedef const struct dynamic_string *ConstDynamicString;


#define DEFAULT_DYNSTRING_CAP 256


/* ==== Constructors/Destructors ==== */
/**
 * Heap allocates a new DynamicString with a capacity of DEFAULT_DYNSTRING_CAP
 * @return a new DynamicString or NULL if allocation fails
 */
DynamicString dynstr_init( void ) __result_use_check;
/**
 * Heap allocates a new DynamicString with a capacity of ‹cap›
 * @param cap   capacity of the DynamicString
 * @return      a new DynamicString or NULL if allocation fails
 */
DynamicString dynstr_init_cap( size_t cap ) __result_use_check;
/**
 * Heap allocates a new DynamicString with the data set to the supplied string.
 * Capacity is set to the strings length including the terminating zero ('\0')
 * @return a new DynamicString or NULL if allocation fails
 */
DynamicString dynstr_init_as( const char * ) __result_use_check;

/**
 * Frees all memory associated with the DynamicString
 */
void dynstr_destroy( DynamicString );


/* ==== Modifiers ==== */
/**
 * Appends a string to the end of the DynamicString.
 * <p>
 * `app` may not be NULL
 *
 * @return 0 on success, -1 on allocation error
 */
int dynstr_append( DynamicString, const char *app );
/**
 * Appends at most `len` characters to the end of the dynstr
 * <p>
 * `app` may not be NULL
 *
 * @param dynstr
 * @param app       appended string
 * @param len       maximum number of chars appended
 * @return RV_SUCCESS on success, RV_ERROR on allocation error
 */
int dynstr_appendn( DynamicString, const char *app, size_t len );
/**
 * Appends a formatted string to the end of a DynamicString
 *
 * @return RV_ERROR on alloc fail, else RV_SUCCESS
 */
int dynstr_appendf( DynamicString, const char *fmt, ... ) __printflike( 2, 3 );
/**
 * Appends a formatted string to the end of a DynamicString
 *
 * @return RV_ERROR on alloc fail, else RV_SUCCESS
 */
int dynstr_vappendf( DynamicString, const char *fmt, va_list vargs );

/**
 * Adds a string to the start of the DynamicString
 * @return 0 on success, -1 on allocation error
 */
int dynstr_prepend( DynamicString, const char * );


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
int dynstr_slice( DynamicString, size_t start_idx, ssize_t end_idx );
int dynstr_slice_e( DynamicString, ssize_t end_idx );
int dynstr_slice_s( DynamicString, size_t start_idx );

/**
 * Resets the dynstr to an empty string
 * @return RV_ERROR on realloc fail, else RV_SUCCESS
 */
int dynstr_reset( DynamicString dynstr );


/* ==== Getters ==== */
/**
 * Allocates a new string with the DynamicString contents
 * @return pointer to heap-allocated data
 */
char *dynstr_data_copy( ConstDynamicString ) __result_use_check;
/**
 * @return a pointer of the DynamicStrings data
 */
char *dynstr_data( DynamicString );
/**
 * @return length of the string
 */
size_t dynstr_len( ConstDynamicString );

#endif //CLIBS_DYNSTRING_H
