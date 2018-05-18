/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * libac.h - Miscellaneous functions
 *
 * Copyright (c) 2017 - 2018	Andrew Clayton <andrew@digital-domain.net>
 */

#ifndef _LIBAC_H_
#define _LIBAC_H_

#include <inttypes.h>
#include <stdbool.h>
#include <search.h>
#include <netdb.h>
#include <time.h>
#include <crypt.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIBAC_MAJOR_VERSION	 0
#define LIBAC_MINOR_VERSION	28
#define LIBAC_MICRO_VERSION	 3

typedef uint64_t u64;
typedef int64_t  s64;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint8_t   u8;
typedef int8_t    s8;

#define __unused		__attribute__((unused))
#define __maybe_unused		__attribute__((unused))
#define __always_unused		__attribute__((unused))

typedef struct crypt_data ac_crypt_data_t;

#define AC_BYTE_BIT_SET(byte, bit)	((byte) |= (1 << (bit)))
#define AC_BYTE_BIT_CLR(byte, bit)	((byte) &= ~(1 << (bit)))
#define AC_BYTE_BIT_FLP(byte, bit)	((byte) ^= (1 << (bit)))
#define AC_BYTE_BIT_TST(byte, bit)	((byte) & (1 << (bit)))

#define AC_BYTE_NIBBLE_HIGH(byte) (((byte) >> 4) & 0x0f)
#define AC_BYTE_NIBBLE_LOW(byte)  ((byte) & 0x0f)

#define AC_CQUEUE_OVERWRITE	0x01

#define AC_FS_COPY_OVERWRITE	0x01

#define AC_STR_SPLIT_ALWAYS	0x00
#define AC_STR_SPLIT_STRICT	0x01

/*
 * Define a second, millisecond and microsecond in terms of nanoseconds.
 */
#define AC_TIME_NS_SEC		1000000000ULL
#define AC_TIME_NS_MSEC		   1000000ULL
#define AC_TIME_NS_USEC		      1000ULL

#define AC_LONG_TO_PTR(x)	((void *)(long)x)
#define AC_PTR_TO_LONG(p)	((long)p)

typedef enum ac_geo_ellipsoid_t {
	AC_GEO_EREF_WGS84 = 0,
	AC_GEO_EREF_GRS80,
	AC_GEO_EREF_AIRY1830
} ac_geo_ellipsoid_t;

typedef enum ac_hash_algo_t {
	AC_HASH_ALGO_MD5 = 0,
	AC_HASH_ALGO_SHA1,
	AC_HASH_ALGO_SHA256,
	AC_HASH_ALGO_SHA512
} ac_hash_algo_t;

typedef enum ac_misc_ppb_factor_t {
	AC_MISC_PPB_BYTES = 0,
	AC_MISC_PPB_KBYTES,
	AC_MISC_PPB_MBYTES,
	AC_MISC_PPB_GBYTES,
	AC_MISC_PPB_TBYTES,
	AC_MISC_PPB_PBYTES,
	AC_MISC_PPB_EBYTES
} ac_misc_ppb_factor_t;

typedef enum ac_si_units_t {
	AC_SI_UNITS_NO = 0,
	AC_SI_UNITS_YES
} ac_si_units_t;

typedef struct ac_btree_t {
	void *rootp;

	int (*compar)(const void *, const void *);
	void (*free_node)(void *nodep);
} ac_btree_t;

typedef struct ac_cqueue_t {
	void **queue;
	size_t front;
	size_t rear;
	size_t count;
	size_t size;
	bool dyn_size;
	bool overwrite;

	void (*free_item)(void *item);
} ac_cqueue_t;

typedef struct ac_geo_t {
	ac_geo_ellipsoid_t ref;
	double lat;
	double lon;
	double alt;
	double bearing;
	double easting;
	double northing;
} ac_geo_t;

typedef struct ac_geo_dms_t {
	int degrees;
	int minutes;
	double seconds;
} ac_geo_dms_t;

