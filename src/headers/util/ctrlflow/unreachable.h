/**
 * @file unreachable.h
 * @brief
 * UNREACHABLE macro (abort(3) alias)
 */

// Created by Michal Pitner on 04.11.2025.

#ifndef UNREACHABLE_H
#define UNREACHABLE_H

#include <err.h>    /* warnx */
#include <stdlib.h> /* abort */

/**
 * This part of the code should not be reachable -- print warning (formatted) and abort
 */
#define UNREACHABLE_MSG( ... ) \
    do                         \
    {                          \
        warnx( __VA_ARGS__ );  \
        abort();               \
    }                          \
    while ( 0 )

#define UNREACHABLE()                                                                    \
    UNREACHABLE_MSG( "This part of the code should not be reachable (programmer made a " \
                     "mistake :/)" )


#endif //UNREACHABLE_H
