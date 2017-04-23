/*
 * ac_cqueue.c - Circular queue
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdbool.h>

#include "include/libac.h"

static void null_free_item(void *data)
{
}

ac_cqueue_t *ac_cqueue_new(size_t size, void (*free_item)(void *item))
{
	ac_cqueue_t *cqueue = malloc(sizeof(struct ac_cqueue_t));

	cqueue->queue = malloc(size * sizeof(void *));
	cqueue->front = 0;
	cqueue->rear = 0;
	cqueue->count = 0;
	cqueue->size = size;

	if (!free_item)
		cqueue->free_item = null_free_item;
	else
		cqueue->free_item = free_item;

	return cqueue;
}

int ac_cqueue_push(ac_cqueue_t *cqueue, void *item)
{
	if (cqueue->count == cqueue->size)
		return -1;

	if (cqueue->rear == cqueue->size)
		cqueue->rear = 0;

	cqueue->queue[cqueue->rear] = item;
	cqueue->rear++;
	cqueue->count++;

	return 0;
}

void *ac_cqueue_pop(ac_cqueue_t *cqueue)
{
	if (cqueue->count == 0)
		return NULL;
	else if (cqueue->front == cqueue->size)
		cqueue->front = 0;
	cqueue->count--;

	return cqueue->queue[cqueue->front++];
}

void ac_cqueue_foreach(const ac_cqueue_t *cqueue,
		       void (*action)(void *item, void *data), void *user_data)
{
	size_t i;

	if (cqueue->count == 0)
		return;

	if (cqueue->front < cqueue->rear) {
		for (i = cqueue->front; i < cqueue->rear; i++)
			action(cqueue->queue[i], user_data);
	} else {
		for (i = cqueue->front; i < cqueue->size; i++)
			action(cqueue->queue[i], user_data);
		for (i = 0; i < cqueue->rear; i++)
			action(cqueue->queue[i], user_data);
	}
}

bool ac_cqueue_is_empty(const ac_cqueue_t *cqueue)
{
	return (cqueue->count == 0);
}

size_t ac_cqueue_nr_items(const ac_cqueue_t *cqueue)
{
	return cqueue->count;
}

void ac_cqueue_destroy(ac_cqueue_t *cqueue)
{
	size_t i;

	if (cqueue->front < cqueue->rear) {
		for (i = cqueue->front; i < cqueue->rear; i++)
			cqueue->free_item(cqueue->queue[i]);
	} else {
		for (i = cqueue->front; i < cqueue->size; i++)
			cqueue->free_item(cqueue->queue[i]);
		for (i = 0; i < cqueue->rear; i++)
			cqueue->free_item(cqueue->queue[i]);
	}

	free(cqueue->queue);
	free(cqueue);
}
