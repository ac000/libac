.. highlight:: c

libac
=====

|Builds| |FreeBSD Build Status| |CodeQL|

1. `Overview <#overview>`__
2. `General types & macros <#general-types-macros>`__

-  `Library version <#library-version>`__
-  `Types <#types>`__
-  `Macros <#macros>`__

3. `Functions <#functions>`__

-  `Binary Search Tree functions <#binary-search-tree-functions>`__
-  `Circular Buffer functions <#circular-buffer-functions>`__
-  `Filesystem related functions <#filesystem-related-functions>`__
-  `Geospatial related functions <#geospatial-related-functions>`__
-  `Hash Table functions <#hash-table-functions>`__
-  `JSON functions <#json-functions>`__
-  `JSON Writer functions <#json-writer-functions>`__
-  `Miscellaneous functions <#miscellaneous-functions>`__
-  `Network related functions <#network-related-functions>`__
-  `Quark (string to integer mapping) functions <#quark-functions>`__
-  `Queue functions <#queue-functions>`__
-  `Doubly linked list functions <doubly-linked-list-functions>`__
-  `Singly linked list functions <#singly-linked-list-functions>`__
-  `String functions <#string-functions>`__
-  `Time related functions <#time-related-functions>`__

4. `Build it <#build-it>`__
5. `How to use <#how-to-use>`__
6. `License <#license>`__
7. `Contributing <#contributing>`__

Overview
--------

A C library of miscellaneous functions that has no dependencies other
than Glibc.

General types & macros
----------------------

Library version
~~~~~~~~~~~~~~~

.. code-block::

    #define LIBAC_MAJOR_VERSION
    #define LIBAC_MINOR_VERSION
    #define LIBAC_MICRO_VERSION

Types
~~~~~

.. code-block::

    typedef uint64_t u64
    typedef int64_t  s64
    typedef uint32_t u32
    typedef int32_t  s32
    typedef uint16_t u16
    typedef int16_t  s16
    typedef uint8_t   u8
    typedef int8_t    s8

    typedef enum {
        AC_HASH_ALGO_MD5 = 0,
        AC_HASH_ALGO_SHA1,
        AC_HASH_ALGO_SHA256,
        AC_HASH_ALGO_SHA512
    } ac_hash_algo_t;

    typedef enum {
        AC_SI_UNITS_NO,
        AC_SI_UNITS_YES,
    } ac_si_units_t;

Macros
~~~~~~

.. code-block::

    #define AC_BYTE_BIT_SET(byte, bit)
    #define AC_BYTE_BIT_CLR(byte, bit)
    #define AC_BYTE_BIT_FLP(byte, bit)
    #define AC_BYTE_BIT_TST(byte, bit)
    #define AC_BYTE_NIBBLE_HIGH(byte)
    #define AC_BYTE_NIBBLE_LOW(byte)

    #define AC_LONG_TO_PTR(x)
    #define AC_PTR_TO_LONG(p)

    #define AC_MIN(a, b)
    #define AC_MAX(a, b)

    #define AC_ARRAY_SIZE(a)

    #define AC_UUID4_LEN

misc
~~~~

.. code-block::

    #define __unused
    #define __maybe_unused
    #define __always_unused

These are aliases for \__attribute\_\_((unused))

.. code-block::

    #define __nonstring

This is an alias for \__attribute\_\_((nonstring))

.. code-block::

    #define AC_UUID4_LEN

Functions
---------

Binary Search Tree functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These are a thin wrapper around the Glibc TSEARCH(3) set of binary tree
functions.

Types
~~~~~

.. code-block::

    typedef struct ac_btree {
        void *rootp;

        int (*compar)(const void *, const void *);
        void (*free_node)(void *nodep);
    } ac_btree_t;

ac_btree_new - create a new binary tree
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_btree_new(int (*compar)(const void *, const void *),
                      void (*free_node)(void *nodep)):

ac_btree_add - add a node to the tree
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_btree_add(ac_btree_t *tree, const void *key);

ac_btree_remove - remove a node from the tree
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_btree_remove(ac_btree_t *tree, const void *key);

ac_btree_lookup - lookup a node in the tree
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_btree_lookup(const ac_btree_t *tree, const void *key);

