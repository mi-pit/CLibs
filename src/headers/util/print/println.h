/**
 * @file println.h
 * @brief Formatted print (`printf(3)`) which terminates the message with an extra end line character.
 *
 * Also defines (if not already defined) the string/char which is used to terminate lines.
 */

// Created by Michal Pitner on 14.10.2025.


#ifndef PRINT_H
#define PRINT_H


#ifndef LINEBREAK_STR
#define LINEBREAK_STR "\n"
#endif

/// ig this wouldn't work on windows :/
#ifndef LINEBREAK_CHR
#define LINEBREAK_CHR '\n'
#endif


#include <stdio.h> /* include for printf */


/**
 * @param format_string
 * @param ...
 *
 * @see `printf(3)`
 */
#define println( ... ) ( printf( __VA_ARGS__ ) + printf( "%s", LINEBREAK_STR ) )


#endif //PRINT_H
