/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_quark.c - String to integer mapping
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>

#include "include/libac.h"

struct quark_node {
	int id;
	char *string;
};

static void null_free_quark(void *data __always_unused)
{
}

static void quark_free_node(void *data)
{
	struct quark_node *qn = data;

	if (!qn)
		return;

	free(qn->string);
	free(qn);
}

static int quark_compar(const void *pa, const void *pb)
{
	const struct quark_node *qn1 = pa;
	const struct quark_node *qn2 = pb;

	/* quark_from_string */
	if (qn1->string)
		return strcmp(qn1->string, qn2->string);

	/* quark_to_string */
	if (qn1->id < qn2->id)
		return -1;
	else if (qn1->id > qn2->id)
		return 1;
	else
		return 0;
}

/**
 * ac_quark_init - initialise a new quark
 *
 * @quark: The quark to be initialised
 * @free_func: An optional pointer to a function used to free the quark
 *             itself, not usually needed and can be NULL
 */
void ac_quark_init(ac_quark_t *quark, void(*free_func)(void *ptr))
{
	quark->mapping = ac_btree_new(quark_compar, quark_free_node);
	quark->last = -1;

	if (!free_func)
		quark->free_func = null_free_quark;
	else
		quark->free_func = free_func;
}

/**
 * ac_quark_from_string - create a new string mapping
 *
 * @quark: The quark to create the mapping in
 * @str: The string to be mapped
 *
 * Returns:
 *
 * An integer representing the string
 */
int ac_quark_from_string(ac_quark_t *quark, const char *str)
{
	struct quark_node qnl = { .id = -1, .string = (char *)str };
	struct quark_node *qn;

	qn = ac_btree_lookup(quark->mapping, &qnl);
	if (!qn) {
		qn = malloc(sizeof(struct quark_node));
		qn->string = strdup(str);
		qn->id = ++quark->last;
		ac_btree_add(quark->mapping, qn);
	}

	return qn->id;
}

/**
 * ac_quark_to_string - retrieve the given string
 *
 * @quark: The quark to retrieve the string from
 * @int: The integer representing the string to be retrieved
 *
 * Returns:
 *
 * A pointer to the string
 */
const char *ac_quark_to_string(const ac_quark_t *quark, int id)
{
	struct quark_node qnl = { .id = id, .string = NULL };
	struct quark_node *qn;

	qn = ac_btree_lookup(quark->mapping, &qnl);
	if (!qn)
		return NULL;

	return qn->string;
}

/**
 * ac_quark_destroy - destroy a quark
 *
 * @quark: The quark to be destroyed
 */
void ac_quark_destroy(ac_quark_t *quark)
{
	ac_btree_destroy(quark->mapping);
	quark->free_func(quark);
}
