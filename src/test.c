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
#include <inttypes.h>
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

static void print_node(const void *data, const VISIT which, const int depth)
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

struct queue_data {
	char *name;
	int item;
};

static void print_queue_item(void *item, void *data)
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
	ac_cqueue_t *queue = ac_cqueue_new(3, free_queue_item);
	struct queue_data *qd;

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
	ac_geo_t from = { 0 };
	ac_geo_t to = { 0 };
	ac_geo_dms_t dms;

	printf("*** %s\n", __func__);

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

static void misc_test(void)
{
	ac_misc_ppb_t ppb;
	int bytes = 14568264;
	u64 bytes2 = 7375982736;

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

	printf("*** %s\n\n", __func__);
}

static bool ns_lookup_cb(const struct addrinfo *ai, const char *res)
{
	printf("\t%s\n", res);

	return true;
}

static void net_test(void)
{
	int err;
	struct addrinfo hints;

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

static void slist_setval(void *data, void *user_data)
{
	struct list_data *ld = data;

	ld->val = -1;
}

static void slist_print(void *data, void *user_data)
{
	printf("val : %d\n", ((struct list_data *)data)->val);
}

static void slist_test(void)
{
	struct list_data *ld;
	ac_slist_t *mylist = NULL;
	ac_slist_t *p;

	printf("*** %s\n", __func__);

	printf("Adding items\n");
	ld = malloc(sizeof(struct list_data));
	ld->val = 42;
	ac_slist_add(&mylist, ld);

	ld = malloc(sizeof(struct list_data));
	ld->val = 52;
	ac_slist_preadd(&mylist, ld);

	ld = malloc(sizeof(struct list_data));
	ld->val = 62;
	ac_slist_preadd(&mylist, ld);

	ld = malloc(sizeof(struct list_data));
	ld->val = 72;
	ac_slist_preadd(&mylist, ld);

	printf("Manual list traversal\n");
	p = mylist;
	while (p) {
		printf("val : %d\n", ((struct list_data *)p->data)->val);
		p = p->next;
	}
	printf("\n");

	printf("ac_slist_foreach() - Dump list\n");
	ac_slist_foreach(mylist, slist_print, NULL);
	printf("Reverse\n");
	ac_slist_reverse(&mylist);
	ac_slist_foreach(mylist, slist_print, NULL);
	printf("Remove (72)\n");
	ac_slist_remove(&mylist, ld, free);
	ac_slist_foreach(mylist, slist_print, NULL);
	printf("Reverse\n");
	ac_slist_reverse(&mylist);
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
	char dst[32];

	printf("*** %s\n", __func__);

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
	struct _times {
		struct timespec start;
		struct timespec end;
	};
	static const struct _times times[] = {
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
	cqueue_test();
	fs_test();
	geo_test();
	misc_test();
	net_test();
	quark_test();
	slist_test();
	str_test();
	time_test();

	exit(EXIT_SUCCESS);
}
