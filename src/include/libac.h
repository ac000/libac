/*
 * libac.h - Miscellaneous functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#ifndef _LIBAC_H_
#define _LIBAC_H_

#include <search.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ac_btree_t {
	void *rootp;
	int (*compar)(const void *, const void *);
	void (*free_node)(void *nodep);
} ac_btree_t;

void *ac_btree_new(int (*compar)(const void *, const void *),
		   void (*free_node)(void *nodep));
void ac_btree_foreach(ac_btree_t *tree, void (*action)(const void *nodep,
					      const VISIT which,
					      const int depth));
void *ac_btree_lookup(ac_btree_t *tree, const void *key);
void *ac_btree_add(ac_btree_t *tree, const void *key);
void *ac_btree_remove(ac_btree_t *tree, const void *key);
void ac_btree_destroy(ac_btree_t *tree);

char *ac_str_chomp(char *string);
char *ac_str_substr(const char *src, void *dest, size_t dest_size, int start,
		    int len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _LIBAC_H_ */
