/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * libac.h - Miscellaneous functions
 *
 * Copyright (c) 2017 - 2020 - 2021	Andrew Clayton
 *					<andrew@digital-domain.net>
 */

#ifndef _LIBAC_H_
#define _LIBAC_H_

#include <sys/types.h>
#include <inttypes.h>
#include <stdbool.h>
#include <search.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#ifndef __FreeBSD__
#include <crypt.h>
#endif
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIBAC_MAJOR_VERSION	 1
#define LIBAC_MINOR_VERSION	 99
#define LIBAC_MICRO_VERSION	 0

typedef uint64_t u64;
typedef int64_t  s64;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint8_t   u8;
typedef int8_t    s8;

#ifndef __unused
#define __unused		__attribute__((unused))
#endif
#ifndef __maybe_unused
#define __maybe_unused		__attribute__((unused))
#endif
#ifndef __always_unused
#define __always_unused		__attribute__((unused))
#endif

typedef struct crypt_data ac_crypt_data_t;

#define AC_BYTE_BIT_SET(byte, bit)	((byte) |= (1ULL << (bit)))
#define AC_BYTE_BIT_CLR(byte, bit)	((byte) &= ~(1ULL << (bit)))
#define AC_BYTE_BIT_FLP(byte, bit)	((byte) ^= (1ULL << (bit)))
#define AC_BYTE_BIT_TST(byte, bit)	((byte) & (1ULL << (bit)))

#define AC_BYTE_NIBBLE_HIGH(byte) (((byte) >> 4) & 0x0f)
#define AC_BYTE_NIBBLE_LOW(byte)  ((byte) & 0x0f)

#define AC_FS_AT_FDCWD		AT_FDCWD
#define AC_FS_COPY_OVERWRITE	0x01

#define AC_STR_SPLIT_ALWAYS	0x00
#define AC_STR_SPLIT_STRICT	0x01

/*
 * Define a second, millisecond and microsecond in terms of nanoseconds.
 */
#define AC_TIME_NS_SEC		1000000000L
#define AC_TIME_NS_MSEC		   1000000L
#define AC_TIME_NS_USEC		      1000L

#define AC_LONG_TO_PTR(x)	((void *)(long)x)
#define AC_PTR_TO_LONG(p)	((long)p)

#define AC_MAX(a, b) \
	({ __typeof__(a) _a = (a); \
           __typeof__ (b) _b = (b); \
         _a > _b ? _a : _b; })

#define AC_MIN(a, b) \
	({ __typeof__(a) _a = (a); \
           __typeof__ (b) _b = (b); \
         _a < _b ? _a : _b; })

#define AC_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define AC_UUID4_LEN		36

typedef enum {
	AC_GEO_EREF_WGS84 = 0,
	AC_GEO_EREF_GRS80,
	AC_GEO_EREF_AIRY1830
} ac_geo_ellipsoid_t;

typedef enum {
	AC_HASH_ALGO_MD5 = 0,
	AC_HASH_ALGO_SHA1,
	AC_HASH_ALGO_SHA256,
	AC_HASH_ALGO_SHA512
} ac_hash_algo_t;

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

typedef enum {
	AC_SI_UNITS_NO = 0,
	AC_SI_UNITS_YES
} ac_si_units_t;

typedef struct ac_btree {
	void *rootp;

	int (*compar)(const void *, const void *);
	void (*free_node)(void *nodep);
} ac_btree_t;

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

typedef struct {
	struct ac_slist **buckets;
	unsigned long count;

	u32 (*hash_func)(const void *key);
	int (*key_cmp)(const void *a, const void *b);
	void (*free_key_func)(void *ptr);
	void (*free_data_func)(void *ptr);
} ac_htable_t;

typedef struct {
	char *str;
	size_t len;
	size_t allocated;
	u8 depth;
	bool skip_tabs;
	char *indenter;
} ac_jsonw_t;

typedef struct ac_list {
	void *data;

	struct ac_list *prev;
	struct ac_list *next;
} ac_list_t;

typedef struct {
	ac_misc_ppb_factor_t factor;
	const char *prefix;

	union {
		u16 v_u16;
		float v_float;
	} value;
} ac_misc_ppb_t;

typedef struct {
	struct ac_btree *qt;
	void **quarks;
	int last;

	void (*free_func)(void *ptr);
} ac_quark_t;

typedef struct {
	struct ac_slist *queue;
	struct ac_slist *tail;
	u32 items;

	void (*free_func)(void *item);
} ac_queue_t;

typedef struct ac_slist {
	void *data;

	struct ac_slist *next;
} ac_slist_t;

#pragma GCC visibility push(default)
extern void *ac_btree_new(int (*compar)(const void *, const void *),
			  void (*free_node)(void *nodep));
extern void ac_btree_foreach(const ac_btree_t *tree,
			     void (*action)(const void *nodep, VISIT which,
					    int depth));
#if __FreeBSD__ || (__GLIBC__ >= 2 && __GLIBC_MINOR__ >= 30)
extern void ac_btree_foreach_data(const ac_btree_t *tree,
				  void (*action)(const void *nodep,
						 VISIT which, void *data),
				  void *user_data);
