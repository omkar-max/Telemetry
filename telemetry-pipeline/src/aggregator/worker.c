#include <stdio.h>
#include "worker.h"
#include "queue.h"
#include "stats.h"

void *worker_thread(void *arg)
{
    worker_ctx_t *ctx = (worker_ctx_t *)arg;

    while (1) {
        frame_t f;

        if (!queue_pop_blocking(ctx->q, &f)) {
            break;
        }

        // printf("WORKER[%d]: producer=%u seq=%u payload=\"%s\"\n",
        //        ctx->worker_id,
        //        f.producer_id,
        //        f.sequence_number,
        //        (char *)f.payload);

        stats_record_processed(ctx->stats, ctx->worker_id);
    }

    return NULL;
}