/*
 * test.c - Test harness for libac
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#define _GNU_SOURCE			/* strdup(3), struct addrinfo */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

#include "include/libac.h"

struct tnode {
	int key;
	void *data;
};

static void free_tnode(void *data)
{
	struct tnode *tn = data;

	if (!tn)
		return;

	free(tn->data);
	free(tn);
}

static void print_node(const void *data, const VISIT which __always_unused,
		       const int depth __always_unused)
{
	struct tnode *tn = *(struct tnode **)data;

	printf("%d : %s\n", tn->key, (char *)tn->data);
}

static int compare(const void *pa, const void *pb)
{
	const struct tnode *tn1 = pa;
	const struct tnode *tn2 = pb;

	if (tn1->key < tn2->key)
		return -1;
	else if (tn1->key > tn2->key)
		return 1;
	else
		return 0;
}

static void btree_test(void)
{
	ac_btree_t *tree;
	struct tnode *tn;
	struct tnode stn;

	printf("*** %s\n", __func__);
	tree = ac_btree_new(compare, free_tnode);

	tn = malloc(sizeof(struct tnode));
	tn->key = 1;
	tn->data = strdup("Hello");
	ac_btree_add(tree, tn);

	tn = malloc(sizeof(struct tnode));
	tn->key = 2;
	tn->data = strdup("World");
	ac_btree_add(tree, tn);

	stn.key = 1;
	tn = ac_btree_lookup(tree, &stn);
	printf("Found tnode: %d - %s\n", tn->key, (char *)tn->data);

	stn.key = 2;
	tn = ac_btree_lookup(tree, &stn);
	printf("Found tnode: %d - %s\n", tn->key, (char *)tn->data);

	ac_btree_foreach(tree, print_node);

	ac_btree_remove(tree, &stn);
	ac_btree_destroy(tree);

	printf("*** %s\n\n", __func__);
}

static void byte_test(void)
{
	u8 byte = 0;

	printf("*** %s\n", __func__);

	printf("Bit 5 of byte is %sset\n", AC_BYTE_BIT_TST(byte, 4) ? "" :
			"not ");
	printf("Setting bit 5 of byte\n");
	AC_BYTE_BIT_SET(byte, 4);
	printf("Bit 5 of byte is %sset\n", AC_BYTE_BIT_TST(byte, 4) ? "" :
			"not ");
	printf("Flipping bit 5 of byte\n");
	AC_BYTE_BIT_FLP(byte, 4);
	printf("Bit 5 of byte is %sset\n", AC_BYTE_BIT_TST(byte, 4) ? "" :
			"not ");
	printf("Flipping bit 5 of byte\n");
	AC_BYTE_BIT_FLP(byte, 4);
	printf("Bit 5 of byte is %sset\n", AC_BYTE_BIT_TST(byte, 4) ? "" :
			"not ");
	printf("Clearing bit 5 of byte\n");
	AC_BYTE_BIT_CLR(byte, 4);
	printf("Bit 5 of byte is %sset\n", AC_BYTE_BIT_TST(byte, 4) ? "" :
			"not ");

	byte = 0x24;
	printf("Lower nibble of byte(0x24) : %hhu\n",
			AC_BYTE_NIBBLE_LOW(byte));
	printf("Upper nibble of byte(0x24) : %hhu\n",
                        AC_BYTE_NIBBLE_HIGH(byte));

	printf("*** %s\n\n", __func__);
}

struct queue_data {
	char *name;
	int item;
};

static void print_queue_item(void *item, void *data __always_unused)
{
	struct queue_data *qd = item;

	printf("\titem %d : %s\n", qd->item, qd->name);
}

static void free_queue_item(void *item)
{
	struct queue_data *qd = item;

	printf("Freeing item %d : %s\n", qd->item, qd->name);
	free(qd->name);
	free(qd);
}

