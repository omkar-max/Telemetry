#ifndef IPC_H
#define IPC_H

/*
    ipc.h
    -----
    This layer is responsible for creating/opening shared memory
    and mapping it into the process using mmap().

    It returns a pointer (shm_ring_t*) that both Producer and Aggregator
    can use with rb_push() / rb_pop().
*/

#include <stdbool.h>
#include "shm_layout.h"

/*
    shm_create_and_map()
    --------------------
    Use this in the process that "creates" shared memory (usually Producer).

    Steps inside:
    1) shm_open(SHM_NAME, O_CREAT | O_RDWR)
    2) ftruncate(fd, sizeof(shm_ring_t))
    3) mmap(... MAP_SHARED ...)

    Returns:
        true  -> success, *out_rb is valid
        false -> failed
*/
bool shm_create_and_map(shm_ring_t **out_rb);

/*
    shm_open_and_map()
    ------------------
    Use this in the process that only "opens" existing shared memory
    (usually Aggregator).

    Steps inside:
    1) shm_open(SHM_NAME, O_RDWR)
    2) mmap(... MAP_SHARED ...)

    Returns:
        true  -> success
        false -> failed
*/
bool shm_open_and_map(shm_ring_t **out_rb);

/*
    shm_unmap()
    -----------
    Unmaps the shared memory region from this process.
*/
void shm_unmap(shm_ring_t *rb);

#endif