typedef struct ac_htable_t {
	struct ac_slist_t **buckets;
	unsigned long count;

	u32 (*hash_func)(const void *key);
	int (*key_cmp)(const void *a, const void *b);
	void (*free_key_func)(void *ptr);
	void (*free_data_func)(void *ptr);
} ac_htable_t;

typedef struct ac_jsonw_t {
	char *str;
	size_t len;
	size_t allocated;
	u8 depth;
	bool skip_tabs;
	char *indenter;
} ac_jsonw_t;

typedef struct ac_misc_ppb_t {
	ac_misc_ppb_factor_t factor;
	const char *prefix;

	union {
		u16 v_u16;
		float v_float;
	} value;
} ac_misc_ppb_t;

typedef struct ac_quark_t {
	ac_btree_t *mapping;
	int last;

	void (*free_func)(void *ptr);
} ac_quark_t;

typedef struct ac_slist_t {
	void *data;

	struct ac_slist_t *next;
} ac_slist_t;

#pragma GCC visibility push(default)
void *ac_btree_new(int (*compar)(const void *, const void *),
		   void (*free_node)(void *nodep));
void ac_btree_foreach(ac_btree_t *tree, void (*action)(const void *nodep,
					      const VISIT which,
					      const int depth));
void *ac_btree_lookup(ac_btree_t *tree, const void *key);
void *ac_btree_add(ac_btree_t *tree, const void *key);
void *ac_btree_remove(ac_btree_t *tree, const void *key);
void ac_btree_destroy(ac_btree_t *tree);

ac_cqueue_t *ac_cqueue_new(size_t size, void (*free_item)(void *item),
			   int flags);
int ac_cqueue_push(ac_cqueue_t *cqueue, void *item);
void *ac_cqueue_pop(ac_cqueue_t *cqueue);
void ac_cqueue_foreach(const ac_cqueue_t *cqueue,
		       void (*action)(void *item, void *data),
		       void *user_data);
bool ac_cqueue_is_empty(const ac_cqueue_t *cqueue);
size_t ac_cqueue_nr_items(const ac_cqueue_t *cqueue);
void ac_cqueue_destroy(ac_cqueue_t *cqueue);

bool ac_fs_is_posix_name(const char *name);
int ac_fs_mkdir_p(const char *path);
ssize_t ac_fs_copy(const char *from, const char *to, int flags);

void ac_geo_dd_to_dms(double degrees, ac_geo_dms_t *dms);
double ac_geo_dms_to_dd(const ac_geo_dms_t *dms);
double ac_geo_haversine(const ac_geo_t *from, const ac_geo_t *to);
void ac_geo_vincenty_direct(const ac_geo_t *from, ac_geo_t *to,
			    double distance);
void ac_geo_bng_to_lat_lon(ac_geo_t *geo);
void ac_geo_lat_lon_to_bng(ac_geo_t *geo);

ac_htable_t *ac_htable_new(u32 (*hash_func)(const void *key),
			   int (*key_cmp)(const void *a, const void *b),
			   void (*free_key_func)(void *key),
			   void (*free_data_func)(void *data));
void ac_htable_insert(ac_htable_t *htable, void *key, void *data);
bool ac_htable_remove(ac_htable_t *htable, void *key);
void *ac_htable_lookup(const ac_htable_t *htable, const void *key);
void ac_htable_foreach(ac_htable_t *htable,
		       void (*action)(void *key, void *value, void *user_data),
		       void *user_data);
void ac_htable_destroy(ac_htable_t *htable);

ac_jsonw_t *ac_jsonw_init(void);
void ac_jsonw_set_indenter(ac_jsonw_t *json, const char *indenter);
void ac_json_add_str(ac_jsonw_t *json, const char *name, const char *value);
void ac_json_add_int(ac_jsonw_t *json, const char *name, s64 value);
void ac_json_add_float(ac_jsonw_t *json, const char *name, double value);
void ac_json_add_bool(ac_jsonw_t *json, const char *name, bool value);
void ac_json_add_null(ac_jsonw_t *json, const char *name);
void ac_json_add_str_or_null(ac_jsonw_t *json, const char *name,
			     const char *value);
