//
// Created by Michal Pitner on 04.11.2025.
//

#ifndef UNREACHABLE_H
#define UNREACHABLE_H

#include <stdlib.h> /* abort */

/// This code should not be reachable -- abort
#define UNREACHABLE_MSG( ... )   \
    do                           \
    {                            \
        fflwarnx( __VA_ARGS__ ); \
        abort();                 \
    }                            \
    while ( 0 )

#define UNREACHABLE()                                                                    \
    UNREACHABLE_MSG( "This part of the code should not be reachable (programmer made a " \
                     "mistake :/)" )


#endif //UNREACHABLE_H
