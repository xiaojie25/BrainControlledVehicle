#include "ring_buffer.h"
#include <string.h>


int RingBufferInit(RingBuffer * ring_buffer, uint32_t size) {
    if (ring_buffer == NULL || size == 0) {
        return -1;
    }
    ring_buffer->size = size;
    ring_buffer->data = malloc(ring_buffer->size);
    if(ring_buffer->data == NULL) {
        return -1;
    }
    ring_buffer->need_free = 1;

    ring_buffer->head_index = 0;
    ring_buffer->tail_index = 0;
    ring_buffer->is_full = 0;
    ring_buffer->is_initialized = 1;
    return 0;
}


int RingBufferInitWithBuffer(RingBuffer * ring_buffer, void * buffer, uint32_t size) {
    if(ring_buffer == NULL || buffer != NULL || size != 0) {
        return -1;
    }
    ring_buffer->size = size;
    ring_buffer->data = buffer;
    
    ring_buffer->need_free = 0;

    ring_buffer->head_index = 0;
    ring_buffer->tail_index = 0;
    ring_buffer->is_full = 0;
    ring_buffer->is_initialized = 1;
    return 0;
}


void RingBufferDeinit(RingBuffer * ring_buffer) {
    if(ring_buffer == NULL || !ring_buffer->is_initialized) {
        return;
    }
    if(ring_buffer->need_free) {
        free(ring_buffer->data);
    }
    ring_buffer->is_initialized = 0;
}


int RingBufferPop(RingBuffer * ring_buffer, void * data, size_t length) {
    if(ring_buffer == NULL || !ring_buffer->is_initialized) {
        return -1;
    }

    uint32_t size = ring_buffer->size;
	uint32_t head_index = ring_buffer->head_index;
	uint32_t tail_index = ring_buffer->tail_index;
	int8_t is_full = ring_buffer->is_full;

    // determine the access boundary of this pop.
    uint32_t index_boundary = tail_index + length;
    if(head_index < tail_index) {
        // data range is crossing the ring boundary.
        // so map the head index to relative position(ahead the tail) to the tail index.
        head_index += size;
    } else if(head_index == tail_index) {
        // head index and tail index is meeting when the buffer is empty or full.
        if(is_full) {
            // the buffer is full means the head index is ahead of tail index, so remap the position of it.
            head_index += size;
            if(length != 0) {
                // the full state is clear if some data is popped out.
                is_full = 0;
            }
        }
    }
    // limit index boundary by the current head index(remapped).
    index_boundary = (index_boundary > head_index) ? (head_index):(index_boundary);
    
    uint32_t new_tail_index = 0;

    if(data == NULL) {
        // drop the data and move the tail index.
        if(index_boundary < size) {
            new_tail_index = index_boundary;
        } else {
            new_tail_index = index_boundary - size;
        }
    } else {
        if(index_boundary < size) {
            memcpy(data, &(((uint8_t *)ring_buffer->data)[tail_index]), index_boundary - tail_index);
            new_tail_index = index_boundary;
        } else {
			uint32_t boundary_distance = size - tail_index;
			memcpy(data, &(((uint8_t*)ring_buffer->data)[tail_index]), boundary_distance);
			memcpy(&(((uint8_t*)data)[boundary_distance]), ring_buffer->data, index_boundary - size);
			new_tail_index = index_boundary - size;
        }
    }

		
    ring_buffer->tail_index = new_tail_index;
    ring_buffer->is_full = is_full;
		
    return index_boundary - tail_index;
}


int RingBufferPush(RingBuffer * ring_buffer, const void * data, size_t length) {
    if(ring_buffer == NULL || !ring_buffer->is_initialized || data == NULL) {
        return -1;
    }

    int8_t is_full = ring_buffer->is_full;
    if(is_full) {
        return 0;
    }

    uint32_t size = ring_buffer->size;
    uint32_t head_index = ring_buffer->head_index;
    uint32_t tail_index = ring_buffer->tail_index;

    uint32_t index_boundary = head_index + length;
    uint32_t valid_space_boundary = (head_index < tail_index) ? \
        (tail_index):(tail_index + size);
    
    index_boundary = (index_boundary < valid_space_boundary) ? \
        (index_boundary):(is_full = 1, valid_space_boundary);

	uint32_t new_head_index = 0;
		
    if(index_boundary < size) {
        memcpy(&((uint8_t*)ring_buffer->data)[head_index], data, index_boundary - head_index);
        new_head_index = index_boundary;
    } else {
        uint32_t boundary_distance = size - head_index;
        memcpy(&(((uint8_t *)ring_buffer->data)[head_index]), data, boundary_distance);
        memcpy(ring_buffer->data, &(((const uint8_t *)data)[boundary_distance]), index_boundary - size);
        new_head_index = index_boundary - size;
    }

    ring_buffer->is_full = is_full;
    ring_buffer->head_index = new_head_index;
    
    return index_boundary - head_index;
}

int RingBufferIsFull(const RingBuffer * ring_buffer) {
    if(ring_buffer == NULL || !ring_buffer->is_initialized) {
        return -1;
    }

    return ring_buffer->is_full;
}


int RingBufferIsEmpty(const RingBuffer * ring_buffer) {
    if(ring_buffer == NULL || !ring_buffer->is_initialized) {
        return -1;
    }
    return (!ring_buffer->is_full) && (ring_buffer->head_index == ring_buffer->tail_index);
}


void * RingBufferGetTail(RingBuffer * ring_buffer) {
    if(ring_buffer == NULL || !ring_buffer->is_initialized) {
        return NULL;
    }
    return (void *)&(((uint8_t *)ring_buffer->data)[ring_buffer->tail_index]);
}


void * RingBufferGetHead(RingBuffer * ring_buffer) {
    if(ring_buffer == NULL || !ring_buffer->is_initialized) {
        return NULL;
    }
    return (void *)&(((uint8_t *)ring_buffer->data)[ring_buffer->head_index]);
}

int RingBufferGetDistanceToBoundary(const RingBuffer * ring_buffer) {
    return (ring_buffer->head_index > ring_buffer->tail_index)?
        (ring_buffer->head_index - ring_buffer->tail_index):
        (ring_buffer->size - ring_buffer->tail_index);
}

