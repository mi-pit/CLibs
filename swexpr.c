//
// Created by MacBook on 04.12.2024.
//

#include "swexpr.h"

#include "errors.h" /* RV */

List Of( data * ) switch_expr_values_stack      = NULL; // def = NULL
List Of( var addr ) switch_expr_variables_stack = NULL; // def = NULL
List Of( size_t ) switch_expr_sizes_stack       = NULL; // def = 0
List Of( bool ) switch_expr_assigned_stack      = NULL; // def = false


static const bool TRUE  = true;
static const bool FALSE = false;


#define try_push( LIST, EXPRESSION, NBYTES )                   \
    do                                                         \
    {                                                          \
        if ( LIST == NULL )                                    \
            if ( ( LIST = list_init_size( NBYTES ) ) == NULL ) \
            {                                                  \
                print_stack_trace();                           \
                return RV_ERROR;                               \
            }                                                  \
        if ( list_append( LIST, EXPRESSION ) == RV_ERROR )     \
        {                                                      \
            print_stack_trace();                               \
            return RV_ERROR;                                   \
        }                                                      \
    }                                                          \
    while ( 0 )

int swex_push( size_t nbytes, const void *data )
{
    try_push( switch_expr_sizes_stack, &nbytes, sizeof( size_t ) );
    try_push( switch_expr_values_stack, data, sizeof( void * ) );
    try_push( switch_expr_assigned_stack, &FALSE, sizeof( bool ) );

    const void *null = NULL;
    try_push( switch_expr_variables_stack, &null, sizeof( void * ) );

    return RV_SUCCESS;
}

bool swex_is_assigned()
{
    return ( *( bool * ) list_peek( switch_expr_assigned_stack ) );
}

int swex_assign()
{
    return list_set_at( switch_expr_assigned_stack,
                        list_size( switch_expr_assigned_stack ) - 1,
                        &TRUE );
}


#define try_pop( LIST )                             \
    do                                              \
    {                                               \
        if ( list_pop( LIST, NULL ) != RV_SUCCESS ) \
        {                                           \
            print_stack_trace();                    \
            return RV_ERROR;                        \
        }                                           \
        if ( list_size( LIST ) == 0 )               \
        {                                           \
            list_destroy( LIST );                   \
            LIST = NULL;                            \
        }                                           \
    }                                               \
    while ( 0 )

int swex_pop( void )
{
    try_pop( switch_expr_values_stack );
    try_pop( switch_expr_assigned_stack );
    try_pop( switch_expr_sizes_stack );
    try_pop( switch_expr_variables_stack );

    return RV_SUCCESS;
}
