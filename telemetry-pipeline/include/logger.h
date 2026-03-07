#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "protocol.h"

/*
logger_t

This structure manages one shared log file.

All worker threads will use the same logger object.
A mutex protects file writes so log lines do not get mixed.
*/
typedef struct {
    FILE *fp;
    pthread_mutex_t lock;
} logger_t;

/* open log file and initialize mutex */
bool logger_init(logger_t *logger, const char *filename);

/* write one CSV row safely */
void logger_write_csv(logger_t *logger, const frame_t *f, int worker_id);

/* flush and close file */
void logger_destroy(logger_t *logger);

#endif