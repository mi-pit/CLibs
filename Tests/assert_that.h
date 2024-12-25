//
// Created by MacBook on 25.12.2024.
//

#ifndef CLIBS_ASSERT_THAT_H
#define CLIBS_ASSERT_THAT_H

#include "../errors.h" /* warn */

#include <stdlib.h> /* exit */

#define assert_that( EXPRESSION, ... )                   \
    do                                                   \
    {                                                    \
        if ( !( EXPRESSION ) )                           \
            exit( fwarnx_ret( RV_ERROR, __VA_ARGS__ ) ); \
    }                                                    \
    while ( 0 )

#endif //CLIBS_ASSERT_THAT_H
