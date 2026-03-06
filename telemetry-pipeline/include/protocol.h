#ifndef PROTOCOL_H
#define PROTOCOL_H

/*
    protocol.h
    ----------
    Defines the data frame that travels through the system.

    Producer Process  --->  Shared Memory Ring Buffer  --->  Aggregator Process

    Every slot in the ring buffer will contain exactly one frame_t.
*/

#include <stdint.h>

/*
    PAYLOAD_SIZE

    The number of bytes reserved for actual data.

    Why fixed size?
    - Easy to store in shared memory
    - No dynamic allocation
    - Predictable memory layout
*/
#define PAYLOAD_SIZE 48


/*
    frame_t

    This structure represents one "message" produced by a producer
    and consumed by the aggregator.

    Layout must remain consistent across all processes that use
    the shared memory.
*/

typedef struct
{
    /*
        Timestamp in nanoseconds.

        Used for:
        - latency measurement
        - ordering events
        - debugging

        Example usage:
        clock_gettime(CLOCK_MONOTONIC)
    */
    uint64_t timestamp_ns;


    /*
        ID of the producer process.

        Example:
            Producer 1
            Producer 2
            Producer 3

        Helps aggregator know which process produced the frame.
    */
    uint32_t producer_id;


    /*
        Sequence number.

        Each producer increments this value every time it
        generates a frame.

        Used to detect:
        - dropped messages
        - ordering issues
    */
    uint32_t sequence_number;


    /*
        Actual data payload.

        Could contain:
        - sensor data
        - telemetry message
        - encoded binary data
        - text message

        Fixed size keeps the ring buffer simple.
    */
    uint8_t payload[PAYLOAD_SIZE];

} frame_t;

#endif