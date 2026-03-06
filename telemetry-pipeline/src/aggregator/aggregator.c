#include <stdio.h>
#include <string.h>   // memcpy
#include <unistd.h>   // usleep

#include "ipc.h"
#include "ringbuf.h"
#include "protocol.h"
#include "sync.h"

/*
    aggregator.c
    ------------
    This program is the consumer.

    It will:
    1) Open + mmap existing shared memory (shm_open_and_map)
    2) Pop frames from ring buffer (rb_pop)
    3) Print them

    For now, if buffer is empty, it will sleep and retry.
*/

int main(void)
{
    shm_ring_t *rb = NULL;

    // Open existing shared memory created by producer
    if (!shm_open_and_map(&rb)) {
        printf("Aggregator: shm_open_and_map failed (start producer first)\n");
        return 1;
    }

    rb_sems_t sems;
    if (!sems_open(&sems)) {
        printf("Aggregator: sems_open failed (start creator producer first)\n");
        return 1;
    }

    printf("Aggregator started. Reading frames...\n");

    while (1)
    {
        frame_t f;

        sem_wait(sems.full);   // wait until at least 1 item exists
        sem_wait(sems.mutex);  // lock header + slot

        rb_pop(rb, &f);

        sem_post(sems.mutex);  // unlock
        sem_post(sems.empty);  // one slot became free
        // Print frame
        printf("AGG: producer=%u seq=%u payload=\"%s\" | count=%u drops=%llu\n",
               f.producer_id,
               f.sequence_number,
               (char *)f.payload,
               rb->hdr.count,
               (unsigned long long)rb->hdr.drops);
    }

    // not reached in this simple version
    shm_unmap(rb);
    return 0;
}