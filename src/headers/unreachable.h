//
// Created by Michal Pitner on 04.11.2025.
//

#ifndef UNREACHABLE_H
#define UNREACHABLE_H

#include <stdlib.h> /* abort */

/// This code should not be reachable -- abort
#define UNREACHABLE() ( abort() )

#endif //UNREACHABLE_H
