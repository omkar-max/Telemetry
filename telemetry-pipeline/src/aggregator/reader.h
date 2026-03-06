#ifndef READER_H
#define READER_H

#include "shm_layout.h"
#include "sync.h"
#include "queue.h"
#include "stats.h"

typedef struct {
    shm_ring_t *rb;
    rb_sems_t  *sems;
    queue_t    *q;
    stats_t    *stats;
} reader_ctx_t;

void *reader_thread(void *arg);

#endif