#ifndef CLIBS_MISC_H
#define CLIBS_MISC_H

#include "attributes.h"
#include "extra_types.h"


/// Stops those pesky `unused variable` compiler warnings/errors
#define UNUSED( UNUSEDARG ) ( ( void ) UNUSEDARG )


/// For string literals
#define STRLEN( STRING ) ( sizeof( STRING ) - 1 )

/// Number of elements in an array (stack-allocated)
#define countof( array ) ( sizeof( array ) / sizeof *( array ) )


/// Evaluates the cmp function as true if the elements are equal
#define cmpeq( cmp_retval ) ( ( cmp_retval ) == 0 )


/**
 * Creates a new comparator function with limited visibility for already comparable numeric types.
 * `TYPE` has to be a single token since it's used to define the name of the function.
 */
#define DEFINE_CMP_FUNCTION( TYPE )                                 \
    LibraryDefined int cmp_##TYPE( const void *p1, const void *p2 ) \
    {                                                               \
        TYPE val_1 = *( TYPE * ) p1;                                \
        TYPE val_2 = *( TYPE * ) p2;                                \
                                                                    \
        if ( val_1 > val_2 )                                        \
            return 1;                                               \
        if ( val_1 < val_2 )                                        \
            return -1;                                              \
        return 0;                                                   \
    }

DEFINE_CMP_FUNCTION( byte )
DEFINE_CMP_FUNCTION( char )
DEFINE_CMP_FUNCTION( int )
DEFINE_CMP_FUNCTION( size_t )
DEFINE_CMP_FUNCTION( int64_t )
DEFINE_CMP_FUNCTION( uint64_t )


/**
 * Creates an arbitrary hash of any data of the specified length
 *
 * @param nbytes number of bytes (chars) under the pointer
 * @return unsigned 64-bit int hash
 */
LibraryDefined uint64_t hash_func( const void *const data, const size_t nbytes )
{
    uint64_t hash = 5381 * nbytes;

    const byte *data_byte = ( byte * ) data;
    for ( size_t i = 0; i < nbytes; ++i )
        hash = ( hash << 5 ) + hash + data_byte[ i ];

    return hash;
}


#endif //CLIBS_MISC_H
