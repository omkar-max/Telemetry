#include <stdio.h>
#include <string.h>     // memset, snprintf
#include <stdlib.h>     // atoi
#include <unistd.h>     // usleep

#include "ipc.h"
#include "ringbuf.h"
#include "protocol.h"
#include "sync.h"

/*
    producer.c
    ----------
    This program acts like a "sensor producer".

    It will:
    1) Create + mmap shared memory (shm_create_and_map)
    2) Initialize ring buffer header (rb_init)
    3) Generate frames and push into ring buffer (rb_push)
*/

int main(int argc, char *argv[])
{
    // Producer id passed as argument: ./producer 1

   // ./producer 1 1 → producer id 1, creator yes

    //./producer 2 0 → producer id 2, creator no

    uint32_t producer_id = 1;
    int creator = 0;

    if (argc >= 2) producer_id = (uint32_t)atoi(argv[1]);
    if (argc >= 3) creator = atoi(argv[2]);

    shm_ring_t *rb = NULL;

    // Create shared memory and map it into this process
    rb_sems_t sems;

     /*
        Step 1: shared memory mapping
        - Creator creates and sets size
        - Others just open existing
    */
    if (creator) {
        if (!shm_create_and_map(&rb)) {
            printf("Producer: shm_create_and_map failed\n");
            return 1;
        }
    } else {
        if (!shm_open_and_map(&rb)) {
            printf("Producer: shm_open_and_map failed (start creator producer first)\n");
            return 1;
        }
    }




    if (creator) {
        if (!sems_create(&sems)) {
            printf("Producer: sems_create failed\n");
            return 1;
        }
        rb_init(rb);  // ONLY creator initializes ring header
    } 
    else {
        if (!sems_open(&sems)) {
            printf("Producer: sems_open failed (start creator producer first)\n");
            return 1;
        }
    }

    printf("Producer %u started. Writing frames...\n", producer_id);

    uint32_t seq = 1;

    while (1)
    {
        frame_t f;
        memset(&f, 0, sizeof(f));

        // Fill metadata
        f.producer_id = producer_id;
        f.sequence_number = seq++;

        // Fill payload (simple text message for now)
        snprintf((char *)f.payload, PAYLOAD_SIZE,
                 "Hello from Producer %u seq=%u",
                 producer_id, f.sequence_number);

        // Push into ring buffer
        sem_wait(sems.empty);  // wait for free slot
        sem_wait(sems.mutex);  // lock header + slot

        rb_push(rb, &f);       // will succeed because we reserved a slot

        sem_post(sems.mutex);  // unlock
        sem_post(sems.full);   // tell consumer "one item is ready"
    }

    // not reached in this simple version
    shm_unmap(rb);
    return 0;
}