static void cqueue_test(void)
{
	ac_cqueue_t *queue = ac_cqueue_new(3, free_queue_item, 0);
	struct queue_data *qd;
	int i;

	printf("*** %s\n", __func__);

	printf("The queue is %sempty\n", ac_cqueue_is_empty(queue) ? "" :
			"not ");

	qd = malloc(sizeof(struct queue_data));
	qd->name = strdup("mercury");
	qd->item = 0;
	printf("Pushing item 0 into the queue\n");
	ac_cqueue_push(queue, qd);

	qd = malloc(sizeof(struct queue_data));
	qd->name = strdup("venus");
	qd->item = 1;
	printf("Pushing item 1 into the queue\n");
	ac_cqueue_push(queue, qd);

	qd = malloc(sizeof(struct queue_data));
	qd->name = strdup("earth");
	qd->item = 2;
	printf("Pushing item 2 into the queue\n");
	ac_cqueue_push(queue, qd);
	printf("There are %zu items in the queue :-\n",
			ac_cqueue_nr_items(queue));
	ac_cqueue_foreach(queue, print_queue_item, NULL);

	printf("Popping item 0 from the queue\n");
	qd = ac_cqueue_pop(queue);
	free_queue_item(qd);
	ac_cqueue_foreach(queue, print_queue_item, NULL);

	qd = malloc(sizeof(struct queue_data));
	qd->name = strdup("mars");
	qd->item = 3;
	printf("Pushing item 3 into the queue\n");
	ac_cqueue_push(queue, qd);
	ac_cqueue_foreach(queue, print_queue_item, NULL);

	printf("Popping item 1 from the queue\n");
	qd = ac_cqueue_pop(queue);
	printf("Pushing item 1 back into the queue\n");
	ac_cqueue_push(queue, qd);
	ac_cqueue_foreach(queue, print_queue_item, NULL);

	printf("The queue is %sempty\n", ac_cqueue_is_empty(queue) ? "" :
			"not ");
	printf("Destroying queue\n");
	ac_cqueue_destroy(queue);

	printf("Creating new dynamically sized queue\n");
	queue = ac_cqueue_new(0, NULL, 0);
	printf("Adding 33 numbers to the queue, 1..33\n");
	for (i = 1; i < 34; i++)
		ac_cqueue_push(queue, (void *)(long)i);
	printf("Reading back numbers: ");
	for (i = 1; i < 34; i++)
		printf("%d ", (int)(long)ac_cqueue_pop(queue));
	printf("\n");
	printf("Destroying queue\n");
	ac_cqueue_destroy(queue);

	printf("Overwrite queue entries in a queue of 2\n");
	queue = ac_cqueue_new(2, free_queue_item, AC_CQUEUE_OVERWRITE);
	qd = malloc(sizeof(struct queue_data));
	qd->name = strdup("mercury");
	qd->item = 1;
	printf("Pushing item %d (%s) into the queue\n", qd->item, qd->name);
	ac_cqueue_push(queue, qd);

	qd = malloc(sizeof(struct queue_data));
	qd->name = strdup("venus");
	qd->item = 2;
	printf("Pushing item %d (%s) into the queue\n", qd->item, qd->name);
	ac_cqueue_push(queue, qd);

	qd = malloc(sizeof(struct queue_data));
	qd->name = strdup("earth");
	qd->item = 3;
	printf("Pushing item %d (%s) into the queue\n", qd->item, qd->name);
	ac_cqueue_push(queue, qd);

	qd = ac_cqueue_pop(queue);
	printf("Popping item 1 from the queue\n");
	free_queue_item(qd);

	qd = malloc(sizeof(struct queue_data));
	qd->name = strdup("mars");
	qd->item = 4;
	printf("Pushing item %d (%s) into the queue\n", qd->item, qd->name);
	ac_cqueue_push(queue, qd);

	ac_cqueue_foreach(queue, print_queue_item, NULL);
	ac_cqueue_destroy(queue);

	printf("*** %s\n\n", __func__);
}

static void fs_test(void)
{
	int ret;
	const char name1[] = "-bad name";
	const char name2[] = "a_good_name";

	printf("*** %s\n", __func__);

	if (ac_fs_is_posix_name(name1))
		printf("Good : ");
	else
		printf("Bad  : ");
	printf("'%s'\n", name1);

	if (ac_fs_is_posix_name(name2))
		printf("Good : ");
	else
		printf("Bad  : ");
	printf("'%s'\n", name2);

	printf("Creating direstory : /tmp/libac/mkdir_p/test\n");
	ac_fs_mkdir_p("/tmp/libac/mkdir_p/test");

	unlink("/tmp/services");
	printf("Copying /etc/services -> /tmp/services\n");
	ac_fs_copy("/etc/services", "/tmp/services", 0);
	printf("Copying /etc/services -> /tmp/services (overwrite)\n");
	ac_fs_copy("/etc/services", "/tmp/services", AC_FS_COPY_OVERWRITE);
	printf("Copying /etc/services -> /tmp/services\n");
	ret = ac_fs_copy("/etc/services", "/tmp/services", 0);
	if (ret == -1)
		perror("ac_fs_copy");

	printf("*** %s\n\n", __func__);
}

