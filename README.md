# libac

[![Builds](https://github.com/ac000/libac/actions/workflows/build_tests.yaml/badge.svg)](https://github.com/ac000/libac/actions/workflows/build_tests.yaml)
[![FreeBSD Build Status](https://api.cirrus-ci.com/github/ac000/libac.svg)](https://cirrus-ci.com/github/ac000/libac)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/ac000/libac.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/ac000/libac/context:cpp)

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
  * [Binary Search Tree functions](#binary-search-tree-functions)
  * [Circular Buffer functions](#circular-buffer-functions)
  * [Filesystem related functions](#filesystem-related-functions)
  * [Geospatial related functions](#geospatial-related-functions)
  * [Hash Table functions](#hash-table-functions)
  * [JSON functions](#json-functions)
  * [JOSN Writer functions](#json-writer-functions)
  * [Miscellaneous functions](#miscellaneous-functions)
  * [Network related functions](#network-related-functions)
  * [Quark (string to integer mapping) functions](#quark-functions)
  * [Queue functions](#queue-functions)
  * [Doubly linked list functions](doubly-linked-list-functions)
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

### ac\_misc\_shuffle\_t

    AC_MISC_SHUFFLE_FISHER_YATES

### ac\_si\_units\_t

    AC_SI_UNITS_NO
    AC_SI_UNITS_YES

### Macros

    AC_BYTE_BIT_SET(byte, bit)
    AC_BYTE_BIT_CLR(byte, bit)
    AC_BYTE_BIT_FLP(byte, bit)
    AC_BYTE_BIT_TST(byte, bit)
    AC_BYTE_NIBBLE_HIGH(byte)
    AC_BYTE_NIBBLE_LOW(byte)

    AC_LONG_TO_PTR(x)
    AC_PTR_TO_LONG(p)

    AC_MIN(a, b)
    AC_MAX(a, b)

    AC_ARRAY_SIZE(a)

### misc

    #define __unused
    #define __maybe_unused
    #define __always_unused

These are aliases for \_\_attribute\_\_((unused))

    #define AC_FS_COPY_OVERWRITE

    #define AC_UUID4_LEN	36

    #define AC_STR_SPLIT_ALWAYS
    #define AC_STR_SPLIT_STRICT

    #define AC_TIME_NS_SEC
    #define AC_TIME_NS_MSEC
    #define AC_TIME_NS_USEC


## Functions

### Binary Search Tree functions

These are a thin wrapper around the Glibc TSEARCH(3) set of binary tree
functions.

#### ac\_btree\_new - create a new binary tree

    void *ac_btree_new(int (*compar)(const void *, const void *),
                       void (*free_node)(void *nodep)):

#### ac\_btree\_add - add a node to the tree

    void *ac_btree_add(ac_btree_t *tree, const void *key);

#### ac\_btree\_remove - remove a node from the tree

    void *ac_btree_remove(ac_btree_t *tree, const void *key);

#### ac\_btree\_lookup - lookup a node in the tree

    void *ac_btree_lookup(const ac_btree_t *tree, const void *key);

#### ac\_btree\_foreach - iterate over the tree

    void ac_btree_foreach(const ac_btree_t *tree,
                          void (*action)(const void *nodep, VISIT which,
                                         int depth));

#### ac\_btree\_foreach\_data - iterate over the tree with user\_data

    #if __FreeBSD__ || (__GLIBC__ >= 2 && __GLIBC_MINOR__ >= 30)
    void ac_btree_foreach(const ac_btree_t *tree,
                          void (*action)(const void *nodep, VISIT which,
                                         void *data),
                          void *user_data);
    #endif

#### ac\_btree\_is\_empty - test if the binary tree is empty

    bool ac_btree_is_empty(const ac_btree_t *tree);

#### ac\_btree\_destroy - destroy a binary tree freeing all memory

    void ac_btree_destroy(const ac_btree_t *tree);


### Circular Buffer functions

#### ac\_circ\_buf\_new - create a new circular buffer (size must be power of 2)

    ac_circ_buf_t *ac_circ_buf_new(u32 size, u32 elem_sz);

#### ac\_circ\_buf\_count - how many items are in the buffer

    u32 ac_circ_buf_count(const ac_circ_buf_t *cbuf);

#### ac\_circ\_buf\_pushm - push multiple items into the buffer

    int ac_circ_buf_pushm(ac_circ_buf_t *cbuf, void *buf, u32 count);

#### ac\_circ\_buf\_push - push an item into the buffer

    int ac_circ_buf_push(ac_circ_buf_t *cbuf, void *buf);

#### ac\_circ\_buf\_popm - pop multiple items from buffer

    int ac_circ_buf_popm(ac_circ_buf_t *cbuf, void *buf, u32 count);

#### ac\_circ\_buf\_pop - pop an item from the buffer

    void *ac_circ_buf_pop(ac_circ_buf_t *cbuf);

#### ac\_circ\_buf\_foreach - iterate over elements in the circular buffer

    void ac_circ_buf_foreach(const ac_circ_buf_t *cbuf,
                             void (*action)(void *item, void *data),
                             void *user_data);

#### ac\_circ\_buf\_reset - reset the circular buffer to empty

    void ac_circ_buf_reset(ac_circ_buf_t *cbuf);

#### ac\_circ\_buf\_destroy - destroy a circular buffer

    void ac_circ_buf_destroy(const ac_circ_buf_t *cbuf);


### Filesystem related functions

#### ac\_fs\_is\_posix\_name - checks if a filename follows POSIX guidelines

    bool ac_fs_is_posix_name(const char *name);

#### ac\_fs\_mkdir\_p - mkdir with parents

    int ac_fs_mkdir_p(int dirfd, const char *path, mode_t mode);

#### ac\_fs\_copy - copy a file

    ssize_t ac_fs_copy(const char *from, const char *to, int flags);


### Geospatial related functions

#### ac\_geo\_dd\_to\_dms - convert decimal degrees into degrees, minutes & seconds

    void ac_geo_dd_to_dms(double degrees, ac_geo_dms_t *dms);

#### ac\_geo\_dms\_to\_dd - convert degrees, minutes & seconds into decimal degrees

    double ac_geo_dms_to_dd(const ac_geo_dms_t *dms);

#### ac\_geo\_haversine - calculate the distance between two points on Earth

    double ac_geo_haversine(const ac_geo_t *from, const ac_geo_t *to);

#### ac\_geo\_vincenty\_direct - given an initial point, bearing and distance calculate the end point

    void ac_geo_vincenty_direct(const ac_geo_t *from, ac_geo_t *to,
                                double distance);

#### ac\_geo\_bng\_to\_lat\_lon - convert British National grid Eastings & Northings to latitude & longitude decimal degrees

    void ac_geo_bng_to_lat_lon(ac_geo_t *geo);

#### ac\_geo\_lat\_lon\_to\_bng - convert latitude & longitude decimal degrees to British National Grid Eastings & Northings

    void ac_geo_lat_lon_to_bng(ac_geo_t *geo);


### Hash Table functions

#### ac\_htable\_new - create a new hash table

    ac_htable_t *ac_htable_new(u32 (*hash_func)(const void *key),
                               int (*key_cmp)(const void *a, const void *b),
                               void (*free_key_func)(void *key),
                               void (*free_data_func)(void *data));

#### ac\_htable\_insert - inserts a new entry into a hash table

    void ac_htable_insert(ac_htable_t *htable, void *key, void *data);

#### ac\_htable\_remove - remove an entry from a hash table

    bool ac_htable_remove(ac_htable_t *htable, const void *key);

#### ac\_htable\_lookup - lookup an entry in a hash table

    void *ac_htable_lookup(const ac_htable_t *htable, const void *key);

#### ac\_htable\_foreach - iterate over each entry in a hash table

    void ac_htable_foreach(const ac_htable_t *htable,
                           void (*action)(void *key, void *value,
                                          void *user_data), void *user_data);

#### ac\_htable\_destroy - destroy the given hash table

    void ac_htable_destroy(const ac_htable_t *htable);


### JSON functions

#### ac\_json\_load\_from\_fd - loads json from an open file descriptor

    char *ac_json_load_from_fd(int fd, off_t offset);

#### ac\_json\_load\_from\_file - loads json from the specified file

    char *ac_json_load_from_file(const char *file, off_t offset);


### JSON Writer functions

#### ac\_jsonw\_init - initialises a new ac\_jsonw\_t object

    ac_jsonw_t *ac_jsonw_init(void);

#### void ac\_jsonw\_indent\_sz - set the JSON indentation size

    void ac_jsonw_indent_sz(ac_jsonw_t *json, int size);

#### ac\_jsonw\_set\_indenter - set the indentation character/string

    void ac_jsonw_set_indenter(ac_jsonw_t *json, const char *indenter);

#### ac\_jsonw\_add\_str - adds a string to the JSON

    void ac_jsonw_add_str(ac_jsonw_t *json, const char *name,
                          const char *value);

#### ac\_jsonw\_add\_int - adds an integer to the JSON

    void ac_jsonw_add_int(ac_jsonw_t *json, const char *name, s64 value);

#### ac\_jsonw\_add\_real - adds a realnumber to the JSON

    void ac_jsonw_add_real(ac_jsonw_t *json, const char *name, double value,
                           int dp);

#### ac\_jsonw\_add\_bool - adds a boolean value to the JSON

    void ac_jsonw_add_bool(ac_jsonw_t *json, const char *name, bool value);

#### ac\_jsonw\_add\_null - adds a null value to the JSON

    void ac_jsonw_add_null(ac_jsonw_t *json, const char *name);

#### ac\_jsonw\_add\_str\_or\_null - adds a string or a null value to the JSON

    void ac_jsonw_add_str_or_null(ac_jsonw_t *json, const char *name,
                                  const char *value);

#### ac\_jsonw\_add\_array - adds an array to the JSON

    void ac_jsonw_add_array(ac_jsonw_t *json, const char *name);

#### ac\_jsonw\_end\_array - ends a JSON array

    void ac_jsonw_end_array(ac_jsonw_t *json);

#### ac\_jsonw\_add\_object - adds a new JSON object to the JSON

    void ac_jsonw_add_object(ac_jsonw_t *json, const char *name);

#### ac\_jsonw\_end\_object - ends a JSON object

    void ac_jsonw_end_object(ac_jsonw_t *json);

#### ac\_jsonw\_end - ends the JSON

    void ac_jsonw_end(ac_jsonw_t *json);

#### ac\_jsonw\_free - free's the ac\_jsonw\_t object

    void ac_jsonw_free(const ac_jsonw_t *json);

#### ac\_jsonw\_len - gets the length of the created JSON

    size_t ac_jsonw_len(const ac_jsonw_t *json);

#### ac\_jsonw\_get - gets the created JSON

    const char *ac_jsonw_get(const ac_jsonw_t *json);


### Miscellaneous functions

#### ac\_misc\_ppb - pretty print bytes

    void ac_misc_ppb(u64 bytes, ac_si_units_t si, ac_misc_ppb_t *ppb);

#### ac\_misc\_passcrypt

    char *ac_misc_passcrypt(const char *pass, ac_hash_algo_t hash_type,
                            ac_crypt_data_t *data);

#### ac\_misc\_gen\_uuid4 - generate a type 4 UUID

    const char *ac_misc_gen_uuid4(char *dst);

#### ac\_misc\_luhn\_check - perform the Luhn Check on a number

    bool ac_misc_luhn_check(u64 num);

#### ac\_misc\_shuffle - shuffle a list of elements

    int ac_misc_shuffle(void *base, size_t nmemb, size_t size,
                        ac_misc_shuffle_t algo);

#### ac\_hash\_func\_str - create a hash value for a given string

    u32 ac_hash_func_str(const void *key);

#### ac\_hash\_func\_u32 - create a hash value for a given u32

    u32 ac_hash_func_u32(const void *key);

#### ac\_hash\_func\_ptr - create a hash for a given pointer

    u32 ac_hash_func_ptr(const void *key);

#### ac\_cmp\_ptr - compare two pointers

    int ac_cmp_ptr(const void *a, const void *b};

#### ac\_cmp\_str - compare two strings

    int ac_cmp_str(const void *a, const void *b);

#### ac\_cmp\_u32 - compare two u32's

    int ac_cmp_u32(const void *a, const void *b);


### Network related functions

#### ac\_net\_port\_from\_sa - extract the port number from a struct sockaddr

    u16 ac_net_port_from_sa(const struct sockaddr *sa);

#### ac\_net\_inet\_pton - address family agnostic wrapper around inet\_pton(3)

    int ac_net_inet_pton(const char *src, void *dst);

#### ac\_net\_inet\_ntop - address family agnostic wrapper around inet\_ntop(3)

    const char *ac_net_inet_ntop(const void *src, char *dst, socklen_t size);

#### ac\_net\_ns\_lookup\_by\_host - lookup a host by hostname (get its IP(s))

    int ac_net_ns_lookup_by_host(const struct addrinfo *hints,
                                 const char *node,
                                 bool (*ac_ns_lookup_cb)
                                      (const struct addrinfo *ai,
                                       const char *res));

#### ac\_net\_ns\_lookup\_by\_ip - lookup a host by IP address (get its hostname)

    int ac_net_ns_lookup_by_ip(const struct addrinfo *hints,
                               const char *node,
                               bool (*ac_ns_lookup_cb)
                                    (const struct addrinfo *ai,
                                     const char *res));

#### ac\_net\_ipv4\_isin - check if an IPv4 address is within the given network

    bool ac_net_ipv4_isin(const char *network, u8 cidr, const char *addr);

#### ac\_net\_ipv4\_isin\_sa - check if an IPv4 address is within the given network

    bool ac_net_ipv4_isin_sa(const char *network, u8 cidr,
                             const struct sockaddr *sa);

#### ac\_net\_ipv6\_isin - check if an IPv6 address is within the given network

    bool ac_net_ipv6_isin(const char *network, u8 prefixlen, const char *addr);

#### ac\_net\_ipv6\_isin\_sa - check if an IPv6 address is within the given network

    bool ac_net_ipv6_isin_sa(const char *network, u8 prefixlen,
                             const struct sockaddr *sa);


### Quark functions

#### ac\_quark\_init - initialise a new quark

    void ac_quark_init(ac_quark_t *quark, void(*free_func)(void *ptr));

#### ac\_quark\_from\_string - create a new string mapping

    int ac_quark_from_string(ac_quark_t *quark, const char *str);

#### ac\_quark\_to\_string - retrieve the given string

    const char *ac_quark_to_string(const ac_quark_t *quark, int id);

#### ac\_quark\_destroy - destroy a quark

    void ac_quark_destroy(const ac_quark_t *quark);


### Queue functions

#### ac\_queue\_new - create a new queue

    ac_queue_t *ac_queue_new(void);

#### ac\_queue\_push - add an item to the queue

    int ac_queue_push(ac_queue_t *queue, void *item);

#### ac\_queue\_pop - get the head element from the queue

    void *ac_queue_pop(ac_queue_t *queue);

#### ac\_queue\_foreach - iterate over elements in a queue

    void ac_queue_foreach(const ac_queue_t *queue,
                          void (*action)(void *item, void *data),
                          void *user_data);

#### ac\_queue\_nr\_items - get the number of elements in the queue

    u32 ac_queue_nr_items(const ac_queue_t *queue);

#### ac\_queue\_destroy - destroy a queue freeing all its memory

    void ac_queue_destroy(const ac_queue_t *queue, (*free_func)(void *item));


### Doubly linked list functions

#### ac\_list\_last - find the last item in the list

    ac_list_t *ac_list_last(ac_list_t *list);

#### ac\_list\_len - return the number of entries in the list

    long ac_list_len(const ac_list_t *list);

#### ac\_list\_add - add an item to the end of the list

    void ac_list_add(ac_list_t **list, void *data);

#### ac\_list\_preadd - add an item to the front of the list

    void ac_list_preadd(ac_list_t **list, void *data);

#### ac\_list\_remove - remove an item from the list

    bool ac_list_remove(ac_list_t **list, void *data,
                        void (*free_data)(void *data));

#### ac\_list\_remove\_nth - remove the nth item from the list

    bool ac_list_remove_nth(ac_list_t **list, long n,
                            void (*free_data)(void *data));

#### ac\_list\_remove\_custom - remove an item from the list with the given data

    bool ac_list_remove_custom(ac_list_t **list, void *data,
                               int (*compar)(const void *a, const void *b),
                               void (*free_data)(void *data));

#### ac\_list\_reverse - reverse a list

    void ac_list_reverse(ac_list_t **list);

#### ac\_list\_find - find an item in the list

    ac_list_t *ac_list_find(ac_list_t *list, const void *data);

#### ac\_list\_find\_custom - find an item in the list with the given data

    ac_list_t *ac_list_find_custom(ac_list_t *list, const void *data,
                                   int (*compar)(const void *a,
                                                 const void *b));

#### ac\_list\_nth\_data - retrieve the item's data at position n

    void *ac_list_nth_data(ac_list_t *list, long n)

#### ac\_list\_foreach - execute a function for each item in the list

    void ac_list_foreach(ac_list_t *list,
                         void (*action)(void *item, void *data),
                         void *user_data);

#### ac\_list\_rev\_foreach - execute a function for each item in the list in reverse

    void ac_list_rev_foreach(ac_list_t *list,
                             void (*action)(void *item, void *data),
                             void *user_data);

#### ac\_list\_destroy - destroy a list, optionally freeing all its items memory

    void ac_list_destroy(ac_list_t **list, void (*free_data)(void *data));


### Singly linked list functions

#### ac\_slist\_last - find the last item in the list

    ac_slist_t *ac_slist_last(ac_slist_t *list);

#### ac\_slist\_len - return the number of entries in the list

    long ac_slist_len(const ac_slist_t *list);

#### ac\_slist\_add - add an item to the end of the list

    void ac_slist_add(ac_slist_t **list, void *data);

#### ac\_slist\_preadd - add an item to the front of the list

    void ac_slist_preadd(ac_slist_t **list, void *data);

#### ac\_slist\_remove - remove an item from the list

    bool ac_slist_remove(ac_slist_t **list, void *data, void (*free_data)
                                                             (void *data));

#### ac\_slist\_remove\_nth - remove the nth item from the list

    bool ac_slist_remove_nth(ac_slist_t **list, long n, void (*free_data)
                                                             (void *data));

#### ac\_slist\_remove\_custom - remove an item from the list with the given data

    bool ac_slist_remove_custom(ac_slist_t **list, void *data,
                                int (*compar)(const void *a, const void *b),
                                void (*free_data)(void *data));

#### ac\_slist\_reverse - reverse a list

    void ac_slist_reverse(ac_slist_t **list);

#### ac\_slist\_find - find an item in the list

    ac_slist_t *ac_slist_find(ac_slist_t *list, const void *data);

#### ac\_slist\_find\_custom - find an item in the list with the given data

    ac_slist_t *ac_slist_find_custom(ac_slist_t *list, const void *data,
                                     int (*compar)(const void *a,
                                                   const void *b));

#### ac\_slist\_nth\_data - retrieve the item's data at position n

    void *ac_slist_nth_data(ac_slist_t *list, long n);

#### ac\_slist\_foreach - execute a function for each item in the list

    void ac_slist_foreach(ac_slist_t *list,
                          void (*action)(void *item, void *data),
                          void *user_data);

#### ac\_slist\_destroy - destroy a list, optionally freeing all its items memory

    void ac_slist_destroy(ac_slist_t **list, void (*free_data)(void *data));


### String functions

#### ac\_str\_freev - free a string vector

    void ac_str_freev(char **stringv);

#### ac\_str\_split - split a string up into a NULL terminated vector

    char **ac_str_split(const char *string, int delim, int flags);

#### ac\_str\_chomp - remove trailing white space from a string

    char *ac_str_chomp(char *string);

#### ac\_str\_substr - extract a substring from a string

    char *ac_str_substr(const char *src, char *dest, size_t start, size_t len);

#### ac\_str\_levenshtein - calculate the Levenshtein distance between two strings

    int ac_str_levenshtein(const char *s, const char *t);


### Time related functions

#### ac\_time\_tspec\_diff - subtract two struct timespec's

    double ac_time_tspec_diff(struct timespec *delta,
                              const struct timespec *end,
                              const struct timespec *start);

#### ac\_time\_secs\_to\_hms - convert total seconds to broken out hrs, mins & secs

    void ac_time_secs_to_hms(long total, int *hours, int *minutes,
                             int *seconds);

#### ac\_time\_nsleep - wrapper around nanosleep(2) thats sleeps through interrupts

    int ac_time_nsleep(u64 nsecs);


## Build it

libac is primarily developed under Linux but it also builds and runs under
FreeBSD.

### Linux

The simplest way is to build the rpm by simply doing

    $ make rpm

in the repository root. It just needs a suitable *rpmbuild/* directory
structure which can be created with

    $ mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

and the *rpmbuild* tool which can be found in the *rpm-build* package.

### FreeBSD

libac has a single dependency on FreeBSD of libuuid, it also need to be built
with gmake (GNU make), these can be installed with

    $ sudo pkg install e2fsprogs-libuuid gmake

then libac can be built with

    $ gmake

or if you don't have GCC installed

    $ gmake CC=clang

## How to use

Just

    #include <libac.h>

in your program and link with *-lac* assuming you've built and installed the
RPM or similar.

See *src/test.c* for examples on using the above.


## Thread safety

libac is intended to be thread-safe.


## License

This library is licensed under the GNU Lesser General Public License (LGPL)
version 2.1

See *COPYING* in the repository root for details.


## Contributing

See *CodingStyle.md* & *Contributing.md*
