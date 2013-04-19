/**************************************************************************
 * Lockless ring buffer implementation, thanks to pulseaudio
 **************************************************************************/

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: replace memory barrier on something real if you want to use this
         ring buffer from different threads */
#define MEM_BARRIER do {} while(0)

/* Lockless ring buffer structure */
struct ring_buffer
{
	uint32_t size;
	uint32_t read_idx;
	uint32_t write_idx;
	uint32_t low_mask;
	uint32_t high_mask;
	uintptr_t data_ptr;
};

/* Init ring buffer, buffer_size must be 2^N */
static inline uint32_t ring_buffer_init(struct ring_buffer* rbuf,
										void* ptr, uint32_t size)
{
	/* Must be 2^N */
	if (((size-1) & size) != 0)
		return -1;

	rbuf->size = size;
	rbuf->read_idx = 0;
	rbuf->write_idx = 0;
	rbuf->low_mask = rbuf->size - 1;
	rbuf->high_mask = rbuf->size * 2 - 1;
	rbuf->data_ptr = (uintptr_t)ptr;

	return 0;
}

/* Reset reads/writes */
static inline void ring_buffer_reset(struct ring_buffer* rbuf)
{
	rbuf->read_idx = 0;
	rbuf->write_idx = 0;
}

/* Returns total size of ring buffer */
static inline uint32_t ring_buffer_total_size(const struct ring_buffer* rbuf)
{
	return rbuf->size;
}

/* Returns used size, i.e. size available for read of ring buffer */
static inline uint32_t ring_buffer_used_size(const struct ring_buffer* rbuf)
{
	MEM_BARRIER;
	return (rbuf->write_idx - rbuf->read_idx) & rbuf->high_mask;
}

/* Returns free size, i.e. size available for write of ring buffer */
static inline uint32_t ring_buffer_free_size(const struct ring_buffer* rbuf)
{
	return rbuf->size - ring_buffer_used_size(rbuf);
}

/* Commit write pointer */
static inline void ring_buffer_write_advance(struct ring_buffer* rbuf,
											 uint32_t size)
{
	MEM_BARRIER;
	rbuf->write_idx = (rbuf->write_idx + size) & rbuf->high_mask;
}

/* Commit read pointer */
static inline void ring_buffer_read_advance(struct ring_buffer* rbuf,
											uint32_t size)
{

	MEM_BARRIER;
	rbuf->read_idx = (rbuf->read_idx + size) & rbuf->high_mask;
}

/* Get write pointers (bottom and top halfs of ring buffer) */
static inline uint32_t ring_buffer_write_ptr(const struct ring_buffer* rbuf,
											 uint32_t size,
											 void** p1, uint32_t* sz1,
											 void** p2, uint32_t* sz2)
{
	uint32_t free_size = ring_buffer_free_size(rbuf);
	uint32_t idx = rbuf->write_idx & rbuf->low_mask;

	if (size == 0 || size > free_size)
		size = free_size;

	if (idx + size > rbuf->size) {
		*p1 = (void*)(uintptr_t)(rbuf->data_ptr + idx);
		*sz1 = rbuf->size - idx;
		*p2 = (void*)(rbuf->data_ptr);
		*sz2 = size - (*sz1);
	}
	else {
		*p1 = (void*)(uintptr_t)(rbuf->data_ptr + idx);
		*sz1 = size;
		*p2 = NULL;
		*sz2 = 0;
	}

	return size;
}

/* Get read pointers (bottom and top halfs of ring buffer) */
static inline uint32_t ring_buffer_read_ptr(const struct ring_buffer* rbuf,
											uint32_t size,
											void** p1, uint32_t* sz1,
											void** p2, uint32_t* sz2)
{
	uint32_t used_size = ring_buffer_used_size(rbuf);
	uint32_t idx = rbuf->read_idx & rbuf->low_mask;

	if (size == 0 || size > used_size)
		size = used_size;

	if (idx + size > rbuf->size) {
		*p1 = (void*)(uintptr_t)(rbuf->data_ptr + idx);
		*sz1 = rbuf->size - idx;
		*p2 = (void*)(rbuf->data_ptr);
		*sz2 = size - (*sz1);
	}
	else {
		*p1 = (void*)(uintptr_t)(rbuf->data_ptr + idx);
		*sz1 = size;
		*p2 = NULL;
		*sz2 = 0;
	}

	return size;
}


#ifdef __cplusplus
}
#endif

#endif //RING_BUFFER_H