ac_btree_foreach - iterate over the tree
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_btree_foreach(const ac_btree_t *tree,
                         void (*action)(const void *nodep, VISIT which,
                                        int depth));

ac_btree_foreach_data - iterate over the tree with user_data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_btree_foreach(const ac_btree_t *tree,
                         void (*action)(const void *nodep, VISIT which,
                                        void *data),
                         void *user_data);

ac_btree_is_empty - test if the binary tree is empty
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_btree_is_empty(const ac_btree_t *tree);

ac_btree_destroy - destroy a binary tree freeing all memory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_btree_destroy(const ac_btree_t *tree);

Circular Buffer functions
~~~~~~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef struct {
        union {
            void *cpy_buf;
            void **ptr_buf;
        } buf;

        u32 head;
        u32 tail;

        u32 size;
        u32 elem_sz;

        int type;
    } ac_circ_buf_t;

ac_circ_buf_new - create a new circular buffer (size must be power of 2)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_circ_buf_t *ac_circ_buf_new(u32 size, u32 elem_sz);

ac_circ_buf_count - how many items are in the buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   u32 ac_circ_buf_count(const ac_circ_buf_t *cbuf);

ac_circ_buf_pushm - push multiple items into the buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_circ_buf_pushm(ac_circ_buf_t *cbuf, void *buf, u32 count);

ac_circ_buf_push - push an item into the buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_circ_buf_push(ac_circ_buf_t *cbuf, void *buf);

ac_circ_buf_popm - pop multiple items from buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_circ_buf_popm(ac_circ_buf_t *cbuf, void *buf, u32 count);

ac_circ_buf_pop - pop an item from the buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_circ_buf_pop(ac_circ_buf_t *cbuf);

ac_circ_buf_foreach - iterate over elements in the circular buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_circ_buf_foreach(const ac_circ_buf_t *cbuf,
                            void (*action)(void *item, void *data),
                            void *user_data);

ac_circ_buf_reset - reset the circular buffer to empty
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_circ_buf_reset(ac_circ_buf_t *cbuf);

ac_circ_buf_destroy - destroy a circular buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_circ_buf_destroy(const ac_circ_buf_t *cbuf);

Filesystem related functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    #define AC_FS_AT_FDCWD          AT_FDCWD
    #define AC_FS_COPY_OVERWRITE    0x01

ac_fs_is_posix_name - checks if a filename follows POSIX guidelines
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_fs_is_posix_name(const char *name);

ac_fs_mkdir_p - mkdir with parents
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_fs_mkdir_p(int dirfd, const char *path, mode_t mode);

ac_fs_copy - copy a file
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ssize_t ac_fs_copy(const char *from, const char *to, int flags);

Geospatial related functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef enum {
        AC_GEO_EREF_WGS84 = 0,
        AC_GEO_EREF_GRS80,
        AC_GEO_EREF_AIRY1830
    } ac_geo_ellipsoid_t;

    typedef struct {
        ac_geo_ellipsoid_t ref;
        double lat;
        double lon;
        double alt;
        double bearing;
        double easting;
        double northing;
    } ac_geo_t;

    typedef struct {
         int degrees;
         int minutes;
         double seconds;
    } ac_geo_dms_t;

ac_geo_dd_to_dms - convert decimal degrees into degrees, minutes & seconds
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_geo_dd_to_dms(double degrees, ac_geo_dms_t *dms);

ac_geo_dms_to_dd - convert degrees, minutes & seconds into decimal degrees
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   double ac_geo_dms_to_dd(const ac_geo_dms_t *dms);

ac_geo_haversine - calculate the distance between two points on Earth
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   double ac_geo_haversine(const ac_geo_t *from, const ac_geo_t *to);

ac_geo_vincenty_direct - given an initial point, bearing and distance calculate the end point
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_geo_vincenty_direct(const ac_geo_t *from, ac_geo_t *to,
                               double distance);

ac_geo_bng_to_lat_lon - convert British National grid Eastings & Northings to latitude & longitude decimal degrees
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_geo_bng_to_lat_lon(ac_geo_t *geo);