#endif
extern void *ac_btree_lookup(const ac_btree_t *tree, const void *key);
extern void *ac_btree_add(ac_btree_t *tree, const void *key);
extern void *ac_btree_remove(ac_btree_t *tree, const void *key);
extern void ac_btree_destroy(const ac_btree_t *tree);
extern bool ac_btree_is_empty(const ac_btree_t *tree);

extern bool ac_fs_is_posix_name(const char *name);
extern int ac_fs_mkdir_p(int dirfd, const char *path, mode_t mode);
extern ssize_t ac_fs_copy(const char *from, const char *to, int flags);

extern ac_circ_buf_t *ac_circ_buf_new(u32 size, u32 elem_sz);
extern u32 ac_circ_buf_count(const ac_circ_buf_t *cbuf);
extern int ac_circ_buf_pushm(ac_circ_buf_t *cbuf, const void *buf,
			     u32 count);
extern int ac_circ_buf_push(ac_circ_buf_t *cbuf, const void *buf);
extern int ac_circ_buf_popm(ac_circ_buf_t *cbuf, void *buf, u32 count);
extern void *ac_circ_buf_pop(ac_circ_buf_t *cbuf);
extern void ac_circ_buf_foreach(const ac_circ_buf_t *cbuf,
				void (*action)(void *item, void *data),
				void *user_data);
extern void ac_circ_buf_reset(ac_circ_buf_t *cbuf);
extern void ac_circ_buf_destroy(const ac_circ_buf_t *cbuf);

extern void ac_geo_dd_to_dms(double degrees, ac_geo_dms_t *dms);
extern double ac_geo_dms_to_dd(const ac_geo_dms_t *dms);
extern double ac_geo_haversine(const ac_geo_t *from, const ac_geo_t *to);
extern void ac_geo_vincenty_direct(const ac_geo_t *from, ac_geo_t *to,
				   double distance);
extern void ac_geo_bng_to_lat_lon(ac_geo_t *geo);
extern void ac_geo_lat_lon_to_bng(ac_geo_t *geo);

extern ac_htable_t *ac_htable_new(u32 (*hash_func)(const void *key),
				  int (*key_cmp)(const void *a, const void *b),
				  void (*free_key_func)(void *key),
				  void (*free_data_func)(void *data));
extern void ac_htable_insert(ac_htable_t *htable, void *key, void *data);
extern bool ac_htable_remove(ac_htable_t *htable, const void *key);
extern void *ac_htable_lookup(const ac_htable_t *htable, const void *key);
extern void ac_htable_foreach(const ac_htable_t *htable,
			      void (*action)(void *key, void *value,
					     void *user_data),
			      void *user_data);
extern void ac_htable_destroy(const ac_htable_t *htable);

extern char *ac_json_load_from_fd(int fd, off_t offset);
extern char *ac_json_load_from_file(const char *file, off_t offset);

extern ac_jsonw_t *ac_jsonw_init(void);
extern void ac_jsonw_indent_sz(ac_jsonw_t *json, int size);
extern void ac_jsonw_set_indenter(ac_jsonw_t *json, const char *indenter);
extern void ac_jsonw_add_str(ac_jsonw_t *json, const char *name,
			     const char *value);
extern void ac_jsonw_add_int(ac_jsonw_t *json, const char *name, s64 value);
extern void ac_jsonw_add_real(ac_jsonw_t *json, const char *name,
			      double value, int dp);
extern void ac_jsonw_add_bool(ac_jsonw_t *json, const char *name, bool value);
extern void ac_jsonw_add_null(ac_jsonw_t *json, const char *name);
extern void ac_jsonw_add_str_or_null(ac_jsonw_t *json, const char *name,
				     const char *value);
extern void ac_jsonw_add_array(ac_jsonw_t *json, const char *name);
extern void ac_jsonw_end_array(ac_jsonw_t *json);
extern void ac_jsonw_add_object(ac_jsonw_t *json, const char *name);
extern void ac_jsonw_end_object(ac_jsonw_t *json);
extern void ac_jsonw_end(ac_jsonw_t *json);
extern void ac_jsonw_free(const ac_jsonw_t *json);
extern size_t ac_jsonw_len(const ac_jsonw_t *json);
extern const char *ac_jsonw_get(const ac_jsonw_t *json);

extern ac_list_t *ac_list_last(ac_list_t *list);
extern long ac_list_len(const ac_list_t *list);
extern void ac_list_add(ac_list_t **list, void *data);
extern void ac_list_preadd(ac_list_t **list, void *data);
extern bool ac_list_remove(ac_list_t **list, void *data,
			   void (*free_data)(void *data));
extern bool ac_list_remove_nth(ac_list_t **list, long n,
			       void (*free_data)(void *data));
extern bool ac_list_remove_custom(ac_list_t **list, void *data,
				  int (*compar)(const void *a, const void *b),
				  void (*free_data)(void *data));
extern void ac_list_reverse(ac_list_t **list);
extern ac_list_t *ac_list_find(ac_list_t *list, const void *data);
extern ac_list_t *ac_list_find_custom(ac_list_t *list, const void *data,
				      int (*compar)(const void *a,
						    const void *b));
