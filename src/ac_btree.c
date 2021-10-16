/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_btree.c - Binary search tree functions
 *
 * Copyright (c) 2017, 2019 - 2021	Andrew Clayton
 *					<andrew@digital-domain.net>
 */

#define _GNU_SOURCE		/* tdestroy(3) */

#include <stdlib.h>
#include <search.h>

#include "include/libac.h"
#include "platform.h"

static void null_free_node(void *data __always_unused)
{
}

/**
 * ac_btree_destroy - destroy a binary tree freeing all memory
 *
 * @tree: The binary tree to be destroyed
 */
void ac_btree_destroy(const ac_btree_t *tree)
{
	if (!tree)
		return;

	tdestroy(tree->rootp, tree->free_node);

	free((void *)tree);
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
	ac_btree_t *tree = malloc(sizeof(ac_btree_t));

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
void ac_btree_foreach(const ac_btree_t *tree,
		      void (*action)(const void *nodep, VISIT which,
				     int depth))
{
	twalk(tree->rootp, action);
}

/**
 * ac_btree_foreach_data - iterate over the tree with user_data
 *
 * @tree: The binary tree to operate on
 * @action: Function to be called for each node
 * @user_data: Optional user data argument passed to action() as closure
 */
void ac_btree_foreach_data(const ac_btree_t *tree,
			   void (*action)(const void *nodep, VISIT which,
					  void *data),
			   void *user_data)
{
	twalk_r(tree->rootp, action, user_data);
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
void *ac_btree_lookup(const ac_btree_t *tree, const void *key)
{
	void *node = tfind(key, &tree->rootp, tree->compar);

	if (!node)
		return NULL;

	return *(void **)node;
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
 * wasn't found or if the last node was removed
 *
 * You can make a call to ac_btree_is_empty() to find out which of the
 * above
 */
void *ac_btree_remove(ac_btree_t *tree, const void *key)
{
	void *node = ac_btree_lookup(tree, key);
	void *pnode;

	if (!node)
		return NULL;

	pnode = tdelete(key, &tree->rootp, tree->compar);
	tree->free_node(node);
	if (!pnode || !tree->rootp)
		return NULL;

	return *(void **)pnode;
}

/**
 * ac_btree_is_empty - test if the binary tree is empty
 *
 * @tree: The tree to check
 *
 * Returns:
 *
 * true if the tree is empty, false otherwise
 */
bool ac_btree_is_empty(const ac_btree_t *tree)
{
	return !tree->rootp;
}
