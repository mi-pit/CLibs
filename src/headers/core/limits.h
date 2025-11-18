/**
 * @file limits.h
 * @brief This file includes <limits.h> & <stdint.h> and defines `INT_BIT` & `LONG_BIT`.
 */

// Created by Michal Pitner on 18.11.2025.

#ifndef CLIBS_LIMITS_H
#define CLIBS_LIMITS_H

#include <limits.h> /* keyword-MAX */
#include <stdint.h> /* intXX-MAX */


// clang-format off
#ifndef LONG_BIT
    #if LONG_MAX == INT32_MAX
        #define LONG_BIT 32
    #elif LONG_MAX == INT64_MAX
        #define LONG_BIT 64
    #endif // LONG_MAX
#endif // ndef LONG_BIT


#ifndef INT_BIT
    #if INT_MAX == INT16_MAX
        #define INT_BIT 16
    #elif INT_MAX == INT32_MAX
        #define INT_BIT 32
    #elif INT_MAX == INT64_MAX
        #define INT_BIT 64
    #endif
#endif // ndef INT_BIT


#endif //CLIBS_LIMITS_H
