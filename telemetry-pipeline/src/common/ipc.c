#include "ipc.h"

#include <fcntl.h>      // O_CREAT, O_RDWR
#include <sys/mman.h>   // mmap, munmap, MAP_SHARED
#include <sys/stat.h>   // mode constants
#include <unistd.h>     // ftruncate, close
#include <stdio.h>      // printf (basic prints)

/*
    Helper:
    How many bytes do we need in shared memory?

    Because shm_layout.h defines:
        typedef struct { header + slots[] } shm_ring_t;

    So shared memory size is exactly:
        sizeof(shm_ring_t)
*/
static size_t shm_bytes(void)
{
    return sizeof(shm_ring_t);
}

bool shm_create_and_map(shm_ring_t **out_rb)
{
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        printf("shm_open create failed\n");
        return false;
    }

    // Set correct size of shared memory object
    if (ftruncate(fd, (off_t)shm_bytes()) != 0) {
        printf("ftruncate failed\n");
        close(fd);
        return false;
    }

    // Map into this process
    void *ptr = mmap(NULL, shm_bytes(),
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     fd, 0);

    close(fd);

    if (ptr == MAP_FAILED) {
        printf("mmap failed\n");
        return false;
    }

    *out_rb = (shm_ring_t *)ptr;
    return true;
}

bool shm_open_and_map(shm_ring_t **out_rb)
{
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd < 0) {
        printf("shm_open open failed (does it exist?)\n");
        return false;
    }

    void *ptr = mmap(NULL, shm_bytes(),
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     fd, 0);

    close(fd);

    if (ptr == MAP_FAILED) {
        printf("mmap failed\n");
        return false;
    }

    *out_rb = (shm_ring_t *)ptr;
    return true;
}

void shm_unmap(shm_ring_t *rb)
{
    if (rb == NULL) return;
    munmap((void *)rb, shm_bytes());
}