void ac_json_add_array(ac_jsonw_t *json, const char *name);
void ac_json_end_array(ac_jsonw_t *json);
void ac_json_add_object(ac_jsonw_t *json, const char *name);
void ac_json_end_object(ac_jsonw_t *json);
void ac_json_end(ac_jsonw_t *json);
void ac_json_free(ac_jsonw_t *json);
size_t ac_json_len(const ac_jsonw_t *json);
const char *ac_json_get(const ac_jsonw_t *json);

void ac_misc_ppb(u64 bytes, ac_si_units_t si, ac_misc_ppb_t *ppb);
char *ac_misc_passcrypt(const char *pass, ac_hash_algo_t hash_type,
			ac_crypt_data_t *data);
u32 ac_hash_func_ptr(const void *key);
u32 ac_hash_func_str(const void *key);
u32 ac_hash_func_u32(const void *key);
int ac_cmp_ptr(const void *a, const void *b);
int ac_cmp_str(const void *a, const void *b);
int ac_cmp_u32(const void *a, const void *b);

u16 ac_net_port_from_sa(const struct sockaddr *sa);
int ac_net_inet_pton(const char *src, void *dst);
const char *ac_net_inet_ntop(const void *src, char *dst, socklen_t size);
int ac_net_ns_lookup_by_host(const struct addrinfo *hints, const char *node,
			     bool (*ac_ns_lookup_cb)(const struct addrinfo *ai,
						     const char *res));
int ac_net_ns_lookup_by_ip(const struct addrinfo *hints, const char *node,
			   bool (*ac_ns_lookup_cb)(const struct addrinfo *ai,
						   const char *res));
bool ac_net_ipv4_isin(const char *network, u8 cidr, const char *addr);
bool ac_net_ipv6_isin(const char *network, u8 prefixlen, const char *addr);

void ac_quark_init(ac_quark_t *quark, void (*free_func)(void *ptr));
int ac_quark_from_string(ac_quark_t *quark, const char *str);
const char *ac_quark_to_string(ac_quark_t *quark, int id);
void ac_quark_destroy(ac_quark_t *quark);

ac_slist_t *ac_slist_last(ac_slist_t *list);
long ac_slist_len(ac_slist_t *list);
void ac_slist_add(ac_slist_t **list, void *data);
void ac_slist_preadd(ac_slist_t **list, void *data);
bool ac_slist_remove(ac_slist_t **list, void *data, void (*free_data)
							 (void *data));
bool ac_slist_remove_nth(ac_slist_t **list, int n, void (*free_data)
							(void *data));
bool ac_slist_remove_custom(ac_slist_t **list, void *data,
			    int (*compar)(const void *a, const void *b),
			    void (*free_data)(void *data));
void ac_slist_reverse(ac_slist_t **list);
ac_slist_t *ac_slist_find(ac_slist_t *list, const void *data);
ac_slist_t *ac_slist_find_custom(ac_slist_t *list, const void *data,
				 int (*compar)(const void *a, const void *b));
void *ac_slist_nth_data(ac_slist_t *list, int n);
void ac_slist_foreach(ac_slist_t *list, void (*action)(void *item, void *data),
		      void *user_data);
void ac_slist_destroy(ac_slist_t **list, void (*free_data)(void *data));

void ac_str_freev(char **stringv);
char **ac_str_split(const char *string, int delim, int flags);
char *ac_str_chomp(char *string);
char *ac_str_substr(const char *src, void *dest, size_t dest_size, int start,
		    int len);
int ac_str_levenshtein(const char *s, const char *t);

double ac_time_tspec_diff(struct timespec *delta, const struct timespec *end,
			  const struct timespec *start);
void ac_time_secs_to_hms(long total, int *hours, int *minutes, int *seconds);
int ac_time_nsleep(u64 period);
#pragma GCC visibility pop

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _LIBAC_H_ */