ac_geo_lat_lon_to_bng - convert latitude & longitude decimal degrees to British National Grid Eastings & Northings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_geo_lat_lon_to_bng(ac_geo_t *geo);

Hash Table functions
~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef struct {
        struct ac_slist **buckets;
        unsigned long count;

        u32 (*hash_func)(const void *key);
        int (*key_cmp)(const void *a, const void *b);
        void (*free_key_func)(void *ptr);
        void (*free_data_func)(void *ptr);
    } ac_htable_t;

ac_htable_new - create a new hash table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_htable_t *ac_htable_new(u32 (*hash_func)(const void *key),
                              int (*key_cmp)(const void *a, const void *b),
                              void (*free_key_func)(void *key),
                              void (*free_data_func)(void *data));

ac_htable_insert - inserts a new entry into a hash table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_htable_insert(ac_htable_t *htable, void *key, void *data);

ac_htable_remove - remove an entry from a hash table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_htable_remove(ac_htable_t *htable, const void *key);

ac_htable_lookup - lookup an entry in a hash table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_htable_lookup(const ac_htable_t *htable, const void *key);

ac_htable_foreach - iterate over each entry in a hash table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_htable_foreach(const ac_htable_t *htable,
                          void (*action)(void *key, void *value,
                                         void *user_data), void *user_data);

ac_htable_destroy - destroy the given hash table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_htable_destroy(const ac_htable_t *htable);

JSON functions
~~~~~~~~~~~~~~

ac_json_load_from_fd - loads json from an open file descriptor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   char *ac_json_load_from_fd(int fd, off_t offset);

ac_json_load_from_file - loads json from the specified file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   char *ac_json_load_from_file(const char *file, off_t offset);

JSON Writer functions
~~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef struct {
        char *str;
        size_t len;
        size_t allocated;
        u8 depth;
        bool skip_tabs;
        char *indenter;
    } ac_jsonw_t;

ac_jsonw_init - initialises a new ac_jsonw_t object
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_jsonw_t *ac_jsonw_init(void);

void ac_jsonw_indent_sz - set the JSON indentation size
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_indent_sz(ac_jsonw_t *json, int size);

ac_jsonw_set_indenter - set the indentation character/string
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_set_indenter(ac_jsonw_t *json, const char *indenter);

ac_jsonw_add_str - adds a string to the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_add_str(ac_jsonw_t *json, const char *name,
                         const char *value);

ac_jsonw_add_int - adds an integer to the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_add_int(ac_jsonw_t *json, const char *name, s64 value);

ac_jsonw_add_real - adds a realnumber to the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_add_real(ac_jsonw_t *json, const char *name, double value,
                          int dp);

ac_jsonw_add_bool - adds a boolean value to the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_add_bool(ac_jsonw_t *json, const char *name, bool value);

ac_jsonw_add_null - adds a null value to the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_add_null(ac_jsonw_t *json, const char *name);

ac_jsonw_add_str_or_null - adds a string or a null value to the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_add_str_or_null(ac_jsonw_t *json, const char *name,
                                 const char *value);

ac_jsonw_add_array - adds an array to the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_add_array(ac_jsonw_t *json, const char *name);

ac_jsonw_end_array - ends a JSON array
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_end_array(ac_jsonw_t *json);

ac_jsonw_add_object - adds a new JSON object to the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_add_object(ac_jsonw_t *json, const char *name);

ac_jsonw_end_object - ends a JSON object
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_end_object(ac_jsonw_t *json);

ac_jsonw_end - ends the JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_end(ac_jsonw_t *json);

ac_jsonw_free - free’s the ac_jsonw_t object
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_jsonw_free(const ac_jsonw_t *json);

ac_jsonw_len - gets the length of the created JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   size_t ac_jsonw_len(const ac_jsonw_t *json);

ac_jsonw_get - gets the created JSON
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   const char *ac_jsonw_get(const ac_jsonw_t *json);

