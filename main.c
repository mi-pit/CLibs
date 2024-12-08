//
// Created by MacBook on 04.12.2024.
//

#include "errors.h"
#include "string_utils.h"
#include "swexpr.h"

#include <regex.h>
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
    printf( "global: %i\n\n", global );

    string_t s = "Karel\nKokot_\nHopspop_\n_Pips-pop\nKunkr lagr\n";

    regex_t regex;
    int rec_rv = regcomp( &regex, "[ _]", REG_EXTENDED | REG_NEWLINE );
    if ( rec_rv != 0 )
    {
        char buffer[ 100 ] = { 0 };
        regerror( rec_rv, &regex, buffer, 100 );
        return fwarnx_ret( EXIT_FAILURE, "%s", buffer );
    }

    str_t *spl;
    ssize_t count = string_split_regex( &spl, s, &regex, false );
    if ( count < 0 )
    {
        print_stack_trace();
        return 2;
    }
    for ( ssize_t i = 0; i < count; ++i )
    {
        printf( "\"%s\"\n", string_escaped( spl[ i ] ) );
    }

    if ( string_split( &spl, s, "", false ) < 0 )
        print_stack_trace();

    regfree( &regex );
    printf( "hovno bobek\n" );

    exit( EXIT_SUCCESS );
}
