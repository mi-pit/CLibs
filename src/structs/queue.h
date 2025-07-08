/**
 * @file queue.h
 * @brief First in, first out.
 *
 * | Method     | Time complexity |
 * |------------|-----------------|
 * | `init`     | O(1)            |
 * | `destroy`  | O(n)            |
 * | `clear`    | O(n)            |
 * | `enqueue`  | O(1)            |
 * | `dequeue`  | O(1)            |
 * | `get`      | O(n)            |
 * | `get_head` | O(1)            |
 * | `get_tail` | O(1)            |
 * | `get_size` | O(n)            |
 * | `is_empty` | O(1)            |
 */

//
// Created by Michal Pitner on 08.06.2025.
//

#ifndef CLIBS_QUEUE_H
#define CLIBS_QUEUE_H

#include "../headers/attributes.h"
#include "../headers/extra_types.h"


typedef struct fifo_queue Queue;


/**
 * Initializes a FIFO queue.
 *
 * @param el_size `sizeof` a single element
 * @return pointer to a valid Queue or `NULL`
 */
Constructor Queue *queue_init( size_t el_size );
/**
 * Frees all memory owned by the queue.
 */
void queue_destroy( Queue * );
/**
 * Truncates the queue to length = 0.
 */
void queue_clear( Queue * );

/**
 * Appends an element to the end of the Queue.
 *
 * @param data pointer to data
 * @return `RV_ERROR` if alloc fails, else `RV_SUCCESS`
 */
int queue_enqueue( Queue *, const void *data );
/**
 * Removes an element from the front of the Queue.
 *
 * The removed element is copied to `data_cont`.
 *
 * @param data_cont pointer to space in memory
 *                  able to hold at least `Queue::el_size` bytes or `NULL`
 * @return `RV_EXCEPTION` if queue is empty, else `RV_SUCCESS`
 */
int queue_dequeue( Queue *, void *data_cont );

/**
 * Fetches an item at `index`.
 *
 * @param index
 * @param data_cont pointer to space in memory
 *                  able to hold at least `Queue::el_size` bytes
 * @return `RV_EXCEPTION` if either
 * - `index` is OOB
 * - `data_cont` is `NULL`
 */
int queue_get( const Queue *, size_t index, void *data_cont );
/**
 * Fetches the next item to be dequeued.
 *
 * @param data_cont pointer to space in memory
 *                  able to hold at least `Queue::el_size` bytes
 * @return `RV_EXCEPTION` if no head
 */
int queue_get_head( const Queue *, void *data_cont );
/**
 * Fetches the last item that was enqueued.
 *
 * @param data_cont pointer to space in memory
 *                  able to hold at least `Queue::el_size` bytes
 * @return `RV_EXCEPTION` if there are no items
 */
int queue_get_tail( const Queue *, void *data_cont );

/// Length of queue (number of items)
size_t queue_get_size( const Queue * );
/** seems self-explanatory */
bool queue_is_empty( const Queue * );


/**
 * @cond INTERNAL
 * Iterator over queue.
 */
const struct queue_node *queue__iterator_get_head( const Queue * );
const struct queue_node *queue__iterator_get_next( const struct queue_node * );
/** @endcond */


// struct queue_node

/** @return queue node data pointer */
const void *queue_node_get_data( const struct queue_node * );

#endif //CLIBS_QUEUE_H
