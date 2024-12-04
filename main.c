//
// Created by MacBook on 04.12.2024.
//

#include "swexpr.h"

#include <printf.h>
#include <stdlib.h>

int global = 0;

#define TYPE ssize_t

TYPE test_swex( void )
{
    swex_init_str( "default" ) as_new( TYPE, Hopspop )
    {
        swex_case_str( "jedna" )
        {
            resolve( TYPE, ( TYPE ) 1 );
        }
        swex_case_str( "dva" )
        {
            resolve( TYPE, ( TYPE ) 2 );
        }

        swex_default()
        {
            resolve( TYPE, ( TYPE ) -10 );

            swex_init_val( TYPE, Hopspop ) as( global )
            {
                swex_case_imm( TYPE, ( TYPE ) -1 )
                {
                    resolve( int, 100 );
                }

                swex_default()
                {
                    resolve( int, 1 );
                }
            }
            swex_finish();

            resolve( TYPE, ( TYPE ) 69096 );
        }
    }
    swex_finish();

    return Hopspop;
}

int main( void )
{
    TYPE r = test_swex();
    printf( "ret: %lli\n", ( signed long long int ) r );
    printf( "global: %i\n", global );

    return 0;
}
