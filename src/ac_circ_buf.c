/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_circ_buf.c - A circular buffer of fixed size (power of 2)
 *
 * This can store either pointers to data or copies of the data.
 *
 * Based on include/linux/circ_buf.h from
 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree
 *
 * Copyright (c) 2019 - 2020	Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/libac.h"

/* Buffer type; storing pointers or copying data */
enum { PTR_BUF = 0, CPY_BUF };

/*
 * How many items are in the buffer
 *
 * When storing pointers ->elem_sz will be 1
 */
static inline u32 circ_count(const ac_circ_buf_t *cbuf)
{
	return ((cbuf->head - cbuf->tail) / cbuf->elem_sz) & (cbuf->size - 1);
}

/*
 * How much free space is in the buffer, 0..size-1
 *
 * When storing pointers ->elem_sz will be 1
 */
static inline u32 circ_space(const ac_circ_buf_t *cbuf)
{
	return ((cbuf->tail - (cbuf->head + cbuf->elem_sz)) / cbuf->elem_sz) &
	       (cbuf->size - 1);
}

/*
 * How many contiguous items are in the buffer without wrapping
 * around
 *
 * When storing pointers ->elem_sz will be 1
 */
static inline u32 circ_count_to_end(const ac_circ_buf_t *cbuf)
{
	u32 end = cbuf->size - (cbuf->tail / cbuf->elem_sz);
	u32 n = ((cbuf->head / cbuf->elem_sz) + end) & (cbuf->size - 1);

	return n < end ? n : end;
}

/*
 * How much contiguous free space is in the buffer without
 * wrapping around
 *
 * When storing pointers ->elem_sz will be 1
 */
static inline u32 circ_space_to_end(const ac_circ_buf_t *cbuf)
{
	u32 end = cbuf->size - 1 - (cbuf->head / cbuf->elem_sz);
	u32 n = (end + (cbuf->tail / cbuf->elem_sz)) & (cbuf->size - 1);

	return n <= end ? n : end + 1;
}

static bool is_pow2(u32 val)
{
	return !(val & (val - 1));
}

/**
 * ac_circ_buf_new - create a new circular buffer
 *
 * @size: The required size of the buffer, must be a power of two
 * @elem_sz: The size of the individual elements being placed into
 *           the buffer. Set to 0 for the storing of pointers rather
 *           than copying the data.
 *
 * Returns:
 *
 * A pointer to a newly allocated buffer or NULL on failure
 */
ac_circ_buf_t *ac_circ_buf_new(u32 size, u32 elem_sz)
{
	ac_circ_buf_t *cbuf;

	if (!is_pow2(size))
		return NULL;

	cbuf = malloc(sizeof(ac_circ_buf_t));
	cbuf->head = cbuf->tail = 0;
	cbuf->size = size;

	if (elem_sz == 0) {
		cbuf->elem_sz = 1;
		cbuf->type = PTR_BUF;
		cbuf->buf.ptr_buf = malloc(size * sizeof(void *));
	} else {
		cbuf->elem_sz = elem_sz;
		cbuf->type = CPY_BUF;
		cbuf->buf.cpy_buf = malloc((size_t)size * elem_sz);
	}

	return cbuf;
}

/**
 * ac_circ_buf_count - how many items are in the buffer
 *
 * @cbuf: The circular buffer to work on
 *
 * Returns:
 *
 * The number of items in the buffer
 */
u32 ac_circ_buf_count(const ac_circ_buf_t *cbuf)
{
	return circ_count(cbuf);
}

/**
 * ac_circ_buf_pushm - push multiple items into the buffer
 *
 * @cbuf: The circular buffer to work on
 * @buf: The item(s) to push into the buffer
 * @count: The number of items contained in @buf
 *
 * Returns:
 *
 * 0 on success or -1 if there was no room (contiguous space)
 */
int ac_circ_buf_pushm(ac_circ_buf_t *cbuf, const void *buf, u32 count)
{
	if (circ_space_to_end(cbuf) < count) {
		if (circ_count(cbuf) == 0 && count <= cbuf->size)
			cbuf->head = cbuf->tail = 0;
		else
			return -1;
	}

	if (cbuf->type == PTR_BUF)
		memcpy(cbuf->buf.ptr_buf + cbuf->head, buf,
		       count * sizeof(void *));
	else
		memcpy(cbuf->buf.cpy_buf + cbuf->head, buf,
		       (size_t)count * cbuf->elem_sz);

	cbuf->head = (cbuf->head + (count * cbuf->elem_sz)) &
		     ((cbuf->size - 1) * cbuf->elem_sz);

	return 0;
}