static void geo_test(void)
{
	ac_geo_t from;
	ac_geo_t to;
	ac_geo_dms_t dms;

	printf("*** %s\n", __func__);

	memset(&from, 0, sizeof(ac_geo_t));
	memset(&to, 0, sizeof(ac_geo_t));

	from.ref = AC_GEO_EREF_WGS84;
	from.lat = 57.138386;
	from.lon = -4.668295;
	to.lat = 57.409441;
	to.lon = -4.334497;

	printf("Distance from -> to : %f\n", ac_geo_haversine(&from, &to));
	from.bearing = 45.0;
	ac_geo_vincenty_direct(&from, &to, 40000.0);
	printf("(%f, %f) -> (%f, %f)\n", from.lat, from.lon, to.lat, to.lon);
	ac_geo_dd_to_dms(38.8897, &dms);
	printf("38.8897° -> %d° %d′ %g″\n", dms.degrees, dms.minutes,
			dms.seconds);
	printf("38° 53′ 22.92″ -> %g°\n", ac_geo_dms_to_dd(&dms));
	memset(&from, 0, sizeof(ac_geo_t));
	from.easting = 216677;
	from.northing = 771282;
	from.ref = AC_GEO_EREF_WGS84;
	ac_geo_bng_to_lat_lon(&from);
	printf("(216677 E, 771282 N) -> (%f°, %f°)\n", from.lat, from.lon);
	from.alt = 0;
	ac_geo_lat_lon_to_bng(&from);
	printf("(%f°, %f°) -> (%.0f E, %.0f N)\n", from.lat, from.lon,
			from.easting, from.northing);

	printf("*** %s\n\n", __func__);
}

static void htable_print_entry(void *key, void *data,
			       void *user_data __always_unused)
{
	printf("%s -> %s\n", (char *)key, (char *)data);
}

static void htable_test(void)
{
	ac_htable_t *htable;
	char *data;

	printf("*** %s\n", __func__);

	printf("New hash table with static string keys/data\n");
	htable = ac_htable_new(ac_hash_func_str, ac_cmp_str, NULL, NULL);
	ac_htable_insert(htable, "::1", "localhost");
	ac_htable_insert(htable, "fe80::/10", "link-local");
	printf("There are %lu item(s) in the hash table\n", htable->count);
	data = ac_htable_lookup(htable, "::1");
	printf("lookup: ::1 -> %s\n", data);
	printf("Removing an item\n");
	ac_htable_remove(htable, "::1");
	printf("There are %lu item(s) in the hash table\n", htable->count);
	printf("Destoying hash table\n");
	ac_htable_destroy(htable);

	printf("New hash table with dynamically allocated string keys/data\n");
	htable = ac_htable_new(ac_hash_func_str, ac_cmp_str, free, free);
	ac_htable_insert(htable, strdup("::1"), strdup("localhost"));
	ac_htable_insert(htable, strdup("fe80::/10"), strdup("link-loca"));
	printf("There are %lu item(s) in the hash table\n", htable->count);
	data = ac_htable_lookup(htable, "fe80::/10");
	printf("lookup: fe80::/10 -> %s\n", data);
	printf("Re-inserting previous entry\n");
	ac_htable_insert(htable, strdup("fe80::/10"), strdup("link-local"));
	printf("There are %lu item(s) in the hash table\n", htable->count);
	printf("All entries :-\n");
	ac_htable_foreach(htable, htable_print_entry, NULL);
	printf("Removing an item\n");
	ac_htable_remove(htable, "fe80::/10");
	printf("There are %lu item(s) in the hash table\n", htable->count);
	printf("Destoying hash table\n");
	ac_htable_destroy(htable);

	printf("New hash table with static int keys/dynamic data\n");
	htable = ac_htable_new(ac_hash_func_ptr, ac_cmp_ptr, NULL, free);
	ac_htable_insert(htable, AC_LONG_TO_PTR(1), strdup("one"));
	ac_htable_insert(htable, AC_LONG_TO_PTR(2), strdup("two"));
	printf("There are %lu item(s) in the hash table\n", htable->count);
	data = ac_htable_lookup(htable, AC_LONG_TO_PTR(2));
	printf("lookup: 2 -> %s\n", data);
	printf("Destoying hash table\n");
	ac_htable_destroy(htable);

	printf("*** %s\n\n", __func__);
}

