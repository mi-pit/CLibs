//
// Created by MacBook on 04.12.2024.
//

#include "swexpr.h"

#include "Dev/errors.h" /* RV, f_stack_trace() */


Private List *switch_expr_values_stack    = NULL;
Private List *switch_expr_variables_stack = NULL;
Private List *switch_expr_sizes_stack     = NULL;
Private List *switch_expr_assigned_stack  = NULL;


static const bool SWEX_TRUE  = true;
static const bool SWEX_FALSE = false;


#define try_push( LIST, EXPRESSION, NBYTES )                               \
    do                                                                     \
    {                                                                      \
        if ( LIST == NULL && ( LIST = list_init_size( NBYTES ) ) == NULL ) \
            return f_stack_trace( RV_ERROR );                              \
        if ( list_append( LIST, EXPRESSION ) == RV_ERROR )                 \
            return f_stack_trace( RV_ERROR );                              \
    }                                                                      \
    while ( 0 )

int switch_expression_push( size_t nbytes, const void *data )
{
    try_push( switch_expr_sizes_stack, &nbytes, sizeof( size_t ) );
    try_push( switch_expr_values_stack, data, sizeof( void * ) );
    try_push( switch_expr_assigned_stack, &SWEX_FALSE, sizeof( bool ) );

    const void *null = NULL;
    try_push( switch_expr_variables_stack, &null, sizeof( void * ) );

    return RV_SUCCESS;
}

bool switch_expression_is_assigned( void )
{
    return ( *( bool * ) list_peek( switch_expr_assigned_stack ) );
}

int switch_expression_assign( void )
{
    return list_set_at( switch_expr_assigned_stack,
                        list_size( switch_expr_assigned_stack ) - 1,
                        &SWEX_TRUE );
}


#define try_pop( LIST )                             \
    do                                              \
    {                                               \
        if ( list_pop( LIST, NULL ) != RV_SUCCESS ) \
            return f_stack_trace( RV_ERROR );       \
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

int switch_expression_init_var( void *var_addr )
{
    return list_set_at( switch_expr_variables_stack,
                        list_size( switch_expr_variables_stack ) - 1,
                        var_addr );
}

void *switch_expression_get_varaddr( void )
{
    return list_at_last( switch_expr_variables_stack );
}

size_t switch_expression_size_peek( void )
{
    return *( size_t * ) list_peek( switch_expr_sizes_stack );
}

void *switch_expression_value_peek( void )
{
    return *( void ** ) list_peek( switch_expr_values_stack );
}
