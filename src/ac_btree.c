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

/**
 * ac_btree_destroy - destroy a binary tree freeing all memory
 *
 * @tree: The binary tree to be destroyed
 */
void ac_btree_destroy(ac_btree_t *tree)
{
	if (!tree)
		return;

	tdestroy(tree->rootp, tree->free_node);

	free(tree);
}

/**
 * ac_btree_new - create a new binary tree
 *
 * @compar: A comparison function. Function should return an integer less
 *          than, equal to or greater than zero if the first argument is
 *          considered to be respectively less than, equal to or greater
 *          than the second
 * @free_node: Pointer to function called to free a nodes memory. Can be NULL
 */
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

/**
 * ac_btree_foreach - iterate over the tree
 *
 * @tree: The binary tree to operate on
 * @action: Function to be called for each node
 */
void ac_btree_foreach(ac_btree_t *tree, void (*action)(const void *nodep,
					      const VISIT which,
					      const int depth))
{
	twalk(tree->rootp, action);
}

/**
 * ac_btree_lookup - lookup a node in the tree
 *
 * @tree: The tree to do the lookup on
 * @key: The item to be matched
 *
 * Returns:
 *
 * A pointer to the node if matched or NULL if not
 */
void *ac_btree_lookup(ac_btree_t *tree, const void *key)
{
	return *(void **)tfind(key, &tree->rootp, tree->compar);
}

/**
 * ac_btree_add - add a node to the tree
 *
 * @tree: The tree to add the node to
 * @key: The item to be added
 *
 * Returns:
 *
 * A pointer to the newly added node or a pointer to the node if it already
 * exists.
 */
void *ac_btree_add(ac_btree_t *tree, const void *key)
{
	return *(void **)tsearch(key, &tree->rootp, tree->compar);
}

/**
 * ac_btree_remove - remove a node from the tree
 *
 * @tree: The tree to remove the node from
 * @key: The item to be removed
 *
 * Returns:
 *
 * A pointer to the parent node of the removed item or NULL if the node
 * wasn't found
 */
void *ac_btree_remove(ac_btree_t *tree, const void *key)
{
	void *node = ac_btree_lookup(tree, key);
	void *pnode = tdelete(key, &tree->rootp, tree->compar);

	tree->free_node(node);

	return *(void **)pnode;
}