static void misc_test(void)
{
	ac_misc_ppb_t ppb;
	int bytes = 14568264;
	u64 bytes2 = 7375982736;
	ac_crypt_data_t data = { .initialized = 0 };
	const char *pass = "asdfghjk";

	printf("*** %s\n", __func__);

	ac_misc_ppb(bytes, AC_SI_UNITS_YES, &ppb);
	if (ppb.factor == AC_MISC_PPB_BYTES)
		printf("%d bytes : %hu bytes\n", bytes, ppb.value.v_u16);
	else
		printf("%d bytes : %.2f %s\n", bytes, ppb.value.v_float,
				ppb.prefix);

	ac_misc_ppb(bytes2, AC_SI_UNITS_NO, &ppb);
	if (ppb.factor == AC_MISC_PPB_BYTES)
		printf("%" PRIu64 " bytes : %hu bytes\n", bytes2,
				ppb.value.v_u16);
	else
		printf("%" PRIu64 " bytes : %.2f %s\n", bytes2,
				ppb.value.v_float, ppb.prefix);

	printf("%s -> %s\n", pass, ac_misc_passcrypt(pass, AC_HASH_ALGO_MD5,
				&data));
	printf("%s -> %s\n", pass, ac_misc_passcrypt(pass, AC_HASH_ALGO_SHA256,
				&data));
	printf("%s -> %s\n", pass, ac_misc_passcrypt(pass, AC_HASH_ALGO_SHA512,
				&data));

	printf("*** %s\n\n", __func__);
}

static bool ns_lookup_cb(const struct addrinfo *ai __always_unused,
			 const char *res)
{
	printf("\t%s\n", res);

	return true;
}

static void net_test(void)
{
	int err;
	int i;
	struct addrinfo hints;
	struct addrinfo *res;
	struct sockaddr_in6 in6 = { .sin6_family = AF_INET6,
				    .sin6_port = htons(1976),
				    .sin6_flowinfo = 0 };
	struct sockaddr_in in4 = { .sin_family = AF_INET,
				   .sin_port = htons(2121),
				   .sin_addr.s_addr = 0 };
	char addrp[INET6_ADDRSTRLEN];
	u8 addrn[sizeof(struct in6_addr)];
	const struct {
		const char *addr;
		const char *network;
		const u8 prefixlen;
	} nets[] = {
		{ "192.168.1.10", "192.168.2.0", 24 },
		{ "2001:db8:dead:beef::f00d", "2001:db8:dead:beef::", 64 }
	};

	printf("*** %s\n", __func__);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	printf("localhost ->\n");
	err = ac_net_ns_lookup_by_host(&hints, "localhost", ns_lookup_cb);
	if (err)
		perror("ac_net_ns_lookup_by_host");

	printf("::1 ->\n");
	err = ac_net_ns_lookup_by_ip(&hints, "::1", ns_lookup_cb);
	if (err)
		perror("ac_net_ns_lookup_by_ip");

	printf("Port : %hu\n", ac_net_port_from_sa((struct sockaddr *)&in6));
	printf("Port : %hu\n", ac_net_port_from_sa((struct sockaddr *)&in4));

	getaddrinfo("www.google.com", "443", &hints, &res);
	printf("www.google.com -> %s -> ", ac_net_inet_ntop(
				res->ai_addr, addrp, INET6_ADDRSTRLEN));
	freeaddrinfo(res);
	ac_net_inet_pton(addrp, addrn);
	for (i = 0; i < 15; i += 2) {
		printf("%02x%02x", addrn[i], addrn[i + 1]);
		if (i < 14)
			 printf(":");
	}
	printf("\n");

	printf("%s is%sin %s/%hhu\n",
			nets[0].addr,
			ac_net_ipv4_isin(nets[0].network, nets[0].prefixlen,
					 nets[0].addr) ? " " : " NOT ",
			nets[0].network, nets[0].prefixlen);
	printf("%s is%sin %s/%hhu\n",
			nets[1].addr,
			ac_net_ipv6_isin(nets[1].network, nets[1].prefixlen,
					 nets[1].addr) ? " " : " NOT ",
			nets[1].network, nets[1].prefixlen);

	printf("*** %s\n\n", __func__);
}

