/*
 * ac_btree.c - Binary tree functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#define _GNU_SOURCE		/* tdestroy(3) */

#include <stdlib.h>
#include <search.h>

#include "include/libac.h"

static void null_free_node(void *data)
{
}

void ac_btree_destroy(ac_btree_t *tree)
{
	if (!tree)
		return;

	tdestroy(tree->rootp, tree->free_node);

	free(tree);
}

void *ac_btree_new(int (*compar)(const void *, const void *),
		   void (*free_node)(void *nodep))
{
	ac_btree_t *tree = malloc(sizeof(struct ac_btree_t));

	tree->rootp = NULL;
	tree->compar = compar;

	if (!free_node)
		tree->free_node = null_free_node;
	else
		tree->free_node = free_node;

	return tree;
}

void ac_btree_foreach(ac_btree_t *tree, void (*action)(const void *nodep,
					      const VISIT which,
					      const int depth))
{
	twalk(tree->rootp, action);
}

void *ac_btree_lookup(ac_btree_t *tree, const void *key)
{
	return *(void **)tfind(key, &tree->rootp, tree->compar);
}

void *ac_btree_add(ac_btree_t *tree, const void *key)
{
	return *(void **)tsearch(key, &tree->rootp, tree->compar);
}

void *ac_btree_remove(ac_btree_t *tree, const void *key)
{
	void *node = ac_btree_lookup(tree, key);
	void *pnode = tdelete(key, &tree->rootp, tree->compar);

	tree->free_node(node);

	return *(void **)pnode;
}
