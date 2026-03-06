#include "sync.h"
#include <fcntl.h>
#include <stdio.h>

static bool open_one(sem_t **out, const char *name, int oflag, unsigned initval)
{
    sem_t *s = sem_open(name, oflag, 0666, initval);
    if (s == SEM_FAILED) {
        printf("sem_open failed for %s\n", name);
        return false;
    }
    *out = s;
    return true;
}

bool sems_create(rb_sems_t *s)
{
    // Initial values:
    // empty = capacity, full = 0, mutex = 1
    if (!open_one(&s->empty, SEM_EMPTY_NAME, O_CREAT, RB_CAPACITY)) return false;
    if (!open_one(&s->full,  SEM_FULL_NAME,  O_CREAT, 0))          return false;
    if (!open_one(&s->mutex, SEM_MUTEX_NAME, O_CREAT, 1))          return false;
    return true;
}

bool sems_open(rb_sems_t *s)
{
    // open existing (no O_CREAT)
    if (!open_one(&s->empty, SEM_EMPTY_NAME, 0, 0)) return false;
    if (!open_one(&s->full,  SEM_FULL_NAME,  0, 0)) return false;
    if (!open_one(&s->mutex, SEM_MUTEX_NAME, 0, 0)) return false;
    return true;
}

void sems_close(rb_sems_t *s)
{
    if (!s) return;
    if (s->empty) sem_close(s->empty);
    if (s->full)  sem_close(s->full);
    if (s->mutex) sem_close(s->mutex);
    s->empty = s->full = s->mutex = NULL;
}

void sems_unlink_all(void)
{
    sem_unlink(SEM_EMPTY_NAME);
    sem_unlink(SEM_FULL_NAME);
    sem_unlink(SEM_MUTEX_NAME);
}