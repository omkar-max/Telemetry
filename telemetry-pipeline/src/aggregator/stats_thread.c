#include <stdio.h>
#include <unistd.h>
#include "stats_thread.h"
#include "stats.h"
#include "queue.h"
#include "shm_layout.h"

void *stats_thread(void *arg)
{
    stats_thread_ctx_t *ctx = (stats_thread_ctx_t *)arg;

    uint64_t prev_processed = 0;

    while (1) {
        sleep(1);

        uint64_t total_received = 0;
        uint64_t total_processed = 0;
        uint64_t seq_gaps = 0;
        uint64_t producer_counts[MAX_PRODUCERS] = {0};
        uint64_t worker_counts[MAX_WORKERS] = {0};

        stats_snapshot(ctx->stats,
                       &total_received,
                       &total_processed,
                       &seq_gaps,
                       producer_counts,
                       worker_counts);

        uint64_t throughput = total_processed - prev_processed;
        prev_processed = total_processed;

        size_t q_count = 0;
        pthread_mutex_lock(&ctx->q->lock);
        q_count = ctx->q->count;
        pthread_mutex_unlock(&ctx->q->lock);

        printf("\n----- STATS -----\n");
        printf("received_total   : %llu\n", (unsigned long long)total_received);
        printf("processed_total  : %llu\n", (unsigned long long)total_processed);
        printf("throughput       : %llu frames/sec\n", (unsigned long long)throughput);
        printf("queue_depth      : %zu / %zu\n", q_count, ctx->q->cap);
        printf("shared_drops     : %llu\n", (unsigned long long)ctx->rb->hdr.drops);
        printf("seq_gaps_detected: %llu\n", (unsigned long long)seq_gaps);

        printf("per producer:\n");
        for (int i = 0; i < MAX_PRODUCERS; i++) {
            if (producer_counts[i] > 0) {
                printf("  P%d: %llu\n", i, (unsigned long long)producer_counts[i]);
            }
        }

        printf("per worker:\n");
        for (int i = 0; i < MAX_WORKERS; i++) {
            if (worker_counts[i] > 0) {
                printf("  W%d: %llu\n", i, (unsigned long long)worker_counts[i]);
            }
        }

        printf("-----------------\n\n");
    }

    return NULL;
}