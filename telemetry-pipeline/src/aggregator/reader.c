#include "reader.h"
#include "ringbuf.h"
#include "queue.h"
#include "sync.h"
#include "stats.h"

void *reader_thread(void *arg)
{
    reader_ctx_t *ctx = (reader_ctx_t *)arg;

    while (1) {
        frame_t f;

        sem_wait(ctx->sems->full);
        sem_wait(ctx->sems->mutex);

        rb_pop(ctx->rb, &f);

        sem_post(ctx->sems->mutex);
        sem_post(ctx->sems->empty);

        /* update ordered receive stats here */
        stats_record_received(ctx->stats, f.producer_id, f.sequence_number);

        if (!queue_push_blocking(ctx->q, &f)) {
            break;
        }
    }

    return NULL;
}