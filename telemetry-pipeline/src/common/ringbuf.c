#include "ringbuf.h"

/*
    rb_init()
    ---------
    Think of this like setting up an empty circular tray:

    - head = 0 (next write goes to slot 0)
    - tail = 0 (next read comes from slot 0)
    - count = 0 (tray has 0 items)
    - capacity = RB_CAPACITY (how many slots exist)
    - drops = 0 (no frames dropped yet)
*/
void rb_init(shm_ring_t *rb)
{
    rb->hdr.capacity = RB_CAPACITY;
    rb->hdr.head = 0;
    rb->hdr.tail = 0;
    rb->hdr.count = 0;
    rb->hdr.drops = 0;
}

/*
    rb_is_empty()
    -------------
    Buffer is empty when count == 0.
*/
bool rb_is_empty(const shm_ring_t *rb)
{
    return rb->hdr.count == 0;
}

/*
    rb_is_full()
    ------------
    Buffer is full when count == capacity.
*/
bool rb_is_full(const shm_ring_t *rb)
{
    return rb->hdr.count == rb->hdr.capacity;
}

/*
    rb_push()
    ---------
    Layman view:

    - If tray is full, you cannot place a new item -> return false.
    - Put the frame into slots[head]
    - Move head forward by 1
      If head reaches end, wrap to 0 using modulo.
    - Increase count by 1
*/
bool rb_push(shm_ring_t *rb, const frame_t *in)
{
    if (rb_is_full(rb)) {
        rb->hdr.drops++;     // track how many pushes failed
        return false;
    }

    rb->slots[rb->hdr.head] = *in;

    rb->hdr.head = (rb->hdr.head + 1) % rb->hdr.capacity;
    rb->hdr.count++;

    return true;
}

/*
    rb_pop()
    --------
    Layman view:

    - If tray is empty, nothing to take -> return false.
    - Read frame from slots[tail] into *out
    - Move tail forward by 1 (wrap around with modulo)
    - Decrease count by 1
*/
bool rb_pop(shm_ring_t *rb, frame_t *out)
{
    if (rb_is_empty(rb)) {
        return false;
    }

    *out = rb->slots[rb->hdr.tail];

    rb->hdr.tail = (rb->hdr.tail + 1) % rb->hdr.capacity;
    rb->hdr.count--;

    return true;
}