extern void *ac_list_nth_data(ac_list_t *list, long n);
extern void ac_list_foreach(ac_list_t *list,
			    void (*action)(void *item, void *data),
			    void *user_data);
extern void ac_list_rev_foreach(ac_list_t *list,
				void (*action)(void *item, void *data),
				void *user_data);
extern void ac_list_destroy(ac_list_t **list, void (*free_data)(void *data));

extern void ac_misc_ppb(u64 bytes, ac_si_units_t si, ac_misc_ppb_t *ppb);
extern char *ac_misc_passcrypt(const char *pass, ac_hash_algo_t hash_type,
			       ac_crypt_data_t *data);
extern const char *ac_misc_gen_uuid4(char *dst);
extern int ac_misc_shuffle(void *base, size_t nmemb, size_t size,
			   ac_misc_shuffle_t algo);
extern bool ac_misc_luhn_check(u64 num);
extern u32 ac_hash_func_ptr(const void *key);
extern u32 ac_hash_func_str(const void *key);
extern u32 ac_hash_func_u32(const void *key);
extern int ac_cmp_ptr(const void *a, const void *b);
extern int ac_cmp_str(const void *a, const void *b);
extern int ac_cmp_u32(const void *a, const void *b);

extern u16 ac_net_port_from_sa(const struct sockaddr *sa);
extern int ac_net_inet_pton(const char *src, void *dst);
extern const char *ac_net_inet_ntop(const void *src, char *dst, socklen_t size);
extern int ac_net_ns_lookup_by_host(const struct addrinfo *hints,
				    const char *node,
				    bool (*ac_ns_lookup_cb)
				         (const struct addrinfo *ai,
					  const char *res));
extern int ac_net_ns_lookup_by_ip(const struct addrinfo *hints,
				  const char *node,
				  bool (*ac_ns_lookup_cb)
				       (const struct addrinfo *ai,
					const char *res));
extern bool ac_net_ipv4_isin(const char *network, u8 cidr, const char *addr);
extern bool ac_net_ipv4_isin_sa(const char *network, u8 prefixlen,
				const struct sockaddr *sa);
extern bool ac_net_ipv6_isin(const char *network, u8 prefixlen,
			     const char *addr);
extern bool ac_net_ipv6_isin_sa(const char *network, u8 prefixlen,
				const struct sockaddr *sa);

extern void ac_quark_init(ac_quark_t *quark, void (*free_func)(void *ptr));
extern int ac_quark_from_string(ac_quark_t *quark, const char *str);
extern const char *ac_quark_to_string(const ac_quark_t *quark, int id);
extern void ac_quark_destroy(const ac_quark_t *quark);

extern ac_queue_t *ac_queue_new(void);
extern u32 ac_queue_nr_items(const ac_queue_t *queue);
extern int ac_queue_push(ac_queue_t *queue, void *item);
extern void *ac_queue_pop(ac_queue_t *queue);
extern void ac_queue_foreach(const ac_queue_t *queue,
			     void (*action)(void *item, void *data),
			     void *user_data);
extern void ac_queue_destroy(const ac_queue_t *queue,
			     void (*free_func)(void *item));

extern ac_slist_t *ac_slist_last(ac_slist_t *list);
extern long ac_slist_len(const ac_slist_t *list);
extern void ac_slist_add(ac_slist_t **list, void *data);
extern void ac_slist_preadd(ac_slist_t **list, void *data);
extern bool ac_slist_remove(ac_slist_t **list, void *data,
			    void (*free_data)(void *data));
extern bool ac_slist_remove_nth(ac_slist_t **list, long n,
				void (*free_data)(void *data));
extern bool ac_slist_remove_custom(ac_slist_t **list, void *data,
				   int (*compar)(const void *a, const void *b),
				   void (*free_data)(void *data));
extern void ac_slist_reverse(ac_slist_t **list);
extern ac_slist_t *ac_slist_find(ac_slist_t *list, const void *data);
extern ac_slist_t *ac_slist_find_custom(ac_slist_t *list, const void *data,
					int (*compar)(const void *a,
						      const void *b));
extern void *ac_slist_nth_data(ac_slist_t *list, long n);
extern void ac_slist_foreach(ac_slist_t *list,
			     void (*action)(void *item, void *data),
			     void *user_data);
extern void ac_slist_destroy(ac_slist_t **list, void (*free_data)(void *data));

extern void ac_str_freev(char **stringv);
extern char **ac_str_split(const char *string, int delim, int flags);
extern char *ac_str_chomp(char *string);
extern char *ac_str_substr(const char *src, char *dest, size_t start,
			   size_t len);
extern int ac_str_levenshtein(const char *s, const char *t);

extern double ac_time_tspec_diff(struct timespec *delta,
				 const struct timespec *end,
				 const struct timespec *start);
extern void ac_time_secs_to_hms(long total, int *hours, int *minutes,
				int *seconds);
extern int ac_time_nsleep(u64 period);
#pragma GCC visibility pop

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _LIBAC_H_ */
