/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_list.c - doubly linked list
 *
 * Copyright (c) 2019		Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdbool.h>

#include "include/libac.h"

/**
 * ac_list_last - find the last item in the list
 *
 * @list: The list to operate on
 *
 * Returns:
 *
 * A pointer to the last item in the list
 */
ac_list_t *ac_list_last(ac_list_t *list)
{
	while (list->next)
		list = list->next;

	return list;
}

/**
 * ac_list_len - return the number of entries in the list
 *
 * @list: The list to operate on
 *
 * Returns:
 *
 * The number of entries in the list, 0 if empty
 */
long ac_list_len(ac_list_t *list)
{
	long len = 0;

	while (list) {
		len++;
		list = list->next;
	}

	return len;
}

/**
 * ac_list_add - add an item to the end of the list
 *
 * @list: The list to add the item to
 * @data: The data to add
 */
void ac_list_add(ac_list_t **list, void *data)
{
	ac_list_t *new = malloc(sizeof(ac_list_t));

	new->data = data;
	new->next = NULL;
	new->prev = NULL;

	if (*list) {
		ac_list_t *last = ac_list_last(*list);

		last->next = new;
		new->prev = last;
	} else {
		*list = new;
	}
}

/**
 * ac_list_preadd - add an item to the front of the list
 *
 * @list: The list to add the item to
 * @data: The data to add
 *
 * This would be quicker than ac_list_add when adding multiple items,
 * so it may be better to use this function and then ac_list_reverse()
 */
void ac_list_preadd(ac_list_t **list, void *data)
{
	ac_list_t *new = malloc(sizeof(ac_list_t));

	new->data = data;
	new->prev = NULL;
	if (*list)
		new->next = *list;
	else
		new->next = NULL;

	*list = new;
}

/**
 * ac_list_remove - remove an item from the list
 *
 * @list: The list to remove the item from
 * @data: The data to be removed
 * @free_data: An optional pointer to a function to call to free the item data
 *
 * Returns:
 *
 * true if the item was found and removed, false otherwise
 */
bool ac_list_remove(ac_list_t **list, void *data,
		    void (*free_data)(void *data))
{
	ac_list_t **pp = list;
	ac_list_t *p;
	ac_list_t *prev;
	bool ret = false;

	while ((p = *pp) != NULL) {
		prev = p->prev;
		if (p->data == data) {
			*pp = p->next;
			if (*pp)
				(*pp)->prev = prev;

			if (free_data)
				free_data(p->data);
			free(p);
			ret = true;
			break;
		}
		pp = &p->next;
	}

	return ret;
}

/**
 * ac_list_remove_nth - remove the nth item from the list
 *
 * @list: The list to remove the item from
 * @n: The position of the item to be removed. Starting at 0
 * @free_data: An optional pointer to a function to call to free the item data
 *
 * Returns:
 *
 * true if the item was found and removed, false otherwise
 */
bool ac_list_remove_nth(ac_list_t **list, long n,
			void (*free_data)(void *data))
{
	ac_list_t **pp = list;
	ac_list_t *p;
	ac_list_t *prev;
	bool ret = false;
	long i = 0;

	while ((p = *pp) != NULL) {
		prev = p->prev;
		if (i++ == n) {
			*pp = p->next;
			if (*pp)
				(*pp)->prev = prev;

			if (free_data)
				free_data(p->data);
			free(p);
			ret = true;
			break;
		}
		pp = &p->next;
	}

	return ret;
}

/**
 * ac_list_remove_custom - remove an item from the list with the given data
 *
 * @list: The list to remove the item from
 * @data: The data to be removed
 * @compar: A comparison function (should return 0 when item found)
 * @free_data: An optional pointer to a function to call to free the item data
 *
 * Returns:
 *
 * true if the item was found and removed, false otherwise
 */
bool ac_list_remove_custom(ac_list_t **list, void *data,
			   int (*compar)(const void *a, const void *b),
			   void (*free_data)(void *data))
{
	ac_list_t **pp = list;
	ac_list_t *p;
	ac_list_t *prev;
	bool ret = false;

	while ((p = *pp) != NULL) {
		bool again = compar(p->data, data);

		prev = p->prev;
		if (!again) {
			*pp = p->next;
			if (*pp)
				(*pp)->prev = prev;

			if (free_data)
				free_data(p->data);
			free(p);
			ret = true;
			break;
		}
		pp = &p->next;
	}

	return ret;
}

/**
 * ac_list_reverse - reverse a list
 *
 * @list: The list to reverse
 */
void ac_list_reverse(ac_list_t **list)
{
	ac_list_t *last = NULL;

	while (*list) {
		last = *list;
		*list = last->next;
		last->next = last->prev;
		last->prev = *list;
	}

	*list = last;
}

/**
 * ac_list_find - find an item in the list
 *
 * @list: The list to look for the item in
 * @data: The data to find
 *
 * Returns:
 *
 * The list entry if found, NULL otherwise
 */
ac_list_t *ac_list_find(ac_list_t *list, const void *data)
{
	while (list) {
		if (list->data == data)
			return list;
		list = list->next;
	}

	return NULL;
}

/**
 * ac_list_find_custom - find an item in the list with the given data
 *
 * @list: The list to look for the item in
 * @data: The data to find
 * @compar: A comparison function (should return 0 when item found)
 *
 * Returns:
 *
 * The list entry if found, NULL otherwise
 */
ac_list_t *ac_list_find_custom(ac_list_t *list, const void *data,
			       int (*compar)(const void *a, const void *b))
{
	while (list) {
		bool again = compar(list->data, data);

		if (!again)
			return list;
		list = list->next;
	}

	return NULL;
}

/**
 * ac_list_nth_data - retrieve the item's data at position n
 *
 * @list: The list to look for the item in
 * @n: The position in the list, starting at 0, to retrieve
 *
 * Returns:
 *
 * The item data if it was found, NULL otherwise
 */
void *ac_list_nth_data(ac_list_t *list, long n)
{
	long i;

	if (!list)
		return NULL;

	for (i = 0; i < n; i++) {
		list = list->next;
		if (!list)
			return NULL;
	}

	return list->data;
}

/**
 * ac_list_foreach - execute a function for each item in the list
 *
 * @list: The list to operate on
 * @action: The function to execute on each item
 * @user_data: Optional data to pass to @action, can be NULL
 */
void ac_list_foreach(ac_list_t *list, void (*action)(void *item, void *data),
		     void *user_data)
{
	while (list) {
		action(list->data, user_data);
		list = list->next;
	}
}

/**
 * ac_list_rev_foreach - execute a function for each item in the list in
 *			 reverse
 *
 * @list: The list to operate on
 * @action: The function to execute on each item
 * @user_data: Optional data to pass to @action, can be NULL
 */
void ac_list_rev_foreach(ac_list_t *list,
			 void (*action)(void *item, void *data),
			 void *user_data)
{
	ac_list_t *p;

	if (!list)
		return;

	p = ac_list_last(list);
	while (p) {
		action(p->data, user_data);
		p = p->prev;
	}
}

/**
 * ac_list_destroy - destroy a list, optionally freeing all its items memory
 *
 * @list: The list to destroy
 * @free_data: Function to free an items memory, can be NULL
 */
void ac_list_destroy(ac_list_t **list, void (*free_data)(void *data))
{
	while (*list) {
		ac_list_t *p = (*list)->next;

		if (free_data)
			free_data((*list)->data);
		free(*list);
		*list = p;
	}
}
