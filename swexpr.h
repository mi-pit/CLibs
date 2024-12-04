//
// Created by MacBook on 18.02.2024.
//
#ifndef CLIBS_SWEXPR_H
#define CLIBS_SWEXPR_H

#include "dynarr.h"
#include "pointer_utils.h"

#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>


/* FIXME
 *  nested swexes
 */

#define Of( TYPEARG )

static List Of( data * ) switch_expr_values_        = NULL; // def = NULL
static List Of( variable * ) switch_expr_variables_ = NULL; // def = NULL
static List Of( size_t ) switch_expr_sizes_         = NULL; // def = 0
static List Of( bool ) switch_expr_assigned_        = NULL; // def = false

void swex_push( size_t nbytes, void *expression );
void swex_assign( int, ... );
void swex_pop( void );

/**
 * Initializes the swexpr to, in each ‹ case ›,
 * compare it to the data behind the supplied pointer
 */
#define swex_init_ptr( expr_type, expr, nbytes )                             \
    {                                                                        \
        size_t swex_init_ptr_nbytes = nbytes;                                \
        free_n( switch_expr_values_ );                                       \
        switch_expr_value_ = malloc( swex_init_ptr_nbytes );                 \
        if ( switch_expr_value_ == NULL )                                    \
            err( ENOMEM, "%s @ %d: switch_expression", __func__, __LINE__ ); \
        memcpy( switch_expr_values_, ( expr ), ( swex_init_ptr_nbytes ) );   \
        switch_expr_size_     = ( swex_init_ptr_nbytes );                    \
        switch_expr_assigned_ = false;                                       \
    }                                                                        \
    expr_type tmp_var_swex_;

/**
 * Initializes the swexpr to, in each ‹case›,
 * compare it to the string supplied
 */
#define swex_init_str( expr )                                                \
    {                                                                        \
        size_t swex_init_ptr_nbytes = strlen( expr ) + 1;                    \
        free_n( switch_expr_values_ );                                       \
        switch_expr_value_ = malloc( swex_init_ptr_nbytes );                 \
        if ( switch_expr_value_ == NULL )                                    \
            err( ENOMEM, "%s @ %d: switch_expression", __func__, __LINE__ ); \
        memcpy( switch_expr_values_, ( expr ), ( swex_init_ptr_nbytes ) );   \
        switch_expr_size_     = ( swex_init_ptr_nbytes );                    \
        switch_expr_assigned_ = false;                                       \
    }                                                                        \
    string_t tmp_var_swex_;

/**
 * Initializes the swexpr to, in each ‹ case ›, compare it to this expression
 */
#define swex_init_val( expr_type, expr )                                            \
    {                                                                               \
        expr_type swexpr_init_var_tmp__ = ( expr );                                 \
        free_n( switch_expr_values_ );                                              \
        switch_expr_value_ = malloc( sizeof( expr_type ) );                         \
        if ( switch_expr_value_ == NULL )                                           \
            err( ENOMEM, "%s: switch_expression", __func__ );                       \
        memcpy( switch_expr_values_, &swexpr_init_var_tmp__, sizeof( expr_type ) ); \
        switch_expr_size_     = sizeof( expr_type );                                \
        switch_expr_assigned_ = false;                                              \
    }                                                                               \
    expr_type tmp_var_swex_;

/**
 * Creates a new local variable to store the result from ‹ resolve ›
 */
#define as_new( var_type, var ) \
    var_type var;               \
    switch_expr_variable_ = &( var );

/**
 * Uses an existing variable to store the result from ‹ resolve ›
 */
#define as( var_name ) switch_expr_variables_ = &var_name;

/**
 * Assigns the 'result' into the swexpr variable
 */
#define resolve( result_type, result )                            \
    do                                                            \
    {                                                             \
        deref_as( result_type, switch_expr_variables_ ) = result; \
        switch_expr_assigned_                           = true;   \
    }                                                             \
    while ( 0 )

/**
 * Next statement/block is executed if the currently "switched"
 * value matches this expression
 * <p>
 * Each case (including default) is only executed if no other
 * case branch has been executed in this swexpr so far
 */
#define swex_case_imm( type, expr_case )                                      \
    tmp_var_swex_ = ( expr_case );                                            \
    if ( !switch_expr_assigned_ && switch_expr_sizes_ == sizeof( type ) &&    \
         memcmp( switch_expr_values_, &tmp_var_swex_, sizeof( type ) ) == 0 ) \
        if ( ( switch_expr_assigned_ = true ) )

#define swex_case_ptr( expr_case, nbytes )                          \
    tmp_var_swex_ = ( expr_case );                                  \
    if ( !switch_expr_assigned_ && switch_expr_size_ == nbytes &&   \
         memcmp( switch_expr_value_, tmp_var_swex_, nbytes ) == 0 ) \
        if ( ( switch_expr_assigned_ = true ) )

#define swex_case_str( expr_case )                                                     \
    tmp_var_swex_ = ( expr_case );                                                     \
    if ( !switch_expr_assigned_ && strcmp( switch_expr_values_, tmp_var_swex_ ) == 0 ) \
        if ( ( switch_expr_assigned_ = true ) )

/**
 * Executes the following statement <b>if and only if</b>
 * no value has been assigned to the switch–target so far
 */
#define swex_default() if ( !switch_expr_assigned_ && ( switch_expr_assigned_ = true ) )

/**
 * Frees all memory owned by the swexpr and resets
 * all static variables needed for the functioning.
 *
 * @attention It is recommended to invoke this right after the swexpr body.
 * Not calling this function will cause memory leaks and calling it prematurely will
 * probably cause a NULL pointer dereference.
 */
#define swex_finish()                  \
    do                                 \
    {                                  \
        free_n( switch_expr_values_ ); \
        switch_expr_variable_ = NULL;  \
        switch_expr_size_     = 0;     \
        switch_expr_assigned_ = false; \
    }                                  \
    while ( 0 )

#endif //CLIBS_SWEXPR_H
