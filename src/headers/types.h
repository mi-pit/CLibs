/*
 * Includes most standard types and defines `byte` type.
 */

#ifndef CLIBS_TYPES_H
#define CLIBS_TYPES_H

/* imports */
/* types */
#include <stdbool.h>   /* bool, true/false */
#include <stddef.h>    /* size_t, ptrdiff_t */
#include <stdint.h>    /* int*_t */
#include <sys/types.h> /* off_t, mode_t */

/* other */
#include <inttypes.h> /* PRI macros */
#include <limits.h>   /* INT*_MAX */


typedef int8_t byte;

#endif //CLIBS_TYPES_H
