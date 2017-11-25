/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_misc.c - Miscellaneous functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <time.h>

#include "include/libac.h"

#define K(si)	((si) ? 1000 : 1024)
#define M(si)	((si) ? 1000*1000 : 1024*1024)
#define G(si)	((si) ? 1000*1000*1000 : 1024*1024*1024)
#define T(si)	((si) ? (u64)1000*1000*1000*1000 : (u64)1024*1024*1024*1024)
#define P(si)	((si) ? (u64)1000*1000*1000*1000*1000 : \
			(u64)1024*1024*1024*1024*1024)
#define E(si)	((si) ? (u64)1000*1000*1000*1000*1000*1000 : \
			(u64)1024*1024*1024*1024*1024*1024)

static void ppp_set_prefix(ac_si_units_t si, ac_misc_ppb_t *ppb)
{
	const char *pfx;

	switch (ppb->factor) {
	case AC_MISC_PPB_BYTES:
		pfx = "bytes";
		break;
	case AC_MISC_PPB_KBYTES:
		pfx = (si == AC_SI_UNITS_YES) ? "KB" : "KiB";
		break;
	case AC_MISC_PPB_MBYTES:
		pfx = (si == AC_SI_UNITS_YES) ? "MB" : "MiB";
		break;
	case AC_MISC_PPB_GBYTES:
		pfx = (si == AC_SI_UNITS_YES) ? "GB" : "GiB";
		break;
	case AC_MISC_PPB_TBYTES:
		pfx = (si == AC_SI_UNITS_YES) ? "TB" : "TiB";
		break;
	case AC_MISC_PPB_PBYTES:
		pfx = (si == AC_SI_UNITS_YES) ? "PB" : "PiB";
		break;
	case AC_MISC_PPB_EBYTES:
		pfx = (si == AC_SI_UNITS_YES) ? "EB" : "EiB";
		break;
	}

	ppb->prefix = pfx;
}

/**
 * ac_misc_ppb - pretty print bytes
 *
 * @bytes: The bytes value to pretty print
 * @si: Whether to use SI units or not, can be either; AC_SI_UNITS_NO or
 *      AC_SI_UNITS_YES
 * @ppb: A structure that is filled out with the result
 */
void ac_misc_ppb(u64 bytes, ac_si_units_t si, ac_misc_ppb_t *ppb)
{
        if (bytes < K(si)) {
		ppb->factor = AC_MISC_PPB_BYTES;
		ppb->value.v_u16 = bytes;
	} else if (bytes < M(si)) {
		ppb->factor = AC_MISC_PPB_KBYTES;
		ppb->value.v_float = (float)bytes / K(si);
	} else if (bytes < G(si)) {
		ppb->factor = AC_MISC_PPB_MBYTES;
		ppb->value.v_float = (float)bytes / M(si);
	} else if (bytes < T(si)) {
		ppb->factor = AC_MISC_PPB_GBYTES;
		ppb->value.v_float = (float)bytes / G(si);
	} else if (bytes < P(si)) {
		ppb->factor = AC_MISC_PPB_TBYTES;
		ppb->value.v_float = (float)bytes / T(si);
	} else if (bytes < E(si)) {
		ppb->factor = AC_MISC_PPB_PBYTES;
		ppb->value.v_float = (float)bytes / P(si);
	} else {
		ppb->factor = AC_MISC_PPB_EBYTES;
		ppb->value.v_float = (float)bytes / E(si);
	}

	ppp_set_prefix(si, ppb);
}

/**
 * ac_misc_passcrypt - wrapper around crypt_r(3)
 *
 * @pass: The password to crypt
 * @hash_type: The type of hash to pass to crypt_r(3)
 * @data: Used to store the crypted password, should not be free'd
 *
 * Returns:
 *
 * A poniter to the crypted password contained in @data
 */
char *ac_misc_passcrypt(const char *pass, ac_hash_algo_t hash_type,
			ac_crypt_data_t *data)
{
	const char salt_chars[64] =
		"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char salt[21];
	int i;

	memset(salt, 0, sizeof(salt));
	switch (hash_type) {
	case AC_HASH_ALGO_MD5:
		strcpy(salt, "$1$");
		break;
	case AC_HASH_ALGO_SHA256:
		strcpy(salt, "$5$");
		break;
	case AC_HASH_ALGO_SHA512:
		strcpy(salt, "$6$");
		break;
	default:
		return NULL;
	}

	for (i = 3; i < 19; i++) {
		long r;
		struct timespec tp;

		clock_gettime(CLOCK_REALTIME, &tp);
		srandom(tp.tv_nsec / 2);
		r = random() % 64; /* 0 - 63 */
		salt[i] = salt_chars[r];
	}
	salt[i] = '$';

	return crypt_r(pass, salt, data);
}

#define GOLDEN_MUL	0x61C88647	/* From the Linux kernel */
/**
 * ac_hash_func_str - create a hash value for a given string
 *
 * @key: The string/key to hash
 *
 * This is the Jenkins One At A Time Hash function
 *
 * This function is suitable for use in ac_htable_new()
 *
 * Returns:
 *
 * A u32 hash value
 */
u32 ac_hash_func_str(const void *key)
{
	size_t i = 0;
	u32 hash = 0;
	const char *k = key;
	size_t len = strlen(k);

	while (i != len) {
		hash += k[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}

/**
 * ac_hash_func_u32 - create a hash value for a given u32
 *
 * @key: The u32/key to hash
 *
 * This function is suitable for use in ac_htable_new()
 *
 * Returns:
 *
 * A u32 hash value
 */
u32 ac_hash_func_u32(const void *key)
{
	const u32 k = *(const u32 *)key;

	return k * GOLDEN_MUL;
}

/**
 * ac_hash_func_ptr - create a hash for a given pointer
 *
 * @key: The pointer/key to hash
 *
 * This function is suitable for use in ac_htable_new()
 *
 * Returns:
 *
 * A u32 hash value
 */
u32 ac_hash_func_ptr(const void *key)
{
	return (const long)key * GOLDEN_MUL;
}

/**
 * ac_cmp_ptr - compare two pointers
 *
 * @a: The first pointer to compare
 * @b: The second pointer to compare
 *
 * This function is suitable for use in ac_htable_new()
 *
 * Returns:
 *
 * An int, 0 if the pointers match, non-zero if they don't
 */
int ac_cmp_ptr(const void *a, const void *b)
{
	return !(a == b);
}

/**
 * ac_cmp_str - compare two strings
 *
 * @a: The first string to compare
 * @b: The second string to compare
 *
 * This function is suitable for use in ac_htable_new()
 *
 * Returns:
 *
 * An int, 0 if the strings match, non-zero if they don't
 */
int ac_cmp_str(const void *a, const void *b)
{
	const char *s1 = a;
	const char *s2 = b;

	return strcmp(s1, s2);
}

/**
 * ac_cmp_u32 - compare two u32's
 *
 * @a: The first u32 to compare
 * @b: The second u32 to compare
 *
 * This function is suitable for use in ac_htable_new()
 *
 * Returns:
 *
 * An int, 0 if the u32's match, non-zero if they don't
 */
int ac_cmp_u32(const void *a, const void *b)
{
	const u32 v1 = *(const u32 *)a;
	const u32 v2 = *(const u32 *)b;

	return !(v1 == v2);
}