/**
 * ac_circ_buf_push - push an item into the buffer
 *
 * @cbuf: The circular buffer to work on
 * @buf: The item to add
 *
 * Returns:
 *
 * 0 on success or -1 if the buffer is full
 */
int ac_circ_buf_push(ac_circ_buf_t *cbuf, const void *buf)
{
	if (circ_space(cbuf) == 0)
		return -1;

	if (cbuf->type == PTR_BUF)
		cbuf->buf.ptr_buf[cbuf->head] = (void *)buf;
	else
		memcpy(cbuf->buf.cpy_buf + cbuf->head, buf, cbuf->elem_sz);

	cbuf->head = (cbuf->head + cbuf->elem_sz) &
		     ((cbuf->size - 1) * cbuf->elem_sz);

	return 0;
}

/**
 * ac_circ_buf_popm - pop multiple items from buffer
 *
 * @cbuf: The circular buffer to work on
 * @count: The number of items to pop from the buffer
 *
 * Returns:
 *
 * 0 on success and buf will contain the popped items or -1 if there
 * was not enough contiguous items to satisfy @count
 */
int ac_circ_buf_popm(ac_circ_buf_t *cbuf, void *buf, u32 count)
{
	if (circ_count_to_end(cbuf) < count)
		return -1;

	if (cbuf->type == PTR_BUF)
		memcpy(buf, cbuf->buf.ptr_buf + cbuf->tail,
		       count * sizeof(void *));
	else
		memcpy(buf, cbuf->buf.cpy_buf + cbuf->tail,
		       (size_t)count * cbuf->elem_sz);

	cbuf->tail = (cbuf->tail + (count * cbuf->elem_sz)) &
		     ((cbuf->size - 1) * cbuf->elem_sz);

	return 0;
}

/**
 * ac_circ_buf_pop - pop an item from the buffer
 *
 * @cbuf: The circular buffer to work on
 *
 * Returns:
 *
 * A pointer to the popped item on success or NULL if the buffer is empty
 */
void *ac_circ_buf_pop(ac_circ_buf_t *cbuf)
{
	void *item;

	if (circ_count(cbuf) == 0)
		return NULL;

	if (cbuf->type == PTR_BUF)
		item = cbuf->buf.ptr_buf[cbuf->tail];
	else
		item = cbuf->buf.cpy_buf + cbuf->tail;

	cbuf->tail = (cbuf->tail + cbuf->elem_sz) &
		     ((cbuf->size - 1) * cbuf->elem_sz);

	return item;
}

/**
 * ac_circ_buf_foreach - iterate over elements in the circular buffer
 *
 * @cbuf: The circular buffer to work on
 * @action: The function to call on each element
 * @user_data: Optional user data to pass to action. Can be NULL
 */
void ac_circ_buf_foreach(const ac_circ_buf_t *cbuf,
			 void (*action)(void *item, void *data),
			 void *user_data)
{
	u32 i;
	u32 count = circ_count(cbuf);

	if (count == 0)
		return;

	for (i = 0; i < count; i++) {
		u32 k;

		k = (cbuf->tail + (i * cbuf->elem_sz)) &
		    ((cbuf->size - 1) * cbuf->elem_sz);

		if (cbuf->type == PTR_BUF)
			action(cbuf->buf.ptr_buf[k], user_data);
		else
			action(cbuf->buf.cpy_buf + k, user_data);
	}
}

/**
 * ac_circ_buf_reset - reset the circular buffer to empty
 *
 * @cbuf: The circular buffer to work on
 */
void ac_circ_buf_reset(ac_circ_buf_t *cbuf)
{
	cbuf->head = cbuf->tail = 0;
}

/**
 * ac_circ_buf_destroy - destroy a circular buffer
 *
 * @cbuf: The circular buffer to work on
 */
void ac_circ_buf_destroy(const ac_circ_buf_t *cbuf)
{
	if (cbuf->type == PTR_BUF)
		free(cbuf->buf.ptr_buf);
	else
		free(cbuf->buf.cpy_buf);
	free((void *)cbuf);
}
