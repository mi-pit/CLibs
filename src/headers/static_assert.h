/**
 * @file static_assert.h
 * @brief Static assert macro for C99 compatibility
 *
 * Makes it so that the macro acts as a static assert in C11+ and does nothing in C99 and earlier.
 */
//
// Created by Michal Pitner on 07.10.2025.
//

#ifndef STATIC_ASSERT_H
#define STATIC_ASSERT_H

#if __STDC_VERSION__ >= 201112L
#define STATIC_ASSERT( CONDITION, MESSAGE ) _Static_assert( CONDITION, MESSAGE )

#else
/// Does nothing
#define STATIC_ASSERT( CONDITION, MESSAGE ) struct UNUSED_STATIC_ASSERT_STRUCT

// The `struct` is there so that there is no
// trailing semicolon after use.

#endif


#endif //STATIC_ASSERT_H
