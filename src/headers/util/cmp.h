/**
 * @file cmp.h
 * @brief Comparator functions.
 *
 * May be used for sorting numeric types.
 */

#ifndef CLIBS_CMP_H
#define CLIBS_CMP_H

#include "../core/attributes.h"
#include "../core/types.h"


/// Evaluates the cmp function as true if the elements are equal
#define cmpeq( cmp_retval ) ( ( cmp_retval ) == 0 )


/**
 * Creates a new static comparator function with limited visibility
 * for already comparable numeric types.
 *
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


#endif //CLIBS_CMP_H
