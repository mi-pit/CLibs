/**
 * @file swexpr.h
 * @brief
 * A switch which works with more complex data structures.
 *
 * @requires `structs/dynarr.h`
 */

// Created by MacBook on 18.02.2024.

#ifndef CLIBS_SWEXPR_H
#define CLIBS_SWEXPR_H

#include "../../../structs/dynarr.h" /* stack-like lists */
#include "../../core/errors.h"       /* err */
#include "../ptrs.h"                 /* deref_as, free_n */

#include <stdbool.h> /* bool */


/* ================================ User usable ================================ */


#define SWEX_NAME_CREATOR( TOKEN ) CLIBS_SWEXPR__##TOKEN


/**
 * Initializes the swexpr to, in each `case`, compare it to this expression
 *
 * @attention must be followed by `swex_finish()` after the body
 */
#define swex_init_val( expr_type, expr )                                 \
    expr_type swex_init_imm_tmp_var_##expr_type##_##expr##__ = ( expr ); \
    expr_type *swex_init_imm_tmp_var_##expr_type##_##expr##_ptr_ =       \
            &( swex_init_imm_tmp_var_##expr_type##_##expr##__ );         \
    SWEX_NAME_CREATOR( FUNC_PUSH )(                                      \
            sizeof( expr_type ),                                         \
            &( swex_init_imm_tmp_var_##expr_type##_##expr##_ptr_ ) );

/**
 * Initializes the swexpr to, in each `case`,
 * compare it to the data behind the supplied pointer
 *
 * @attention must be followed by `swex_finish()` after the body
 */
#define swex_init_ptr( expr, nbytes ) SWEX_NAME_CREATOR( FUNC_PUSH )( nbytes, ( expr ) );

/** @attention must be followed by `swex_finish()` after the body */
#define swex_init_str( expr )                                       \
    const char *swex_init_imm_tmp_var_char##_##expr##__ = ( expr ); \
    SWEX_NAME_CREATOR( FUNC_PUSH )( strlen( expr ),                 \
                                    &( swex_init_imm_tmp_var_char##_##expr##__ ) );

/** Uses an existing variable to store the result from `resolve` */
#define as( VAR_NAME )                                           \
    {                                                            \
        void *swex_as_var_addr = &( VAR_NAME );                  \
        SWEX_NAME_CREATOR( FUNC_INIT_VAR )( &swex_as_var_addr ); \
    }

/** Creates a new local variable to store the result from `resolve` */
#define as_new( NEW_VAR_TYPE, NEW_VAR_NAME )        \
    NEW_VAR_TYPE NEW_VAR_NAME = ( NEW_VAR_TYPE ) 0; \
    as( NEW_VAR_NAME )


/** Assigns the `result` into the swexpr variable */
#define resolve( RESULT_TYPE, RESULT )                                                 \
    do                                                                                 \
    {                                                                                  \
        *deref_as( RESULT_TYPE *, SWEX_NAME_CREATOR( FUNC_GET_VAR_ADDR )() ) = RESULT; \
    }                                                                                  \
    while ( 0 )

/**
 * Next statement/block is executed if the currently "switched"
 * value matches this expression
 *
 * Each case (including default) is only executed if no other
 * case branch has been executed in this swexpr so far
 */
#define swex_case_imm( type, expr_case )                                \
    {                                                                   \
        if ( SWEX_NAME_CREATOR( AUX_VARIABLE ) != NULL )                \
        {                                                               \
            free_n( SWEX_NAME_CREATOR( AUX_VARIABLE ) );                \
            SWEX_NAME_CREATOR( AUX_VARIABLE ) = NULL;                   \
        }                                                               \
        SWEX_NAME_CREATOR( AUX_VARIABLE ) = malloc( sizeof( type ) );   \
        if ( SWEX_NAME_CREATOR( AUX_VARIABLE ) == NULL )                \
            err( ENOMEM, "swex_case_imm" ); /* todo: <- here */         \
        type swex_case_imm_aux_var_ = expr_case;                        \
        memcpy( SWEX_NAME_CREATOR( AUX_VARIABLE ),                      \
                &swex_case_imm_aux_var_,                                \
                sizeof( type ) );                                       \
    }                                                                   \
    if ( !SWEX_NAME_CREATOR( FUNC_IS_ASSIGNED )()                       \
         && ( SWEX_NAME_CREATOR( FUNC_PEEK_SIZE )() ) == sizeof( type ) \
         && memcmp( SWEX_NAME_CREATOR( FUNC_PEEK_VALUE )(),             \
                    SWEX_NAME_CREATOR( AUX_VARIABLE ),                  \
                    sizeof( type ) )                                    \
                    == 0 )                                              \
        if ( SWEX_NAME_CREATOR( FUNC_ASSIGN )() != 1 )

/** @see `swex_case_imm` */
#define swex_case_ptr( expr_case, nbytes )                                             \
    if ( !SWEX_NAME_CREATOR( FUNC_IS_ASSIGNED )()                                      \
         && ( SWEX_NAME_CREATOR( FUNC_PEEK_SIZE )() ) == sizeof( void * )              \
         && memcmp( SWEX_NAME_CREATOR( FUNC_PEEK_VALUE )(), expr_case, nbytes ) == 0 ) \
        if ( SWEX_NAME_CREATOR( FUNC_ASSIGN )() != 1 )

/** @see `swex_case_imm` */
#define swex_case_str( expr_case )                                             \
    if ( !SWEX_NAME_CREATOR( FUNC_IS_ASSIGNED )()                              \
         && strcmp( SWEX_NAME_CREATOR( FUNC_PEEK_VALUE )(), expr_case ) == 0 ) \
        if ( SWEX_NAME_CREATOR( FUNC_ASSIGN )() != 1 )

/**
 * Executes the following statement **if and only if**
 * no value has been assigned to the switch–target so far
 */
#define swex_default()                            \
    if ( !SWEX_NAME_CREATOR( FUNC_IS_ASSIGNED )() \
         && ( SWEX_NAME_CREATOR( FUNC_ASSIGN )() != 1 ) )

/**
 * Frees all memory owned by the swexpr and resets
 * all static variables needed for the functioning.
 *
 * @attention Invoke this right as you are leaving the swexpr body.
 * Not calling this function will cause undefined behaviour and calling it prematurely
 * will probably cause a NULL pointer dereference.
 */
#define swex_finish()                                \
    do                                               \
    {                                                \
        SWEX_NAME_CREATOR( FUNC_POP )();             \
        free_n( SWEX_NAME_CREATOR( AUX_VARIABLE ) ); \
    }                                                \
    while ( 0 )


/* ================================ aux ================================ */

/** @cond INTERNAL */
/* DO NOT USE */

// stack for values, pushed in init, queried in cases
LibraryDefined List /* void * */ *SWEX_NAME_CREATOR( STACK_VALUES ) = NULL;
// stack for pointers to variables (variables from `as`)
LibraryDefined List /* void * */ *SWEX_NAME_CREATOR( STACK_VARIABLES ) = NULL;
// stack for the size of the values
LibraryDefined List /* size_t */ *SWEX_NAME_CREATOR( STACK_SIZES ) = NULL;
// stack for bools -- whether a case has matched yet
LibraryDefined List /* bool */ *SWEX_NAME_CREATOR( STACK_ASSIGNED ) = NULL;


LibraryDefined void *SWEX_NAME_CREATOR( AUX_VARIABLE );


LibraryDefined int SWEX_NAME_CREATOR( FUNC_PUSH )( const size_t nbytes,
                                                   const void *const data )
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

    static const bool SWEX_FALSE   = false;
    static const void *const NULL_ = NULL;

    try_push( SWEX_NAME_CREATOR( STACK_SIZES ), &nbytes, sizeof( size_t ) );
    try_push( SWEX_NAME_CREATOR( STACK_VALUES ), data, sizeof( void * ) );
    try_push( SWEX_NAME_CREATOR( STACK_ASSIGNED ), &SWEX_FALSE, sizeof( bool ) );

    try_push( SWEX_NAME_CREATOR( STACK_VARIABLES ), &NULL_, sizeof( void * ) );

    return RV_SUCCESS;
#undef try_push
}

LibraryDefined bool SWEX_NAME_CREATOR( FUNC_IS_ASSIGNED )( void )
{
    return *( bool * ) list_peek( SWEX_NAME_CREATOR( STACK_ASSIGNED ) );
}

LibraryDefined int SWEX_NAME_CREATOR( FUNC_ASSIGN )( void )
{
    static const bool SWEX_TRUE = true;

    return list_set_at( SWEX_NAME_CREATOR( STACK_ASSIGNED ),
                        list_size( SWEX_NAME_CREATOR( STACK_ASSIGNED ) ) - 1,
                        &SWEX_TRUE );
}

LibraryDefined int SWEX_NAME_CREATOR( FUNC_POP )( void )
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

    try_pop( SWEX_NAME_CREATOR( STACK_VALUES ) );
    try_pop( SWEX_NAME_CREATOR( STACK_ASSIGNED ) );
    try_pop( SWEX_NAME_CREATOR( STACK_SIZES ) );
    try_pop( SWEX_NAME_CREATOR( STACK_VARIABLES ) );

    return RV_SUCCESS;
#undef try_pop
}

LibraryDefined int SWEX_NAME_CREATOR( FUNC_INIT_VAR )( void *const var_addr )
{
    return list_set_at( SWEX_NAME_CREATOR( STACK_VARIABLES ),
                        list_size( SWEX_NAME_CREATOR( STACK_VARIABLES ) ) - 1,
                        var_addr );
}

LibraryDefined void *SWEX_NAME_CREATOR( FUNC_GET_VAR_ADDR )( void )
{
    return list_at_last( SWEX_NAME_CREATOR( STACK_VARIABLES ) );
}

LibraryDefined size_t SWEX_NAME_CREATOR( FUNC_PEEK_SIZE )( void )
{
    return deref_as( size_t, list_peek( SWEX_NAME_CREATOR( STACK_SIZES ) ) );
}

LibraryDefined void *SWEX_NAME_CREATOR( FUNC_PEEK_VALUE )( void )
{
    return deref_as( void *, list_peek( SWEX_NAME_CREATOR( STACK_VALUES ) ) );
}
/** @endcond */

#endif //CLIBS_SWEXPR_H
