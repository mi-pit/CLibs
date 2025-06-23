/*
 * A switch which works with more complex data structures.
 *
 * For more info, see `docs/swex.md`
 *
 *
 * Created by MacBook on 18.02.2024.
 */

#ifndef CLIBS_SWEXPR_H
#define CLIBS_SWEXPR_H

#include "../structs/dynarr.h" /* stack-like lists */
#include "errors.h"            /* err */
#include "pointer_utils.h"     /* deref_as, free_n */

#include <stdbool.h> /* bool */


/* ================================ User usable ================================ */


/** Initializes the swexpr to, in each ‹case›, compare it to this expression */
#define swex_init_val( expr_type, expr )                                 \
    expr_type swex_init_imm_tmp_var_##expr_type##_##expr##__ = ( expr ); \
    expr_type *swex_init_imm_tmp_var_##expr_type##_##expr##_ptr_ =       \
            &( swex_init_imm_tmp_var_##expr_type##_##expr##__ );         \
    _Switch_expression_push_( sizeof( expr_type ),                       \
                              &( swex_init_imm_tmp_var_##expr_type##_##expr##_ptr_ ) );

/**
 * Initializes the swexpr to, in each ‹case›,
 * compare it to the data behind the supplied pointer
 */
#define swex_init_ptr( expr, nbytes ) _Switch_expression_push_( nbytes, ( expr ) );

#define swex_init_str( expr )                                              \
    const char *swex_init_imm_tmp_var_##expr_type##_##expr##__ = ( expr ); \
    _Switch_expression_push_( strlen( expr ),                              \
                              &( swex_init_imm_tmp_var_##expr_type##_##expr##__ ) );

/** Uses an existing variable to store the result from ‹resolve› */
#define as( VAR_NAME )                                     \
    {                                                      \
        void *swex_as_var_addr = &( VAR_NAME );            \
        _Switch_expression_init_var_( &swex_as_var_addr ); \
    }

/** Creates a new local variable to store the result from ‹resolve› */
#define as_new( NEW_VAR_TYPE, NEW_VAR_NAME )        \
    NEW_VAR_TYPE NEW_VAR_NAME = ( NEW_VAR_TYPE ) 0; \
    as( NEW_VAR_NAME )


/// Assigns the 'result' into the swexpr variable
#define resolve( result_type, result )                                          \
    do                                                                          \
    {                                                                           \
        *deref_as( result_type *, _Switch_expression_get_varaddr_() ) = result; \
    }                                                                           \
    while ( 0 )

/**
 * Next statement/block is executed if the currently "switched"
 * value matches this expression
 * <p>
 * Each case (including default) is only executed if no other
 * case branch has been executed in this swexpr so far
 */
#define swex_case_imm( type, expr_case )                                        \
    {                                                                           \
        if ( _Swex_aux_variable_ != NULL )                                      \
        {                                                                       \
            free_n( _Swex_aux_variable_ );                                      \
            _Swex_aux_variable_ = NULL;                                         \
        }                                                                       \
        _Swex_aux_variable_ = malloc( sizeof( type ) );                         \
        if ( _Swex_aux_variable_ == NULL )                                      \
            err( ENOMEM, "swex_case_imm" ); /* todo: <- here */                 \
        type swex_case_imm_aux_var_ = expr_case;                                \
        memcpy( _Swex_aux_variable_, &swex_case_imm_aux_var_, sizeof( type ) ); \
    }                                                                           \
    if ( !_Switch_expression_is_assigned_()                                     \
         && ( _Switch_expression_size_peek_() ) == sizeof( type )               \
         && memcmp( _Switch_expression_value_peek_(),                           \
                    _Swex_aux_variable_,                                        \
                    sizeof( type ) )                                            \
                    == 0 )                                                      \
        if ( _Switch_expression_assign_() != 1 )

#define swex_case_ptr( expr_case, nbytes )                                       \
    if ( !_Switch_expression_is_assigned_()                                      \
         && ( _Switch_expression_size_peek_() ) == sizeof( void * )              \
         && memcmp( _Switch_expression_value_peek_(), expr_case, nbytes ) == 0 ) \
        if ( _Switch_expression_assign_() != 1 )

#define swex_case_str( expr_case )                                       \
    if ( !_Switch_expression_is_assigned_()                              \
         && strcmp( _Switch_expression_value_peek_(), expr_case ) == 0 ) \
        if ( _Switch_expression_assign_() != 1 )

/**
 * Executes the following statement <b>if and only if</b>
 * no value has been assigned to the switch–target so far
 */
#define swex_default() \
    if ( !_Switch_expression_is_assigned_() && ( _Switch_expression_assign_() != 1 ) )

/**
 * Frees all memory owned by the swexpr and resets
 * all static variables needed for the functioning.
 *
 * @attention Invoke this right as you are leaving the swexpr body.
 * Not calling this function will cause undefined behaviour and calling it prematurely
 * will probably cause a NULL pointer dereference.
 */
#define swex_finish()                  \
    do                                 \
    {                                  \
        _Switch_expression_pop_();     \
        free_n( _Swex_aux_variable_ ); \
    }                                  \
    while ( 0 )


/* ================================ aux ================================ */

/* DO NOT USE */


LibraryDefined List /* <void *> */ *_Switch_expr_values_stack_    = NULL;
LibraryDefined List /* <void *> */ *_Switch_expr_variables_stack_ = NULL;
LibraryDefined List /* <size_t> */ *_Switch_expr_sizes_stack_     = NULL;
LibraryDefined List /* <bool> */ *_Switch_expr_assigned_stack_    = NULL;


LibraryDefined void *_Swex_aux_variable_;


LibraryDefined int _Switch_expression_push_( const size_t nbytes, const void *const data )
{
#define try_push( LIST, EXPRESSION, NBYTES )                               \
    do                                                                     \
    {                                                                      \
        if ( LIST == NULL && ( LIST = list_init_size( NBYTES ) ) == NULL ) \
            return f_stack_trace( RV_ERROR );                              \
        if ( list_append( LIST, EXPRESSION ) == RV_ERROR )                 \
            return f_stack_trace( RV_ERROR );                              \
    }                                                                      \
    while ( 0 )

    static const bool SWEX_FALSE = false;
    static void *const null      = NULL;

    try_push( _Switch_expr_sizes_stack_, &nbytes, sizeof( size_t ) );
    try_push( _Switch_expr_values_stack_, data, sizeof( void * ) );
    try_push( _Switch_expr_assigned_stack_, &SWEX_FALSE, sizeof( bool ) );

    try_push( _Switch_expr_variables_stack_, &null, sizeof( void * ) );

    return RV_SUCCESS;
#undef try_push
}

LibraryDefined bool _Switch_expression_is_assigned_( void )
{
    return *( bool * ) list_peek( _Switch_expr_assigned_stack_ );
}

LibraryDefined int _Switch_expression_assign_( void )
{
    static const bool SWEX_TRUE = true;

    return list_set_at( _Switch_expr_assigned_stack_,
                        list_size( _Switch_expr_assigned_stack_ ) - 1,
                        &SWEX_TRUE );
}

LibraryDefined int _Switch_expression_pop_( void )
{
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

    try_pop( _Switch_expr_values_stack_ );
    try_pop( _Switch_expr_assigned_stack_ );
    try_pop( _Switch_expr_sizes_stack_ );
    try_pop( _Switch_expr_variables_stack_ );

    return RV_SUCCESS;
#undef try_pop
}

LibraryDefined int _Switch_expression_init_var_( void *const var_addr )
{
    return list_set_at( _Switch_expr_variables_stack_,
                        list_size( _Switch_expr_variables_stack_ ) - 1,
                        var_addr );
}

LibraryDefined void *_Switch_expression_get_varaddr_( void )
{
    return list_at_last( _Switch_expr_variables_stack_ );
}

LibraryDefined size_t _Switch_expression_size_peek_( void )
{
    return deref_as( size_t, list_peek( _Switch_expr_sizes_stack_ ) );
}

LibraryDefined void *_Switch_expression_value_peek_( void )
{
    return deref_as( void *, list_peek( _Switch_expr_values_stack_ ) );
}

#endif //CLIBS_SWEXPR_H
