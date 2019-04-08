/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_queue.c - A FIFO queue of arbitary size
 *
 * Copyright (c) 2017, 2019	Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stdlib.h>

#include "include/libac.h"

/**
 * ac_queue_new - create a new queue
 *
 * Returns:
 *
 * A pointer to the newly created queue
 */
ac_queue_t *ac_queue_new(void)
{
	ac_queue_t *queue;

	queue = malloc(sizeof(ac_queue_t));

	queue->queue = NULL;
	queue->tail = NULL;
	queue->items = 0;

	return queue;
}

/**
 * ac_queue_push - add an item to the queue
 *
 * @queue: The queue to add the item to
 * @item: The item to be added
 *
 * Returns:
 *
 * -1 on failure or 0 on success
 */
int ac_queue_push(ac_queue_t *queue, void *item)
{
	ac_slist_t *new;

	if (!queue)
		return -1;

	new = malloc(sizeof(ac_slist_t));
	new->data = item;
	new->next = NULL;

	if (queue->queue)
		queue->tail->next = new;
	else
		queue->queue = new;

	/* Track the end of the list to speed up appending items */
	queue->tail = new;

	queue->items++;

	return 0;
}

/**
 * ac_queue_pop - get the head element from the queue
 *
 * @queue: The queue to get the element from
 *
 * Returns:
 *
 * A pointer to element or NULL for none
 */
void *ac_queue_pop(ac_queue_t *queue)
{
	ac_slist_t **list;
	ac_slist_t *p;
	void *item;

	if (!queue)
		return NULL;

	list = &queue->queue;
	p = *list;
	if (!p)
		return NULL;

	*list = p->next;
	item = p->data;
	free(p);
	queue->items--;

	return item;
}

/**
 * ac_queue_nr_items - get the number of elements in the queue
 *
 * @queue: The queue to operate on
 *
 * Returns:
 *
 * The number of elements in the queue
 */
u32 ac_queue_nr_items(const ac_queue_t *queue)
{
	return !queue ? 0 : queue->items;
}

/**
 * ac_queue_foreach - iterate over elements in a queue
 *
 * @queue: The queue to iterate over
 * @action: The function to call on each element
 * @user_data: Optional user data to pass to action. Can be NULL
 */
void ac_queue_foreach(const ac_queue_t *queue,
		      void (*action)(void *item, void *data), void *user_data)
{
	if (!queue || queue->items == 0)
		return;

	ac_slist_foreach(queue->queue, action, user_data);
}

/**
 * ac_queue_destroy - destroy a queue freeing all its memory
 *
 * @queue: The queue to destroy
 * @free_func: A function to free an element of the queue or NULL for none
 */
void ac_queue_destroy(ac_queue_t *queue, void (*free_func)(void *item))
{
	if (!queue)
		return;

	ac_slist_destroy(&queue->queue, free_func);
	free(queue);
}
