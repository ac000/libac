1. [Overview](#overview)
2. [Functions](#functions)
  * [Binary Tree functions](#binary-tree-functions)
  * [Circular Queue functions](#circular-queue-functions)
  * [Filesystem related functions](#filesystem-related-functions)
  * [Geospatial related functions](#geospatial-related-functions)
  * [Miscellaneous functions](#miscellaneous-functions)
  * [Network related functions](#network-related-functions)
  * [Quark (string to integer mapping) functions](#quark-functions)
  * [Singly linked list functions](#singly-linked-list-functions)
  * [String functions](#string-functions)
  * [Time related functions](#time-related-functions)
3. [Build it](#build-it)
4. [How to use](#how-to-use)
5. [License](#license)
6. [Contributing](#contributing)

## Overview

A C library of miscellaneous functions that has no dependencies other than
Glibc.


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

#### ac\_fs\_copy - copy a file

    ssize_t ac_fs_copy(const char *from, const char *to, int flags)


### Geospatial related functions

#### ac\_geo\_dd\_to\_dms - convert decimal degrees into degrees, minutes & seconds

    void ac_geo_dd_to_dms(double degrees, ac_geo_dms_t *dms)

#### ac\_geo\_dms\_to\_dd - convert degrees, minutes & seconds into decimal degrees

    double ac_geo_dms_to_dd(const ac_geo_dms_t *dms)

#### ac\_geo\_haversine - calculate the distance between two points on Earth

    double ac_geo_haversine(const ac_geo_t *from, const ac_geo_t *to)

#### ac\_geo\_vincenty\_direct - given an initial point, bearing and distance calculate the end point

    void ac_geo_vincenty_direct(const ac_geo_t *from, ac_geo_t *to,
                                double distance)

#### ac\_geo\_bng\_to\_lat\_lon - convert British National grid Eastings & Northings to latitude & longitude decimal degrees

    void ac_geo_bng_to_lat_lon(ac_geo_t *geo)

#### ac\_geo\_lat\_lon\_to\_bng - convert latitude & longitude decimal degrees to British National Grid Eastings & Northings

    void ac_geo_lat_lon_to_bng(ac_geo_t *geo)


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


### Quark functions

#### ac\_quark\_init - initialise a new quark

    void ac_quark_init(ac_quark_t *quark, void(*free_func)(void *ptr))

#### ac\_quark\_from\_string - create a new string mapping

    int ac_quark_from_string(ac_quark_t *quark, const char *str)

#### ac\_quark\_to\_string - retrieve the given string

    const char *ac_quark_to_string(ac_quark_t *quark, int id)

#### ac\_quark\_destroy - destroy a quark

    void ac_quark_destroy(ac_quark_t *quark)


### Singly linked list functions

#### ac\_slist\_add - add an item to the end of the list

    void ac_slist_add(ac_slist_t **list, void *data)

#### ac\_slist\_preadd - add an item to the front of the list

    void ac_slist_preadd(ac_slist_t **list, void *data)

#### ac\_slist\_remove - remove an item from the list

    bool ac_slist_remove(ac_slist_t **list, void *data, void (*free_data)
                                                             (void *data))

#### ac\_slist\_reverse - reverse a list

    void ac_slist_reverse(ac_slist_t **list)

#### ac\_slist\_find - find an item in the list

    ac_slist_t *ac_slist_find(ac_slist_t *list, void *data)

#### ac\_slist\_foreach - execute a function for each item in the list

    void ac_slist_foreach(ac_slist_t *list,
                          void (*action)(void *item, void *data),
                          void *user_data)

#### ac\_slist\_destroy - destroy a list, optionally freeing all its items memory

    void ac_slist_destroy(ac_slist_t **list, void (*free_data)(void *data))


### String functions

#### ac\_str\_chomp - remove trailing white space from a string

    char *ac_str_chomp(char *string)

#### ac\_str\_substr - extract a substring from a string

    char *ac_str_substr(const char *src, void *dest, size_t dest_size,
                        int start, int len)

#### ac\_str\_levenshtein - calculate the Levenshtein distance between two strings

    int ac_str_levenshtein(const char *s, const char *t)


### Time related functions

#### ac\_time\_tspec\_diff - subtract two struct timespec's

    double ac_time_tspec_diff(struct timespec *delta,
                              const struct timespec *end,
                              const struct timespec *start)

#### ac\_time\_secs\_to\_hms - convert total seconds to broken out hrs, mins & secs

    void ac_time_secs_to_hms(long total, int *hours, int *minutes, int *seconds)


## Build it

The simplest way is to build the rpm, something like

    $ cp libac.spec ~/rpmbuild/SPECS
    $ git archive --format=tar --prefix=libac-<VERSION>/ -o ~/rpmbuild/SOURCES/libac-<VERSION>.tar HEAD
    $ rpmbuild -bb ~/rpmbuild/SPECS/libac.spec
    $ sudo dnf install ~/rpmbuild/RPMS/x86_64/libac-<VERSION>-?.<DIST>.x86_64.rpm


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

Patches and/or pull requests should be sent to:
Andrew Clayton <andrew@digital-domain.net>. Or via GitHub.

See: *git-format-patch(1)*, *git-send-email(1)* and *git-request-pull(1)* for
good ways to do this via email.

Commit messages should be formatted appropriately with a suitable *subject*
line and properly formatted *mesasge body* describing why the change is
needed and what the change does.

They should also be *Signed-off*, this means adding a line like

    Signed-off-by: Name <email>

at the end of the commit message (after a blank line). See
<https://developercertificate.org/>
