#include "logger.h"
#include <string.h>
#include <time.h>

bool logger_init(logger_t *logger, const char *filename)
{
    memset(logger, 0, sizeof(*logger));

    logger->fp = fopen(filename, "w");
    if (!logger->fp) {
        return false;
    }

    if (pthread_mutex_init(&logger->lock, NULL) != 0) {
        fclose(logger->fp);
        logger->fp = NULL;
        return false;
    }

    /* write CSV header row */
    fprintf(logger->fp, "producer_id,sequence_number,worker_id,payload\n");
    fflush(logger->fp);

    return true;
}

void logger_write_csv(logger_t *logger, const frame_t *f, int worker_id)
{
    pthread_mutex_lock(&logger->lock);

    fprintf(logger->fp,
            "%u,%u,%d,%s\n",
            f->producer_id,
            f->sequence_number,
            worker_id,
            (char *)f->payload);

    /*
    Flush immediately so data is visible in file right away.
    Later, for performance, you can remove this and flush in batches.
    */
    fflush(logger->fp);

    pthread_mutex_unlock(&logger->lock);
}

void logger_destroy(logger_t *logger)
{
    if (!logger) return;

    pthread_mutex_destroy(&logger->lock);

    if (logger->fp) {
        fclose(logger->fp);
        logger->fp = NULL;
    }
}