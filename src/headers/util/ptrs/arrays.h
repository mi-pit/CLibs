/**
 * @file arrays.h
 * @brief Utility macros for working with stack arrays
 */

// Created by Michal Pitner on 14.11.2025.


#ifndef CLIBS_ARRAYS_H
#define CLIBS_ARRAYS_H

/// For string literals
#define STRLEN( STRING ) ( sizeof( STRING ) - 1 )

/// Number of elements in an array (stack-allocated)
#define countof( array ) ( sizeof( array ) / sizeof *( array ) )

#endif //CLIBS_ARRAYS_H
