//
// Created by Michal Pitner on 08.06.2025.
//

#ifndef CLIBS_QUEUE_H
#define CLIBS_QUEUE_H

struct queue_node {
    struct queue_node *next;

    void *data;
};

struct fifo_queue;

#endif //CLIBS_QUEUE_H
