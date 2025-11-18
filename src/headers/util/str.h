/**
 * @file str.h
 * @brief Includes all files from `headers/util/str/`
 *
 * These headers define some semi-standard "functions" (macros).
 */


#include "../core/stdc_versions.h" /* (for this header) STDC11 */

/* include */
#include "str/asprintf.h"
#include "str/strdup.h"

#if __STDC_VERSION__ >= STANDARD_C11_VERSION
#include "str/to_string.h"
#endif // C11
