#pragma once
/*
  shm_layout.h
  ------------
  This file defines exactly what the shared memory looks like in RAM.

  Both Producer and Aggregator will mmap() the same shared memory object.
  That means both processes will see the same:
    - head, tail, count
    - slots array of frames

  This is the "shape" of our shared memory region.
*/

#include <stdint.h>
#include "protocol.h"

/*
  SHM_NAME:
  ---------
  This is the shared memory object name used with shm_open().
  On Linux, it will appear like a file under:
    /dev/shm/telemetry_shm_v1
*/
#define SHM_NAME "/telemetry_shm_v1"

/*
  RB_CAPACITY:
  ------------
  Number of slots in the ring buffer.
  Each slot holds one frame_t.

  Start with 1024. Later you can tune this based on:
    capacity = peak_rate * worst_case_consumer_delay
*/
#define RB_CAPACITY 1024

/*
  rb_hdr_t
  --------
  This is the "metadata" for the ring buffer.
  Think of it like the labels on the tray:

  head  -> where next write goes
  tail  -> where next read comes from
  count -> how many items are currently stored
  drops -> how many frames were dropped because buffer was full
*/
typedef struct {
    uint32_t capacity;   // fixed: RB_CAPACITY (for sanity checking)
    uint32_t head;       // write index (0..capacity-1)
    uint32_t tail;       // read index  (0..capacity-1)
    uint32_t count;      // number of frames currently in buffer (0..capacity)
    uint64_t drops;      // how many pushes failed because it was full
} rb_hdr_t;

/*
  shm_ring_t
  ----------
  This is the full shared memory layout:
    [ header ][ slots array ]

  Both processes mmap() this exact structure.
*/
typedef struct {
    rb_hdr_t hdr;
    frame_t  slots[RB_CAPACITY];
} shm_ring_t;