static void quark_test(void)
{
	ac_quark_t quark;

	printf("*** %s\n", __func__);

	ac_quark_init(&quark, NULL);

	printf("Hello -> %d\n", ac_quark_from_string(&quark, "Hello"));
	printf("World -> %d\n", ac_quark_from_string(&quark, "World"));
	printf("0 -> %s\n", ac_quark_to_string(&quark, 0));
	printf("1 -> %s\n", ac_quark_to_string(&quark, 1));
	printf("2 -> %s\n", ac_quark_to_string(&quark, 2));

	ac_quark_destroy(&quark);

	printf("*** %s\n\n", __func__);
}

struct list_data {
	int val;
};

static void slist_setval(void *data, void *user_data __always_unused)
{
	struct list_data *ld = data;

	ld->val = -1;
}

static void slist_print(void *data, void *user_data __always_unused)
{
	printf("val : %d\n", ((struct list_data *)data)->val);
}

static int slist_cmp(const void *p1, const void *p2)
{
	const struct list_data *d1 = (const struct list_data *)p1;
	const struct list_data *d2 = (const struct list_data *)p2;

	if (d1->val < d2->val)
		return -1;
	else if (d1->val > d2->val)
		return 1;
	else
		return 0;
}

static int slist_qcmp(const void *p1, const void *p2)
{
	return slist_cmp(*(const void **)p1, *(const void **)p2);
}

static ac_slist_t *sort_list(ac_slist_t *list)
{
	ac_slist_t *slist = NULL;
	struct list_data **array;
	long i = 0;
	long j;

	array = malloc(ac_slist_len(list) * sizeof(struct list_data *));
	while (list) {
		array[i] = malloc(sizeof(struct list_data));
		array[i++]->val = ((struct list_data *)list->data)->val;
		list = list->next;
	}

	qsort(array, i, sizeof(struct list_data *), slist_qcmp);

	for (j = 0; j < i; j++) {
		struct list_data *ld = array[j];
		ac_slist_add(&slist, ld);
	}
	free(array);

	printf("Sorted list\n");
	ac_slist_foreach(slist, slist_print, NULL);

	return slist;
}

static void slist_test(void)
{
	struct list_data *ld;
	ac_slist_t *mylist = NULL;
	ac_slist_t *slist;
	ac_slist_t *p;

	printf("*** %s\n", __func__);

	printf("Adding items\n");
	ld = malloc(sizeof(struct list_data));
	ld->val = 42;
	ac_slist_add(&mylist, ld);

	ld = malloc(sizeof(struct list_data));
	ld->val = 62;
	ac_slist_preadd(&mylist, ld);

	ld = malloc(sizeof(struct list_data));
	ld->val = 32;
	ac_slist_preadd(&mylist, ld);

	ld = malloc(sizeof(struct list_data));
	ld->val = 72;
	ac_slist_preadd(&mylist, ld);

	ld = malloc(sizeof(struct list_data));
	ld->val = 52;
	ac_slist_preadd(&mylist, ld);

	printf("List has %ld items\n", ac_slist_len(mylist));

	printf("Manual list traversal\n");
	p = mylist;
	while (p) {
		printf("val : %d\n", ((struct list_data *)p->data)->val);
		p = p->next;
	}
	printf("\n");

	slist = sort_list(mylist);
	printf("Freeing... back to original list\n\n");
	ac_slist_destroy(&slist, free);

	printf("ac_slist_foreach() - Dump list\n");
	ac_slist_foreach(mylist, slist_print, NULL);
	printf("Reverse\n");
	ac_slist_reverse(&mylist);
	ac_slist_foreach(mylist, slist_print, NULL);
	printf("Remove (52)\n");
	ac_slist_remove(&mylist, ld, free);
	ac_slist_foreach(mylist, slist_print, NULL);
	printf("Reverse\n");
	ac_slist_reverse(&mylist);
	ac_slist_foreach(mylist, slist_print, NULL);
	ld = ac_slist_nth_data(mylist, 1);
	if (ld)
		printf("1 -> %d\n", ld->val);
	ld = ac_slist_nth_data(mylist, 3);
	if (!ld)
		printf("3 -> Not Found\n");
	printf("Find element data with value 42. ");
	ld = malloc(sizeof(struct list_data));
	ld->val = 42;
	p = ac_slist_find_custom(mylist, ld, slist_cmp);
	if (p)
		printf("Found.\n");
	else
		printf("Not Found.\n");
	free(ld);
	printf("Remove 2nd element\n");
	ac_slist_remove_nth(&mylist, 1, free);
	printf("Remove element with value of 62\n");
	ld = malloc(sizeof(struct list_data));
	ld->val = 62;
	ac_slist_remove_custom(&mylist, ld, slist_cmp, free);
	free(ld);
	ac_slist_foreach(mylist, slist_print, NULL);
	printf("ac_slist_foreach() - set vals to -1\n");
	ac_slist_foreach(mylist, slist_setval, NULL);
	ac_slist_foreach(mylist, slist_print, NULL);
	printf("Destroy slist\n");
	ac_slist_destroy(&mylist, free);

	printf("*** %s\n\n", __func__);
}

