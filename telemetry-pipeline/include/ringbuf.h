#ifndef RINGBUF_H
#define RINGBUF_H

/*
    ringbuf.h
    ---------
    Circular buffer (ring buffer) operations on shm_ring_t.

    This layer should NOT care whether memory is shared or not.
    It simply manipulates:
        head, tail, count
    and reads/writes frame slots.

    Synchronization (semaphores/mutex) will be added later outside this layer.
*/

#include <stdbool.h>
#include "shm_layout.h"
#include "protocol.h"

/*
    rb_init()
    ---------
    Initializes ring buffer header values.

    NOTE:
    In a multi-process program, you must ensure rb_init() is called only once
    when the shared memory is created. (We will handle that later.)
*/
void rb_init(shm_ring_t *rb);

/*
    rb_is_empty()
    -------------
    Returns true if buffer has no frames.
*/
bool rb_is_empty(const shm_ring_t *rb);

/*
    rb_is_full()
    ------------
    Returns true if buffer cannot accept more frames.
*/
bool rb_is_full(const shm_ring_t *rb);

/*
    rb_push()
    ---------
    Inserts a new frame into the ring buffer.
    Returns:
        true  -> success
        false -> buffer full (push failed)
*/
bool rb_push(shm_ring_t *rb, const frame_t *in);

/*
    rb_pop()
    --------
    Removes one frame from the ring buffer.
    Returns:
        true  -> success (frame written into *out)
        false -> buffer empty (nothing to pop)
*/
bool rb_pop(shm_ring_t *rb, frame_t *out);

#endif