/**
 * @file hash.h
 * @brief Simple hash function.
 */

// Created by Michal Pitner on 14.11.2025.


#ifndef CLIBS_HASH_H
#define CLIBS_HASH_H

#include "../core/attributes.h" /* LibraryDefined */
#include "../core/types.h"      /* uint64_t, byte */

/**
 * Creates an arbitrary hash of any data of the specified length.
 *
 * Used, for example, in `set.h` and `dictionary.h`.
 *
 * @param data      pointer to data
 * @param nbytes    number of bytes (chars) under the pointer
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

#endif //CLIBS_HASH_H