static void str_test(void)
{
	char str1[] = "Hello World\r\n";
	char str2[] = "Hello World\r\n";
	const char str3[] = "field0,field1,field2";
	const char str4[] = "field3";
	char dst[32];
	char **fields;
	char **pp;

	printf("*** %s\n", __func__);

	fields = ac_str_split(str3, ',', 0);
	for (pp = fields; *pp != NULL; pp++)
		printf("ac_str_split (str3): %s\n", *pp);
	ac_str_freev(fields);

	fields = ac_str_split(str4, ',', AC_STR_SPLIT_ALWAYS);
	for (pp = fields; *pp != NULL; pp++)
		printf("ac_str_split (str4): %s\n", *pp);
	ac_str_freev(fields);

	fields = ac_str_split(str4, ',', AC_STR_SPLIT_STRICT);
	if (!fields[0])
		printf("ac_str_split (str4): No delimiters found "
				"(AC_STR_SPLIT_STRICT)\n");
	ac_str_freev(fields);

	printf("ac_str_chomp  : %s\n", ac_str_chomp(str1));
	printf("ac_str_substr : %s\n", ac_str_substr(str2, dst, 32, 3, 4));

	printf("Levenshtein diistance between 'str' and 'strings' is %d\n",
			ac_str_levenshtein("str", "strings"));

	printf("*** %s\n\n", __func__);
}

static void time_test(void)
{
	int i;
	int h;
	int m;
	int s;
	struct timespec delta;
	const struct {
		struct timespec start;
		struct timespec end;
	} times[] = {
		{ { 100, 899972154 }, { 101, 15534107 } },
		{ { 100, 250000000 }, { 100, 700000000 } },
		{ { 100, 500000000 }, { 102, 350000000 } },
		{ { 150, 250000000 }, { 151, 250000000 } },
		{ { 150, 500000000 }, { 140, 0 } },
		{ { 150, 0 }, { 140, 500000000 } },
		{ { 0, 0 }, { 0, 0} }
	};

	printf("*** %s\n", __func__);

	for (i = 0; times[i].start.tv_nsec != 0; i++) {
		double et;

		et = ac_time_tspec_diff(&delta, &times[i].end, &times[i].start);
		printf("Time difference is %f seconds\n", et);
	}

	ac_time_secs_to_hms(3675, &h, &m, &s);
	printf("%lds = %dh %dm %ds\n", 3675L, h, m, s);

	printf("*** %s\n\n", __func__);
}

int main(void)
{
	printf("**** Testing libac version %d.%d.%d ****\n",
			LIBAC_MAJOR_VERSION, LIBAC_MINOR_VERSION,
			LIBAC_MICRO_VERSION);

	btree_test();
	byte_test();
	cqueue_test();
	fs_test();
	geo_test();
	htable_test();
	misc_test();
	net_test();
	quark_test();
	slist_test();
	str_test();
	time_test();

	exit(EXIT_SUCCESS);
}
