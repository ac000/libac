/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_htable.c - Hash Table
 *
 * Copyright (c) 2017, 2020	Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stdlib.h>

#include "include/libac.h"

#define HTABLE_SZ	2048

struct bucket_list_elem {
	void *key;
	void *data;
};

static ac_slist_t *bucket_list_lookup(const ac_htable_t *htable,
				      ac_slist_t *list, const void *key)
{
	while (list) {
		const struct bucket_list_elem *elem = list->data;
		bool again = htable->key_cmp(elem->key, key);

		if (!again)
			return list;
		list = list->next;
	}

	return NULL;
}

static bool bucket_list_remove(const ac_htable_t *htable, ac_slist_t **list,
			       void *key)
{
	ac_slist_t **pp = list;
	ac_slist_t *p;
	bool ret = false;

	while ((p = *pp) != NULL) {
		struct bucket_list_elem *elem = p->data;
		bool again = htable->key_cmp(elem->key, key);

		if (!again) {
			*pp = p->next;

			if (htable->free_key_func)
				htable->free_key_func(elem->key);
			if (htable->free_data_func)
				htable->free_data_func(elem->data);
			free(elem);
			free(p);

			ret = true;
			break;
		}
		pp = &p->next;
	}

	return ret;
}

/**
 * ac_htable_new - create a new hash table
 *
 * @hash_func: Pointer to a hashing function
 * @key_cmp: Pointer to a key comparison function
 * @free_key_func: Optional pointer to a key free'ing function
 * @free_data_func: Optional pointer to a data free'ing function
 *
 * Returns:
 *
 * A pointer to a newly created hash table. Should be free'd with
 * ac_htable_destroy()
 */
ac_htable_t *ac_htable_new(u32 (*hash_func)(const void *key),
			   int (*key_cmp)(const void *a, const void *b),
			   void (*free_key_func)(void *key),
			   void (*free_data_func)(void *data))
{
	ac_htable_t *htable;

	htable = malloc(sizeof(ac_htable_t));
	htable->buckets = calloc(HTABLE_SZ, sizeof(ac_slist_t *));
	htable->hash_func = hash_func;
	htable->key_cmp = key_cmp;
	htable->free_key_func = free_key_func;
	htable->free_data_func = free_data_func;
	htable->count = 0;

	return htable;
}

/**
 * ac_htable_insert - inserts a new entry into a hash table
 *
 * @htable: The hash table to insert into
 * @key: The key to use
 * @data: The data to sore
 *
 * If you try and insert with an already existing key, the old entry will
 * be removed/free'd first
 */
void ac_htable_insert(ac_htable_t *htable, void *key, void *data)
{
	struct bucket_list_elem *ble = malloc(sizeof(struct bucket_list_elem));
	u32 bucket = htable->hash_func(key) % HTABLE_SZ;
	ac_slist_t *item;

	ble->key = key;
	ble->data = data;

	item = bucket_list_lookup(htable, htable->buckets[bucket], key);
	if (item) {
		bucket_list_remove(htable, &htable->buckets[bucket], key);
		htable->count--;
	}

	ac_slist_preadd(&htable->buckets[bucket], ble);
	htable->count++;
}

/**
 * ac_htable_remove - remove an entry from a hash table
 *
 * @htable: The hash table to remove from
 * @key: The key to use
 *
 * Returns:
 *
 * true if the entry was removed, false otherwise
 */
bool ac_htable_remove(ac_htable_t *htable, void *key)
{
	u32 bucket = htable->hash_func(key) % HTABLE_SZ;
	bool ret;

	ret = bucket_list_remove(htable, &htable->buckets[bucket], key);
	if (ret)
		htable->count--;

	return ret;
}

/**
 * ac_htable_lookup - lookup an entry in a hash table
 *
 * @htable: The hash table to lookup from
 * @key: The key to use
 *
 * Returns:
 *
 * A pointer to the enties data if found, NULL if not
 */
void *ac_htable_lookup(const ac_htable_t *htable, const void *key)
{
	u32 bucket = htable->hash_func(key) % HTABLE_SZ;
	ac_slist_t *item = bucket_list_lookup(htable, htable->buckets[bucket],
					      key);

	if (!item)
		return NULL;

	return ((struct bucket_list_elem *)item->data)->data;
}

/**
 * ac_htable_foreach - iterate over each entry in a hash table
 *
 * @htable: The hash table to iterate over
 * @action: A pointer to a function to call for each entry. This will get the
 *          key, data and optional user supplied data as arguments
 * @user_data: Optional pointer to data to pass to the above function
 */
void ac_htable_foreach(const ac_htable_t *htable,
		       void (*action)(void *key, void *value, void *user_data),
		       void *user_data)
{
	u32 bucket;

	for (bucket = 0; bucket < HTABLE_SZ; bucket++) {
		ac_slist_t *list = htable->buckets[bucket];

		while (list) {
			struct bucket_list_elem *elem = list->data;

			action(elem->key, elem->data, user_data);
			list = list->next;
		}
	}
}

/**
 * ac_htable_destroy - destroy the given hash table
 *
 * @htable: The hash table to destroy/free
 */
void ac_htable_destroy(const ac_htable_t *htable)
{
	u32 bucket;

	for (bucket = 0; bucket < HTABLE_SZ; bucket++) {
		ac_slist_t *list = htable->buckets[bucket];

		while (list) {
			struct bucket_list_elem *elem = list->data;
			ac_slist_t *p = list->next;

			if (htable->free_key_func)
				htable->free_key_func(elem->key);
			if (htable->free_data_func)
				htable->free_data_func(elem->data);
			free(elem);
			free(list);
			list = p;
		}
	}

	free(htable->buckets);
	free((void *)htable);
}
