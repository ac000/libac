## Overview

A C library of miscellaneous functions.

### Binary Tree functions

These are a thin wrapper around the Glibc TSEARCH(3) set of binary tree
functions.

#### ac\_btree\_new

    void *ac_btree_new(int (*compar)(const void *, const void *),
                       void (*free_node)(void *nodep))

#### ac\_btree\_add

    void *ac_btree_add(ac_btree_t *tree, const void *key)

#### ac\_btree\_remove

    void *ac_btree_remove(ac_btree_t *tree, const void *key)

#### ac\_btree\_lookup

    void *ac_btree_lookup(ac_btree_t *tree, const void *key)

#### ac\_btree\_foreach

    void ac_btree_foreach(ac_btree_t *tree, void (*action)(const void *nodep,
                                                  const VISIT which,
                                                  const int depth))
#### ac\_btree\_destroy

    void ac_btree_destroy(ac_btree_t *tree)


### String functions

#### ac\_str\_chomp

    char *ac_str_chomp(char *string)

#### ac\_str\_substr

    char *ac_str_substr(const char *src, void *dest, size_t dest_size,
                        int start, int len)


### Time related functions

#### ac\_time\_tspec\_diff

    double ac_time_tspec_diff(struct timespec *delta,
                              const struct timespec *end,
                              const struct timespec *start)

### ac\_time\_secs\_to\_hms

    void ac_time_secs_to_hms(long total, int *hours, int *minutes, int *seconds)


## License

This library is licensed under the GNU Lesser General Public License (LGPL)
version 2.1

See COPYING in the repository root for details.
