//
// Created by Michal Pitner on 23.06.2025.
//

#include "../src/headers/cmp.h"
#include "../src/headers/ctrlflow/assert_that.h"
#include "../src/headers/util/ptrs/alloc.h"
#include "../src/structs/dynarr.h"
#include "../src/structs/dynstring.h"
#include "../src/util/string_utils.h"

// dynstr.h
#include "../src/headers/util/print/array_printf.h"

#include <limits.h> /* INT_MAX, INT_MIN */


static void TEST_array_print( void )
{
    const int array[ 10 ] = { 1, 4, 9, 16, 0, -0, -1, INT_MAX, INT_MIN, 123 };
    assert_that( array[ countof( array ) - 1 ] == 123,
                 "the last element should be \"%d\", but it's \"%d\"", 123,
                 array[ countof( array ) - 1 ] );
    array_printf_sde( array, countof( array ), int, "%d",
                      FOREGROUND_BLUE "[" COLOR_DEFAULT, " and ",
                      FOREGROUND_CYAN "]" COLOR_DEFAULT "\n" );

    str_t str;
    array_sprintf_sde( str, array, countof( array ), int, "%d", "(", ",", ")" );
    printf( "%s\n", str );

    str_t repl = string_replaced( str, "(", FOREGROUND_BLUE "(" );
    repl       = string_replaced( repl, ")", ")" COLOR_DEFAULT );
    printf( "%s\n", repl );

    printf( "str: %zu, repl: %zu\n", strlen( str ), strlen( repl ) );
    printf( "%zu\n", strlen( COLOR_DEFAULT ) );

    free( str );

    printf( "END OF %s\n\n", __func__ );
}

static void TEST_print_list( void )
{
    List *ls = list_init_type( int );

    list_printf_sde( ls, int, "> %i", "Start\n", "\n\t", "End\n" );

    for ( int i = 0; i < 10; i++ )
        assert( list_append( ls, &i ) == RV_SUCCESS );

    list_printf_sde( ls, int, "\t> %i\n", "Start\n", "", "End\n" );

    list_destroy( ls );


    assert_that( ( ls = list_init_type( char * ) ) != NULL, );

    static char *array[] = { "Jedna", "Dva", "Tri", "Ctyri", "Pet" };

    for ( size_t i = 0; i < countof( array ); i++ )
    {
        char *string = array[ i ];
        assert_that( list_append( ls, &string ) == RV_SUCCESS, );
    }

    list_printf_sde( ls, char *, "\t> \"%s\"\n", "Start\n", "", "End\n" );

    list_destroy( ls );
}


#define free_many( ... )                                              \
    do                                                                \
    {                                                                 \
        void *_Free_many_ptrarr[]     = { __VA_ARGS__ };              \
        const size_t _Free_many_nptrs = countof( _Free_many_ptrarr ); \
        free_all( _Free_many_nptrs, __VA_ARGS__ );                    \
    }                                                                 \
    while ( 0 )


void foo( void )
{
    const char *static_string = "str";
    char on_stack_string[]    = "str";
    char *on_heap_string      = strdup( "str" );
    void *malloced            = malloc( 123 );
    void *calloced            = calloc( 1, 123 );

    xwarnx( "static=%p, on stack=%p, on heap=%p, malloced=%p, calloced=%p",
            ( void * ) static_string, ( void * ) on_stack_string,
            ( void * ) on_heap_string, malloced, calloced );

    free_many( malloced, on_heap_string, calloced );
}

int main( void )
{
    TEST_array_print();
    TEST_print_list();

    foo();
}
