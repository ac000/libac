1. [Overview](#overview)
2. [Functions](#functions)
  * [Binary Tree functions](#binary-tree-functions)
  * [Circular Queue functions](#circular-queue-functions)
  * [Filesystem related functions](#filesystem-related-functions)
  * [Miscellaneous functions](#miscellaneous-functions)
  * [Network related functions](#network-related-functions)
  * [String functions](#string-functions)
  * [Time related functions](#time-related-functions)
3. [License](#license)
4. [Contributing](#contributing)

## Overview

A C library of miscellaneous functions.

## Functions

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


### Circular queue functions

#### ac\_cqueue\_new

    ac_cqueue_t *ac_cqueue_new(size_t size, void (*free_item)(void *item))

#### ac\_cqueue\_push

    int ac_cqueue_push(ac_cqueue_t *cqueue, void *item)

#### ac\_cqueue\_pop

    void *ac_cqueue_pop(ac_cqueue_t *cqueue)

#### ac\_cqueue\_foreach

    void ac_cqueue_foreach(const ac_cqueue_t *cqueue,
                           void (*action)(void *item, void *data),
                           void *user_data)

#### ac\_cqueue\_is\_empty

    bool ac_cqueue_is_empty(const ac_cqueue_t *cqueue)

#### ac\_cqueue\_nr\_items

    size_t ac_cqueue_nr_items(const ac_cqueue_t *cqueue)

#### ac\_cqueue\_destroy

    void ac_cqueue_destroy(ac_cqueue_t *cqueue)


### Filesystem related functions

#### ac\_fs\_is\_posix\_name

    bool ac_fs_is_posix_name(const char *name)

#### ac\_fs\_mkdir\_p

    int ac_fs_mkdir_p(const char *path)


### Miscellaneous functions

#### ac\_misc\_ppb

    void ac_misc_ppb(u64 bytes, ac_si_units_t si, ac_misc_ppb_t *ppb)


### Network related functions

#### ac\_net\_ns\_lookup\_by\_host


    int ac_net_ns_lookup_by_host(const struct addrinfo *hints,
                                 const char *node,
                                 bool (*ac_ns_lookup_cb)
                                      (const struct addrinfo *ai,
                                       const char *res))

#### ac\_net\_ns\_lookup\_by\_ip


    int ac_net_ns_lookup_by_ip(const struct addrinfo *hints,
                               const char *node,
                               bool (*ac_ns_lookup_cb)
                                    (const struct addrinfo *ai,
                                     const char *res))


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


## Contributing

See *CodingStyle*.

Commit messages should be formatted appropriately with a suitable *subject*
line and properly formatted *mesasge body* describing why the change is
needed and what the change does.
