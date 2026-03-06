#include "queue.h"
#include <stdlib.h>
#include <string.h>

bool queue_init(queue_t *q, size_t capacity)
{
    memset(q, 0, sizeof(*q));
    q->buf = (frame_t *)calloc(capacity, sizeof(frame_t));
    if (!q->buf) return false;

    q->cap = capacity;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    q->stop = false;

    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);

    return true;
}

void queue_destroy(queue_t *q)
{
    if (!q) return;

    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);

    free(q->buf);
    q->buf = NULL;
}

void queue_stop(queue_t *q)
{
    pthread_mutex_lock(&q->lock);
    q->stop = true;

    // Wake everyone so they can exit if they want
    pthread_cond_broadcast(&q->not_empty);
    pthread_cond_broadcast(&q->not_full);

    pthread_mutex_unlock(&q->lock);
}

/*
  queue_push_blocking()
  ---------------------
  Reader thread calls this.
  If queue is full, reader will sleep until a worker pops something.
*/
bool queue_push_blocking(queue_t *q, const frame_t *in)
{
    pthread_mutex_lock(&q->lock);

    while (!q->stop && q->count == q->cap) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    if (q->stop) {
        pthread_mutex_unlock(&q->lock);
        return false;
    }

    q->buf[q->head] = *in;
    q->head = (q->head + 1) % q->cap;
    q->count++;

    // queue has at least one item now → wake one worker
    pthread_cond_signal(&q->not_empty);

    pthread_mutex_unlock(&q->lock);
    return true;
}

/*
  queue_pop_blocking()
  --------------------
  Worker thread calls this.
  If queue is empty, worker sleeps until reader pushes something.
*/
bool queue_pop_blocking(queue_t *q, frame_t *out)
{
    pthread_mutex_lock(&q->lock);

    while (!q->stop && q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    if (q->stop && q->count == 0) {
        pthread_mutex_unlock(&q->lock);
        return false;
    }

    *out = q->buf[q->tail];
    q->tail = (q->tail + 1) % q->cap;
    q->count--;

    // queue has free space now → wake reader
    pthread_cond_signal(&q->not_full);

    pthread_mutex_unlock(&q->lock);
    return true;
}