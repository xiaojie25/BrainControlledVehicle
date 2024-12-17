#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RingBuffer {
    void * data;
    uint32_t size;
    volatile uint32_t head_index;
    volatile uint32_t tail_index;
    volatile int8_t is_full;
	volatile int8_t is_initialized;
	volatile int8_t need_free;
} RingBuffer;


extern int RingBufferInit(RingBuffer * ring_buffer, uint32_t size);

extern int RingBufferInitWithBuffer(RingBuffer * ring_buffer, void * buffer, uint32_t size);

extern void RingBufferDeinit(RingBuffer * ring_buffer);

extern int RingBufferPop(RingBuffer * ring_buffer, void * data, size_t length);

extern int RingBufferPush(RingBuffer * ring_buffer, const void * data, size_t length);

extern int RingBufferIsFull(const RingBuffer * ring_buffer);

extern int RingBufferIsEmpty(const RingBuffer * ring_buffer);

extern void * RingBufferGetTail(RingBuffer * ring_buffer);

extern void * RingBufferGetHead(RingBuffer * ring_buffer);

extern int RingBufferGetDistanceToBoundary(const RingBuffer * ring_buffer);


// MACROS

#define NEW_RING_BUFFER(_size) {												\
	.data = NULL,																\
	.size = (_size),															\
	.head_index = 0,															\
	.tail_index = 0,															\
	.is_full = 0,	   															\
	.is_initialized = 0,														\
	.need_free = 1																\
}


#define NEW_RING_BUFFER_WITH_BUFFER(_size,_buffer) {							\
	.data = _buffer,															\
	.size = (_size),															\
	.head_index = 0,															\
	.tail_index = 0,															\
	.is_full = 0,	   															\
	.is_initialized = 0,														\
	.need_free = 0																\
}

#ifdef __cplusplus
}
#endif


#endif /* __RING_BUFFER_H__ */  
