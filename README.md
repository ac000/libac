1. [Overview](#overview)
2. [Functions](#functions)
  * [Binary Tree functions](#binary-tree-functions)
  * [Circular Queue functions](#circular-queue-functions)
  * [Filesystem related functions](#filesystem-related-functions)
  * [Miscellaneous functions](#miscellaneous-functions)
  * [Network related functions](#network-related-functions)
  * [String functions](#string-functions)
  * [Time related functions](#time-related-functions)
3. [How to use](#how-to-use)
4. [License](#license)
5. [Contributing](#contributing)

## Overview

A C library of miscellaneous functions.

## Functions

### Binary Tree functions

These are a thin wrapper around the Glibc TSEARCH(3) set of binary tree
functions.

#### ac\_btree\_new - create a new binary tree

    void *ac_btree_new(int (*compar)(const void *, const void *),
                       void (*free_node)(void *nodep))

#### ac\_btree\_add - add a node to the tree

    void *ac_btree_add(ac_btree_t *tree, const void *key)

#### ac\_btree\_remove - remove a node from the tree

    void *ac_btree_remove(ac_btree_t *tree, const void *key)

#### ac\_btree\_lookup - lookup a node in the tree

    void *ac_btree_lookup(ac_btree_t *tree, const void *key)

#### ac\_btree\_foreach - iterate over the tree

    void ac_btree_foreach(ac_btree_t *tree, void (*action)(const void *nodep,
                                                  const VISIT which,
                                                  const int depth))

#### ac\_btree\_destroy - destroy a binary tree freeing all memory

    void ac_btree_destroy(ac_btree_t *tree)


### Circular queue functions

#### ac\_cqueue\_new - create a new circular queue

    ac_cqueue_t *ac_cqueue_new(size_t size, void (*free_item)(void *item))

#### ac\_cqueue\_push - add an item to the queue

    int ac_cqueue_push(ac_cqueue_t *cqueue, void *item)

#### ac\_cqueue\_pop - get the head element from the queue

    void *ac_cqueue_pop(ac_cqueue_t *cqueue)

#### ac\_cqueue\_foreach - iterate over elements in a queue

    void ac_cqueue_foreach(const ac_cqueue_t *cqueue,
                           void (*action)(void *item, void *data),
                           void *user_data)

#### ac\_cqueue\_is\_empty - check if a queue is empty

    bool ac_cqueue_is_empty(const ac_cqueue_t *cqueue)

#### ac\_cqueue\_nr\_items - get the number of elements in the queue

    size_t ac_cqueue_nr_items(const ac_cqueue_t *cqueue)

#### ac\_cqueue\_destroy - destroy a circular queue freeing all its memory

    void ac_cqueue_destroy(ac_cqueue_t *cqueue)


### Filesystem related functions

#### ac\_fs\_is\_posix\_name - checks if a filename follows POSIX guidelines

    bool ac_fs_is_posix_name(const char *name)

#### ac\_fs\_mkdir\_p - mkdir with parents

    int ac_fs_mkdir_p(const char *path)


### Miscellaneous functions

#### ac\_misc\_ppb - pretty print bytes

    void ac_misc_ppb(u64 bytes, ac_si_units_t si, ac_misc_ppb_t *ppb)


### Network related functions

#### ac\_net\_ns\_lookup\_by\_host - lookup a host by hostname (get its IP(s))

    int ac_net_ns_lookup_by_host(const struct addrinfo *hints,
                                 const char *node,
                                 bool (*ac_ns_lookup_cb)
                                      (const struct addrinfo *ai,
                                       const char *res))

#### ac\_net\_ns\_lookup\_by\_ip - lookup a host by IP address (get its hostname)

    int ac_net_ns_lookup_by_ip(const struct addrinfo *hints,
                               const char *node,
                               bool (*ac_ns_lookup_cb)
                                    (const struct addrinfo *ai,
                                     const char *res))


### String functions

#### ac\_str\_chomp - remove trailing white space from a string

    char *ac_str_chomp(char *string)

#### ac\_str\_substr - extract a substring from a string

    char *ac_str_substr(const char *src, void *dest, size_t dest_size,
                        int start, int len)


### Time related functions

#### ac\_time\_tspec\_diff - subtract two struct timespec's

    double ac_time_tspec_diff(struct timespec *delta,
                              const struct timespec *end,
                              const struct timespec *start)

### ac\_time\_secs\_to\_hms - convert total seconds to broken out hrs, mins & secs

    void ac_time_secs_to_hms(long total, int *hours, int *minutes, int *seconds)


## How to use

Just

    #include <libac.h>

in your program and link with *-lac* assuming you've built and installed the
RPM or similar.

See *src/test.c* for examples on using the above.


## License

This library is licensed under the GNU Lesser General Public License (LGPL)
version 2.1

See COPYING in the repository root for details.


## Contributing

See *CodingStyle*.

Commit messages should be formatted appropriately with a suitable *subject*
line and properly formatted *mesasge body* describing why the change is
needed and what the change does.
