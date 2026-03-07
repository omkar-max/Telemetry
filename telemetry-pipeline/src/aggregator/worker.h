#ifndef WORKER_H
#define WORKER_H

#include "queue.h"
#include "stats.h"
#include "logger.h"

typedef struct {
    queue_t *q;
    stats_t *stats;
    logger_t *logger;
    int worker_id;
} worker_ctx_t;

void *worker_thread(void *arg);

#endif