#ifndef STATS_THREAD_H
#define STATS_THREAD_H

#include "stats.h"
#include "queue.h"
#include "shm_layout.h"

typedef struct {
    stats_t     *stats;
    queue_t     *q;
    shm_ring_t  *rb;
} stats_thread_ctx_t;

void *stats_thread(void *arg);

#endif