Miscellaneous functions
~~~~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef struct crypt_data ac_crypt_data_t;

    typedef enum {
        AC_MISC_PPB_BYTES = 0,
        AC_MISC_PPB_KBYTES,
        AC_MISC_PPB_MBYTES,
        AC_MISC_PPB_GBYTES,
        AC_MISC_PPB_TBYTES,
        AC_MISC_PPB_PBYTES,
        AC_MISC_PPB_EBYTES
    } ac_misc_ppb_factor_t;

    typedef enum {
        AC_MISC_SHUFFLE_FISHER_YATES = 0
    } ac_misc_shuffle_t;

    typedef struct {
        ac_misc_ppb_factor_t factor;
        const char *prefix;

        union {
            u16 v_u16;
            float v_float;
        } value;
    } ac_misc_ppb_t;

ac_misc_ppb - pretty print bytes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_misc_ppb(u64 bytes, ac_si_units_t si, ac_misc_ppb_t *ppb);

ac_misc_passcrypt
^^^^^^^^^^^^^^^^^

.. code-block::

   char *ac_misc_passcrypt(const char *pass, ac_hash_algo_t hash_type,
                           ac_crypt_data_t *data);

ac_misc_gen_uuid4 - generate a type 4 UUID
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   const char *ac_misc_gen_uuid4(char *dst);

ac_misc_luhn_check - perform the Luhn Check on a number
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_misc_luhn_check(u64 num);

ac_misc_shuffle - shuffle a list of elements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_misc_shuffle(void *base, size_t nmemb, size_t size,
                       ac_misc_shuffle_t algo);

ac_hash_func_str - create a hash value for a given string
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   u32 ac_hash_func_str(const void *key);

ac_hash_func_u32 - create a hash value for a given u32
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   u32 ac_hash_func_u32(const void *key);

ac_hash_func_ptr - create a hash for a given pointer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   u32 ac_hash_func_ptr(const void *key);

ac_cmp_ptr - compare two pointers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_cmp_ptr(const void *a, const void *b};

ac_cmp_str - compare two strings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_cmp_str(const void *a, const void *b);

ac_cmp_u32 - compare two u32’s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_cmp_u32(const void *a, const void *b);

Network related functions
~~~~~~~~~~~~~~~~~~~~~~~~~

ac_net_port_from_sa - extract the port number from a struct sockaddr
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_net_port_from_sa(const struct sockaddr *sa);

ac_net_inet_pton - address family agnostic wrapper around inet_pton(3)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_net_inet_pton(const char *src, void *dst);

ac_net_inet_ntop - address family agnostic wrapper around inet_ntop(3)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   const char *ac_net_inet_ntop(const void *src, char *dst, socklen_t size);

ac_net_ns_lookup_by_host - lookup a host by hostname (get its IP(s))
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_net_ns_lookup_by_host(const struct addrinfo *hints,
                                const char *node,
                                bool (*ac_ns_lookup_cb)
                                     (const struct addrinfo *ai,
                                      const char *res));

ac_net_ns_lookup_by_ip - lookup a host by IP address (get its hostname)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_net_ns_lookup_by_ip(const struct addrinfo *hints,
                              const char *node,
                              bool (*ac_ns_lookup_cb)
                                   (const struct addrinfo *ai,
                                    const char *res));

ac_net_ipv4_isin - check if an IPv4 address is within the given network
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_net_ipv4_isin(const char *network, u8 cidr, const char *addr);

ac_net_ipv4_isin_sa - check if an IPv4 address is within the given network
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_net_ipv4_isin_sa(const char *network, u8 cidr,
                            const struct sockaddr *sa);

ac_net_ipv6_isin - check if an IPv6 address is within the given network
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_net_ipv6_isin(const char *network, u8 prefixlen, const char *addr);

ac_net_ipv6_isin_sa - check if an IPv6 address is within the given network
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_net_ipv6_isin_sa(const char *network, u8 prefixlen,
                            const struct sockaddr *sa);

Quark functions
~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef struct {
        struct ac_btree *qt;
        void **quarks;
        int last;

        void (*free_func)(void *ptr);
    } ac_quark_t;

ac_quark_init - initialise a new quark
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_quark_init(ac_quark_t *quark, void(*free_func)(void *ptr));

ac_quark_from_string - create a new string mapping
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_quark_from_string(ac_quark_t *quark, const char *str);

