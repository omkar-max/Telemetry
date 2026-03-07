#include <stdio.h>
#include <pthread.h>

#include "ipc.h"
#include "sync.h"
#include "queue.h"
#include "stats.h"

#include "reader.h"
#include "worker.h"
#include "logger.h"
#include "stats_thread.h"

#define INTERNAL_Q_CAP 512
#define NUM_WORKERS    2

int main(void)
{
    printf("aggregator main started\n");
    fflush(stdout);
    shm_ring_t *rb = NULL;

    if (!shm_open_and_map(&rb)) {
        printf("Aggregator: shm_open_and_map failed\n");
        return 1;
    }
 

    rb_sems_t sems;
    if (!sems_open(&sems)) {
        printf("Aggregator: sems_open failed\n");
        return 1;
    }

    // printf("opened semaphores\n");
    // fflush(stdout);

    queue_t q;
    if (!queue_init(&q, INTERNAL_Q_CAP)) {
        printf("Aggregator: queue_init failed\n");
        return 1;
    }

    stats_t stats;
    if (!stats_init(&stats)) {
        printf("Aggregator: stats_init failed\n");
        return 1;
    }
    // printf("stats initialized\n");
    // fflush(stdout);
    logger_t logger;
    if (!logger_init(&logger, "telemetry.csv")) {
        printf("Aggregator: logger_init failed\n");
        return 1;
    }

    pthread_t reader_tid;
    reader_ctx_t rctx = {
        .rb = rb,
        .sems = &sems,
        .q = &q,
        .stats = &stats
    };

    if (pthread_create(&reader_tid, NULL, reader_thread, &rctx) != 0) {
        printf("Aggregator: failed to create reader thread\n");
        return 1;
    }
    // printf("reader thread created\n");
    // fflush(stdout);

    pthread_t worker_tids[NUM_WORKERS];
    worker_ctx_t wctx[NUM_WORKERS];

    for (int i = 0; i < NUM_WORKERS; i++) {
        wctx[i].q = &q;
        wctx[i].stats = &stats;
        wctx[i].worker_id = i;
        wctx[i].logger = &logger;

        if (pthread_create(&worker_tids[i], NULL, worker_thread, &wctx[i]) != 0) {
            printf("Aggregator: failed to create worker thread %d\n", i);
            return 1;
        }
    }
    // printf("worker threads created\n");
    // fflush(stdout);

    pthread_t stats_tid;
    stats_thread_ctx_t sctx = {
        .stats = &stats,
        .q = &q,
        .rb = rb
    };

    if (pthread_create(&stats_tid, NULL, stats_thread, &sctx) != 0) {
        printf("Aggregator: failed to create stats thread\n");
        return 1;
    }
    // printf("before creating stats thread\n");
    // fflush(stdout);
    printf("Aggregator started: 1 reader + %d workers + 1 stats thread\n", NUM_WORKERS);

    pthread_join(reader_tid, NULL);
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(worker_tids[i], NULL);
    }
    pthread_join(stats_tid, NULL);

    queue_destroy(&q);
    stats_destroy(&stats);
    logger_destroy(&logger);
    sems_close(&sems);
    shm_unmap(rb);

    return 0;
}