1. [Overview](#overview)
2. [Types, defines, etc](#types-defines-etc)
  * [Library version](#library-version)
  * [Types](#types)
  * [ac\_geo\_ellipsoid\_t](#ac_geo_ellipsoid_t)
  * [ac\_hash\_algo\_t](#ac_hash_algo_t)
  * [ac\_misc\_ppb\_factor\_t](#ac_misc_ppb_factor_t)
  * [ac\_si\_units\_t](#ac_si_units_t)
  * [misc](#misc)
3. [Functions](#functions)
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
4. [Build it](#build-it)
5. [How to use](#how-to-use)
6. [License](#license)
7. [Contributing](#contributing)

## Overview

A C library of miscellaneous functions that has no dependencies other than
Glibc.


## Types, defines, etc

### Library version

    #define LIBAC_MAJOR_VERSION
    #define LIBAC_MINOR_VERSION
    #define LIBAC_MICRO_VERSION

### Types

    typedef uint64_t u64
    typedef int64_t  s64
    typedef uint32_t u32
    typedef int32_t  s32
    typedef uint16_t u16
    typedef int16_t  s16
    typedef uint8_t   u8
    typedef int8_t    s8

    typedef struct crypt_data ac_crypt_data_t

### ac\_geo\_ellipsoid\_t

    AC_GEO_EREF_WGS84
    AC_GEO_EREF_GRS80
    AC_GEO_EREF_AIRY1830

### ac\_hash\_algo\_t

    AC_HASH_ALGO_MD5
    AC_HASH_ALGO_SHA1
    AC_HASH_ALGO_SHA256
    AC_HASH_ALGO_SHA512

### ac\_misc\_ppb\_factor\_t

    AC_MISC_PPB_BYTES
    AC_MISC_PPB_KBYTES
    AC_MISC_PPB_MBYTES
    AC_MISC_PPB_GBYTES
    AC_MISC_PPB_TBYTES
    AC_MISC_PPB_PBYTES
    AC_MISC_PPB_EBYTES

### ac\_si\_units\_t

    AC_SI_UNITS_NO
    AC_SI_UNITS_YES

### misc

    #define __unused
    #define __maybe_unused
    #define __always_unused

These are aliases for \_\_attribute\_\_((unused))

    #define AC_FS_COPY_OVERWRITE


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

#### ac\_misc\_passcrypt

    char *ac_misc_passcrypt(const char *pass, ac_hash_algo_t hash_type,
                            ac_crypt_data_t *data)


### Network related functions

#### ac\_net\_port\_from\_sa - extract the port number from a struct sockaddr

    u16 ac_net_port_from_sa(const struct sockaddr *sa)

#### ac\_net\_inet\_pton - address family agnostic wrapper around inet\_pton(3)

    int ac_net_inet_pton(const char *src, void *dst)

#### ac\_net\_inet\_ntop - address family agnostic wrapper around inet\_ntop(3)

    const char *ac_net_inet_ntop(const void *src, char *dst, socklen_t size)

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

#### ac\_slist\_len - return the number of entries in the list

    long ac_slist_len(ac_slist_t *list)

#### ac\_slist\_add - add an item to the end of the list

    void ac_slist_add(ac_slist_t **list, void *data)

#### ac\_slist\_preadd - add an item to the front of the list

    void ac_slist_preadd(ac_slist_t **list, void *data)

#### ac\_slist\_remove - remove an item from the list

    bool ac_slist_remove(ac_slist_t **list, void *data, void (*free_data)
                                                             (void *data))

#### ac\_slist\_remove\_nth - remove the nth item from the list

    bool ac_slist_remove_nth(ac_slist_t **list, int n, void (*free_data)
                                                            (void *data))

#### ac\_slist\_reverse - reverse a list

    void ac_slist_reverse(ac_slist_t **list)

#### ac\_slist\_find - find an item in the list

    ac_slist_t *ac_slist_find(ac_slist_t *list, void *data)

#### ac\_slist\_nth\_data - retrieve the item's data at position n

    void *ac_slist_nth_data(ac_slist_t *list, int n)

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

e.g when updating to a new version

    $ git archive --format=tar --prefix=libac-0.16.0/ -o ~/rpmbuild/SOURCES/libac-0.16.0.tar HEAD
    $ cp libac.spec ~/rpmbuild/SPECS/ # Unless you have it symlinked, I do
    $ rpmbuild -bb ~/rpmbuild/SPECS/libac.spec
    $ sudo dnf update /home/andrew/rpmbuild/RPMS/x86_64/libac-0.16.0-1.fc24.x86_64.rpm /home/andrew/rpmbuild/RPMS/x86_64/libac-debuginfo-0.16.0-1.fc24.x86_64.rpm

You can create a suitable *rpmbuild/* directory structure with

    $ mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

*rpmbuild* can be found in the *rpm-build* package.


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
