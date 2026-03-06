#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>
#include "protocol.h"

/*
  queue_t
  -------
  This is an in-process (NOT shared memory) bounded queue.
  Used only inside Aggregator process.

  Reader thread: pushes frames into queue
  Worker threads: pop frames from queue

  It is protected by:
    - pthread_mutex_t lock
    - pthread_cond_t  not_empty (workers wait on this)
    - pthread_cond_t  not_full  (reader waits on this)
*/
typedef struct {
    frame_t *buf;          // array of frames
    size_t   cap;          // capacity (number of slots)
    size_t   head;         // write index
    size_t   tail;         // read index
    size_t   count;        // how many frames currently in queue

    pthread_mutex_t lock;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;

    bool stop;             // to allow clean shutdown later (optional)
} queue_t;

bool   queue_init(queue_t *q, size_t capacity);
void   queue_destroy(queue_t *q);

bool   queue_push_blocking(queue_t *q, const frame_t *in);
bool   queue_pop_blocking(queue_t *q, frame_t *out);

void   queue_stop(queue_t *q); // wakes all waiting threads, sets stop=true

#endif