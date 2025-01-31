//
// Created by MacBook on 18.02.2024.
//
#ifndef CLIBS_SWEXPR_H
#define CLIBS_SWEXPR_H

#include "Dev/pointer_utils.h" /* deref_as() */
#include "Structs/dynarr.h" /* List (stack) */

#include <err.h>     /* err() */
#include <errno.h>   /* ENOMEM */
#include <stdbool.h> /* bool */
#include <string.h>  /* memcpy() */


LibraryDefined void *swex_aux_variable_;

int switch_expression_push( size_t nbytes, const void *data );
bool switch_expression_is_assigned( void );
int switch_expression_assign( void );
int switch_expression_pop( void );
int switch_expression_init_var( void *var_addr );
void *switch_expression_get_varaddr( void );

size_t switch_expression_size_peek( void );
void *switch_expression_value_peek( void );

/**
 * Initializes the swexpr to, in each ‹ case ›,
 * compare it to the data behind the supplied pointer
 */
#define swex_init_ptr( expr, nbytes ) switch_expression_push( nbytes, ( expr ) );

#define swex_init_str( expr )                                              \
    const char *swex_init_imm_tmp_var_##expr_type##_##expr##__ = ( expr ); \
    switch_expression_push( strlen( expr ),                                \
                            &( swex_init_imm_tmp_var_##expr_type##_##expr##__ ) );

/**
 * Initializes the swexpr to, in each ‹ case ›, compare it to this expression
 */
#define swex_init_val( expr_type, expr )                                     \
    expr_type swex_init_imm_tmp_var_##expr_type##_##expr##__ = ( expr );     \
    expr_type *swex_init_imm_tmp_var_##expr_type##_##expr##_ptr_ =           \
            &( swex_init_imm_tmp_var_##expr_type##_##expr##__ );             \
    assert( switch_expression_push(                                          \
                    sizeof( expr_type ),                                     \
                    &( swex_init_imm_tmp_var_##expr_type##_##expr##_ptr_ ) ) \
            == RV_SUCCESS );

/**
 * Uses an existing variable to store the result from ‹ resolve ›
 */
#define as( VAR_NAME )                                   \
    {                                                    \
        void *swex_as_var_addr = &( VAR_NAME );          \
        switch_expression_init_var( &swex_as_var_addr ); \
    }

/**
 * Creates a new local variable to store the result from ‹ resolve ›
 */
#define as_new( NEW_VAR_TYPE, NEW_VAR_NAME )        \
    NEW_VAR_TYPE NEW_VAR_NAME = ( NEW_VAR_TYPE ) 0; \
    as( NEW_VAR_NAME )


/**
 * Assigns the 'result' into the swexpr variable
 */
#define resolve( result_type, result )                                        \
    do                                                                        \
    {                                                                         \
        *deref_as( result_type *, switch_expression_get_varaddr() ) = result; \
    }                                                                         \
    while ( 0 )

/**
 * Next statement/block is executed if the currently "switched"
 * value matches this expression
 * <p>
 * Each case (including default) is only executed if no other
 * case branch has been executed in this swexpr so far
 */
#define swex_case_imm( type, expr_case )                                                 \
    {                                                                                    \
        if ( swex_aux_variable_ != NULL )                                                \
        {                                                                                \
            free( swex_aux_variable_ );                                                  \
            swex_aux_variable_ = NULL;                                                   \
        }                                                                                \
        swex_aux_variable_ = malloc( sizeof( type ) );                                   \
        if ( swex_aux_variable_ == NULL )                                                \
            err( ENOMEM, "swex_case_imm" ); /* todo: <- here */                          \
        type swex_case_imm_aux_var_ = expr_case;                                         \
        memcpy( swex_aux_variable_, &swex_case_imm_aux_var_, sizeof( type ) );           \
    }                                                                                    \
    if ( !switch_expression_is_assigned()                                                \
         && ( switch_expression_size_peek() ) == sizeof( type )                          \
         && memcmp( switch_expression_value_peek(), swex_aux_variable_, sizeof( type ) ) \
                    == 0 )                                                               \
        if ( switch_expression_assign() != 1 )

#define swex_case_ptr( expr_case, nbytes )                                     \
    if ( !switch_expression_is_assigned()                                      \
         && ( switch_expression_size_peek() ) == sizeof( void * )              \
         && memcmp( switch_expression_value_peek(), expr_case, nbytes ) == 0 ) \
        if ( switch_expression_assign() != 1 )

#define swex_case_str( expr_case )                                     \
    if ( !switch_expression_is_assigned()                              \
         && strcmp( switch_expression_value_peek(), expr_case ) == 0 ) \
        if ( switch_expression_assign() != 1 )

/**
 * Executes the following statement <b>if and only if</b>
 * no value has been assigned to the switch–target so far
 */
#define swex_default() \
    if ( !switch_expression_is_assigned() && ( switch_expression_assign() != 1 ) )

/**
 * Frees all memory owned by the swexpr and resets
 * all static variables needed for the functioning.
 *
 * @attention Invoke this right as you are leaving the swexpr body.
 * Not calling this function will cause undefined behaviour and calling it prematurely
 * will probably cause a NULL pointer dereference.
 */
#define swex_finish()               \
    do                              \
    {                               \
        switch_expression_pop();    \
        free( swex_aux_variable_ ); \
        swex_aux_variable_ = NULL;  \
    }                               \
    while ( 0 )

#endif //CLIBS_SWEXPR_H
