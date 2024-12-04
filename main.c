//
// Created by MacBook on 04.12.2024.
//

#include "string_utils.h"
#include "swexpr.h"

#include <stdio.h>  /* getline */
#include <stdlib.h> /* malloc */


int main( void )
{
    str_t line = NULL;
    size_t cap = 0;

    while ( ( getline( &line, &cap, stdin ) ) != -1 )
    {
        string_strip( line );

        swex_init_str( line )
        {
            swex_case_str( "hops" )
            {
                printf( "pop\n" );
            }
            swex_case_str( "L" )
            {
                return EXIT_FAILURE;
            }

            swex_default()
            {
                warnx( "kunda" );
            }
        }
    }
    return EXIT_SUCCESS;
}
