#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
#include "queue.h"
#include "shm_layout.h"

#define MAX_PRODUCERS 16
#define MAX_WORKERS   16

typedef struct {
    pthread_mutex_t lock;

    uint64_t total_received;
    uint64_t total_processed;
    uint64_t seq_gaps_detected;

    uint64_t producer_counts[MAX_PRODUCERS];
    uint64_t worker_counts[MAX_WORKERS];

    uint32_t last_seq[MAX_PRODUCERS];
    bool     seen_producer[MAX_PRODUCERS];
} stats_t;

bool stats_init(stats_t *s);
void stats_destroy(stats_t *s);

/* called by reader thread */
void stats_record_received(stats_t *s, uint32_t producer_id, uint32_t seq);

/* called by worker thread */
void stats_record_processed(stats_t *s, int worker_id);

/* called by stats thread */
void stats_snapshot(stats_t *s,
                    uint64_t *total_received,
                    uint64_t *total_processed,
                    uint64_t *seq_gaps_detected,
                    uint64_t producer_counts[MAX_PRODUCERS],
                    uint64_t worker_counts[MAX_WORKERS]);

#endif