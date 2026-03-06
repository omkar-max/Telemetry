#ifndef SYNC_H
#define SYNC_H

#include <stdbool.h>
#include <semaphore.h>
#include "shm_layout.h"

#define SEM_EMPTY_NAME "/telemetry_empty_v1"
#define SEM_FULL_NAME  "/telemetry_full_v1"
#define SEM_MUTEX_NAME "/telemetry_mutex_v1"

typedef struct {
    sem_t *empty;
    sem_t *full;
    sem_t *mutex;
} rb_sems_t;

bool sems_create(rb_sems_t *s);   // creator process uses this
bool sems_open(rb_sems_t *s);     // other processes use this
void sems_close(rb_sems_t *s);
void sems_unlink_all(void);       // optional cleanup

#endif