ac_quark_to_string - retrieve the given string
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   const char *ac_quark_to_string(const ac_quark_t *quark, int id);

ac_quark_destroy - destroy a quark
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_quark_destroy(const ac_quark_t *quark);

Queue functions
~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef struct {
        struct ac_slist *queue;
        struct ac_slist *tail;
        u32 items;

        void (*free_func)(void *item);
    } ac_queue_t;

ac_queue_new - create a new queue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_queue_t *ac_queue_new(void);

ac_queue_push - add an item to the queue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_queue_push(ac_queue_t *queue, void *item);

ac_queue_pop - get the head element from the queue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_queue_pop(ac_queue_t *queue);

ac_queue_foreach - iterate over elements in a queue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_queue_foreach(const ac_queue_t *queue,
                         void (*action)(void *item, void *data),
                         void *user_data);

ac_queue_nr_items - get the number of elements in the queue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   u32 ac_queue_nr_items(const ac_queue_t *queue);

ac_queue_destroy - destroy a queue freeing all its memory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_queue_destroy(const ac_queue_t *queue, (*free_func)(void *item));

Doubly linked list functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef struct ac_list {
        void *data;

        struct ac_list *prev;
        struct ac_list *next;
    } ac_list_t;

ac_list_last - find the last item in the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_list_t *ac_list_last(ac_list_t *list);

ac_list_len - return the number of entries in the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   long ac_list_len(const ac_list_t *list);

ac_list_add - add an item to the end of the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_list_add(ac_list_t **list, void *data);

ac_list_preadd - add an item to the front of the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_list_preadd(ac_list_t **list, void *data);

ac_list_remove - remove an item from the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_list_remove(ac_list_t **list, void *data,
                       void (*free_data)(void *data));

ac_list_remove_nth - remove the nth item from the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_list_remove_nth(ac_list_t **list, long n,
                           void (*free_data)(void *data));

ac_list_remove_custom - remove an item from the list with the given data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_list_remove_custom(ac_list_t **list, void *data,
                              int (*compar)(const void *a, const void *b),
                              void (*free_data)(void *data));

ac_list_reverse - reverse a list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_list_reverse(ac_list_t **list);

ac_list_find - find an item in the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_list_t *ac_list_find(ac_list_t *list, const void *data);

ac_list_find_custom - find an item in the list with the given data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_list_t *ac_list_find_custom(ac_list_t *list, const void *data,
                                  int (*compar)(const void *a,
                                                const void *b));

ac_list_nth_data - retrieve the item’s data at position n
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_list_nth_data(ac_list_t *list, long n)

ac_list_foreach - execute a function for each item in the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_list_foreach(ac_list_t *list,
                        void (*action)(void *item, void *data),
                        void *user_data);

ac_list_rev_foreach - execute a function for each item in the list in reverse
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_list_rev_foreach(ac_list_t *list,
                            void (*action)(void *item, void *data),
                            void *user_data);

ac_list_destroy - destroy a list, optionally freeing all its items memory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_list_destroy(ac_list_t **list, void (*free_data)(void *data));

Singly linked list functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    typedef struct ac_slist {
        void *data;

        struct ac_slist *next;
    } ac_slist_t;

ac_slist_last - find the last item in the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_slist_t *ac_slist_last(ac_slist_t *list);

ac_slist_len - return the number of entries in the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   long ac_slist_len(const ac_slist_t *list);

ac_slist_add - add an item to the end of the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_slist_add(ac_slist_t **list, void *data);

ac_slist_preadd - add an item to the front of the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_slist_preadd(ac_slist_t **list, void *data);

ac_slist_remove - remove an item from the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_slist_remove(ac_slist_t **list, void *data, void (*free_data)
                                                            (void *data));

ac_slist_remove_nth - remove the nth item from the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_slist_remove_nth(ac_slist_t **list, long n, void (*free_data)
                                                            (void *data));

ac_slist_remove_custom - remove an item from the list with the given data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   bool ac_slist_remove_custom(ac_slist_t **list, void *data,
                               int (*compar)(const void *a, const void *b),
                               void (*free_data)(void *data));

ac_slist_reverse - reverse a list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_slist_reverse(ac_slist_t **list);

