#include "stats.h"
#include <string.h>

bool stats_init(stats_t *s)
{
    memset(s, 0, sizeof(*s));
    if (pthread_mutex_init(&s->lock, NULL) != 0) {
        return false;
    }
    return true;
}

void stats_destroy(stats_t *s)
{
    pthread_mutex_destroy(&s->lock);
}

void stats_record_received(stats_t *s, uint32_t producer_id, uint32_t seq)
{
    pthread_mutex_lock(&s->lock);

    s->total_received++;

    if (producer_id < MAX_PRODUCERS) {
        s->producer_counts[producer_id]++;

        if (!s->seen_producer[producer_id]) {
            s->seen_producer[producer_id] = true;
        } else {
            uint32_t diff = seq - s->last_seq[producer_id];

            if (diff > 1) {
                s->seq_gaps_detected += (uint64_t)(diff - 1);
            }
        }

        s->last_seq[producer_id] = seq;
    }

    pthread_mutex_unlock(&s->lock);
}

void stats_record_processed(stats_t *s, int worker_id)
{
    pthread_mutex_lock(&s->lock);

    s->total_processed++;

    if (worker_id >= 0 && worker_id < MAX_WORKERS) {
        s->worker_counts[worker_id]++;
    }

    pthread_mutex_unlock(&s->lock);
}

void stats_snapshot(stats_t *s,
                    uint64_t *total_received,
                    uint64_t *total_processed,
                    uint64_t *seq_gaps_detected,
                    uint64_t producer_counts[MAX_PRODUCERS],
                    uint64_t worker_counts[MAX_WORKERS])
{
    pthread_mutex_lock(&s->lock);

    *total_received = s->total_received;
    *total_processed = s->total_processed;
    *seq_gaps_detected = s->seq_gaps_detected;

    for (int i = 0; i < MAX_PRODUCERS; i++) {
        producer_counts[i] = s->producer_counts[i];
    }

    for (int i = 0; i < MAX_WORKERS; i++) {
        worker_counts[i] = s->worker_counts[i];
    }

    pthread_mutex_unlock(&s->lock);
}