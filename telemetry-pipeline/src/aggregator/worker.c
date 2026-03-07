#include <stdio.h>
#include "worker.h"
#include "queue.h"
#include "stats.h"
#include "logger.h"

void *worker_thread(void *arg)
{
    worker_ctx_t *ctx = (worker_ctx_t *)arg;

    while (1) {
        frame_t f;

        if (!queue_pop_blocking(ctx->q, &f)) {
            break;
        }

        /* console print (optional, can keep for debugging) */
        // printf("WORKER[%d]: producer=%u seq=%u payload=\"%s\"\n",
        //        ctx->worker_id,
        //        f.producer_id,
        //        f.sequence_number,
        //        (char *)f.payload);

        /* write structured CSV log */
        logger_write_csv(ctx->logger, &f, ctx->worker_id);

        /* update stats */
        stats_record_processed(ctx->stats, ctx->worker_id);
    }

    return NULL;
}