ac_slist_find - find an item in the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_slist_t *ac_slist_find(ac_slist_t *list, const void *data);

ac_slist_find_custom - find an item in the list with the given data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   ac_slist_t *ac_slist_find_custom(ac_slist_t *list, const void *data,
                                    int (*compar)(const void *a,
                                                  const void *b));

ac_slist_nth_data - retrieve the item’s data at position n
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void *ac_slist_nth_data(ac_slist_t *list, long n);

ac_slist_foreach - execute a function for each item in the list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_slist_foreach(ac_slist_t *list,
                         void (*action)(void *item, void *data),
                         void *user_data);

ac_slist_destroy - destroy a list, optionally freeing all its items memory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_slist_destroy(ac_slist_t **list, void (*free_data)(void *data));

String functions
~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    #define AC_STR_SPLIT_ALWAYS     0x00
    #define AC_STR_SPLIT_STRICT     0x01

ac_str_freev - free a string vector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_str_freev(char **stringv);

ac_str_split - split a string up into a NULL terminated vector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   char **ac_str_split(const char *string, int delim, int flags);

ac_str_chomp - remove trailing white space from a string
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   char *ac_str_chomp(char *string);

ac_str_substr - extract a substring from a string
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   char *ac_str_substr(const char *src, char *dest, size_t start, size_t len);

ac_str_levenshtein - calculate the Levenshtein distance between two strings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_str_levenshtein(const char *s, const char *t);

Time related functions
~~~~~~~~~~~~~~~~~~~~~~

Types
~~~~~

.. code-block::

    /*
     * Define a second, millisecond and microsecond in terms of nanoseconds.
     */
    #define AC_TIME_NS_SEC          1000000000L
    #define AC_TIME_NS_MSEC            1000000L
    #define AC_TIME_NS_USEC               1000L

ac_time_tspec_diff - subtract two struct timespec’s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   double ac_time_tspec_diff(struct timespec *delta,
                             const struct timespec *end,
                             const struct timespec *start);

ac_time_secs_to_hms - convert total seconds to broken out hrs, mins & secs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   void ac_time_secs_to_hms(long total, int *hours, int *minutes,
                            int *seconds);

ac_time_nsleep - wrapper around nanosleep(2) that sleeps through interrupts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   int ac_time_nsleep(u64 nsecs);

Build it
--------

libac is primarily developed under Linux but it also builds and runs
under FreeBSD.

Linux
~~~~~

The simplest way is to build the rpm by simply doing

::

   $ make rpm

in the repository root. It just needs a suitable *rpmbuild/* directory
structure which can be created with

::

   $ mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

and the *rpmbuild* tool which can be found in the *rpm-build* package.

FreeBSD
~~~~~~~

libac has a single dependency on FreeBSD of libuuid, it also needs to be
built with gmake (GNU make), these can be installed with

::

   $ sudo pkg install e2fsprogs-libuuid gmake

then libac can be built with

::

   $ gmake

or if you don’t have GCC installed

::

   $ gmake CC=clang

How to use
----------

Just

.. code-block::

   #include <libac.h>

in your program and link with *-lac* assuming you’ve built and installed
the RPM or similar.

See *src/test.c* for examples on using the above.

Thread safety
-------------

libac is intended to be thread-safe.

License
-------

This library is licensed under the GNU Lesser General Public License
(LGPL) version 2.1

See *COPYING* in the repository root for details.

Contributing
------------

See `CodingStyle.rst </CodingStyle.rst>`__ &
`Contributing.rst </Contributing.rst>`__

Andrew Clayton <ac@sigsegv.uk>

.. |Builds| image:: https://github.com/ac000/libac/actions/workflows/build_tests.yaml/badge.svg
   :target: https://github.com/ac000/libac/actions/workflows/build_tests.yaml
.. |FreeBSD Build Status| image:: https://api.cirrus-ci.com/github/ac000/libac.svg
   :target: https://cirrus-ci.com/github/ac000/libac
.. |CodeQL| image:: https://github.com/ac000/libac/workflows/CodeQL/badge.svg
   :target: https://github.com/ac000/libac/actions?query=workflow:CodeQL
