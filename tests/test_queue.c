//
// Created by Michal Pitner on 09.06.2025.
//

#include "../src/headers/assert_that.h"
#include "../src/headers/pointer_utils.h"
#include "../src/headers/unit_tests.h"
#include "../src/structs/queue.h"


struct queue_node {
    struct queue_node *next;

    void *data;
};

struct fifo_queue {
    struct queue_node *head; // first in line; first out
    struct queue_node *tail; // last

    size_t el_size; // each element must be the same size
};


TEST( init )
{
    struct fifo_queue *queue = queue_init( 1 );
    UNIT_TEST( queue != NULL );
    queue_destroy( queue );
}
END_TEST

TEST( enqueue )
{
    struct fifo_queue *queue = queue_init( sizeof( int ) );
    assert_that( queue != NULL, "init failed" );

    int data = -1;
    UNIT_TEST( queue_enqueue( queue, &data ) == RV_SUCCESS );
    const struct queue_node *node = queue->head;
    UNIT_TEST( node != NULL );
    UNIT_TEST( queue->head == queue->tail );

    SET_UNIT_TEST_VERBOSITY( false );
    for ( data = 0; data <= 123; data++ )
    {
        int rv;
        UNIT_TEST( ( rv = queue_enqueue( queue, &data ) ) == RV_SUCCESS );
        if ( rv == RV_SUCCESS )
        {
            node = node->next;
            UNIT_TEST( node->data != NULL );
            UNIT_TEST( deref_as( int, node->data ) == data );
        }
        else
        {
            const int to_go = 123 - data;
            CLIBS_UNIT_TESTS_failed_total += to_go;
            UNIT_TEST( to_go <= 0 );
            break;
        }
    }
    SET_UNIT_TEST_VERBOSITY( true );

    queue_destroy( queue );
}
END_TEST

TEST( dequeue )
{
    struct fifo_queue *queue = queue_init( sizeof( int ) );
    assert_that( queue != NULL, "init failed" );

    int data = -1;
    queue_enqueue( queue, &data );
    for ( data = 0; data <= 123; data++ )
        assert_that( queue_enqueue( queue, &data ) == RV_SUCCESS, "enqueue" );

    SET_UNIT_TEST_VERBOSITY( false );
    for ( data = -1; data <= 123; data++ )
    {
        int num;
        UNIT_TEST( queue_dequeue( queue, &num ) == RV_SUCCESS );
        UNIT_TEST( num == data );
    }
    SET_UNIT_TEST_VERBOSITY( true );

    int cont;
    UNIT_TEST( queue_dequeue( queue, &cont ) == RV_EXCEPTION );
    UNIT_TEST( queue->head == NULL );
    UNIT_TEST( queue->tail == NULL );

    queue_destroy( queue );
}
END_TEST


TEST( get )
{
    struct fifo_queue *queue = queue_init( sizeof( int ) );
    assert_that( queue != NULL, "init failed" );

    int data = -1;
    queue_enqueue( queue, &data );
    for ( data = 0; data <= 123; data++ )
        assert_that( queue_enqueue( queue, &data ) == RV_SUCCESS, "enqueue" );

    int got;
    got = -2;
    UNIT_TEST( queue_get( queue, 100, &got ) == RV_SUCCESS );
    UNIT_TEST( got == 99 );

    int tail;
    UNIT_TEST( queue_get( queue, queue_get_size( queue ) - 1, &got ) == RV_SUCCESS );
    UNIT_TEST( queue_get_tail( queue, &tail ) == RV_SUCCESS );
    UNIT_TEST( tail == got );

    got = -2;
    UNIT_TEST( queue_get( queue, 100, &got ) == RV_SUCCESS );
    UNIT_TEST( got == 99 );

    UNIT_TEST( queue_get( queue, 0, &got ) == RV_SUCCESS );
    UNIT_TEST( got == -1 );
    UNIT_TEST( queue_get_head( queue, &got ) == RV_SUCCESS );
    UNIT_TEST( got == -1 );

    UNIT_TEST( queue_get( queue, 1234, &got ) == RV_EXCEPTION );
    UNIT_TEST( queue_get( queue, 124, &got ) != RV_EXCEPTION );
    UNIT_TEST( queue_get( queue, 125, &got ) == RV_EXCEPTION );
    UNIT_TEST( queue_get( queue, queue_get_size( queue ), &got ) == RV_EXCEPTION );
    UNIT_TEST( queue_get( queue, 123, &got ) != RV_EXCEPTION );
    UNIT_TEST( queue_get( queue, -1, &got ) == RV_EXCEPTION );

    UNIT_TEST( queue_get( queue, 0, NULL ) == RV_EXCEPTION );

    queue_destroy( queue );
}
END_TEST


TEST( get_size )
{
    struct fifo_queue *queue = queue_init( sizeof( int ) );
    assert_that( queue != NULL, "init failed" );

    UNIT_TEST( queue_get_size( queue ) == 0 );
    UNIT_TEST( queue_is_empty( queue ) );

    int data = -1;
    assert_that( queue_enqueue( queue, &data ) == RV_SUCCESS, "enqueue" );
    UNIT_TEST( queue_get_size( queue ) == 1 );
    UNIT_TEST( !queue_is_empty( queue ) );
    SET_UNIT_TEST_VERBOSITY( false );
    for ( data = 0; data <= 123; data++ )
    {
        assert_that( queue_enqueue( queue, &data ) == RV_SUCCESS, "enqueue" );
        UNIT_TEST( queue_get_size( queue ) == data + 2ul );
        UNIT_TEST( !queue_is_empty( queue ) );
    }
    SET_UNIT_TEST_VERBOSITY( true );

    queue_destroy( queue );
}
END_TEST


int main( void )
{
    RUN_TEST( init );
    RUN_TEST( enqueue );
    RUN_TEST( dequeue );
    RUN_TEST( get );
    RUN_TEST( get_size );

    FINISH_TESTING();
}
