//
// Created by MacBook on 04.12.2024.
//

#include "swexpr.h"

#include "errors.h" /* RV, print_stack_trace() */

List switch_expr_values_stack    = NULL;
List switch_expr_variables_stack = NULL;
List switch_expr_sizes_stack     = NULL;
List switch_expr_assigned_stack  = NULL;


static const bool TRUE  = true;
static const bool FALSE = false;


#define try_push( LIST, EXPRESSION, NBYTES )                   \
    do                                                         \
    {                                                          \
        if ( LIST == NULL )                                    \
            if ( ( LIST = list_init_size( NBYTES ) ) == NULL ) \
            {                                                  \
                f_stack_trace();                               \
                return RV_ERROR;                               \
            }                                                  \
        if ( list_append( LIST, EXPRESSION ) == RV_ERROR )     \
        {                                                      \
            f_stack_trace();                                   \
            return RV_ERROR;                                   \
        }                                                      \
    }                                                          \
    while ( 0 )

int switch_expression_push( size_t nbytes, const void *data )
{
    try_push( switch_expr_sizes_stack, &nbytes, sizeof( size_t ) );
    try_push( switch_expr_values_stack, data, sizeof( void * ) );
    try_push( switch_expr_assigned_stack, &FALSE, sizeof( bool ) );

    const void *null = NULL;
    try_push( switch_expr_variables_stack, &null, sizeof( void * ) );

    return RV_SUCCESS;
}

bool switch_expression_is_assigned()
{
    return ( *( bool * ) list_peek( switch_expr_assigned_stack ) );
}

int switch_expression_assign( void )
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
            f_stack_trace();                        \
            return RV_ERROR;                        \
        }                                           \
        if ( list_size( LIST ) == 0 )               \
        {                                           \
            list_destroy( LIST );                   \
            LIST = NULL;                            \
        }                                           \
    }                                               \
    while ( 0 )

int switch_expression_pop( void )
{
    try_pop( switch_expr_values_stack );
    try_pop( switch_expr_assigned_stack );
    try_pop( switch_expr_sizes_stack );
    try_pop( switch_expr_variables_stack );

    return RV_SUCCESS;
}
