/*
 * ac_misc.c - Miscellaneous functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 *
 * See COPYING
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>

#include "include/libac.h"

static u16 K;
static u32 M;
static u32 G;
static u64 T;
static u64 P;
static u64 E;

static const char *KS;
static const char *MS;
static const char *GS;
static const char *TS;
static const char *PS;
static const char *ES;

static void set_si_units(bool si)
{
	if (!si) {
		K = 1024;

		KS = "KiB";
		MS = "MiB";
		GS = "GiB";
		TS = "TiB";
		PS = "PiB";
		ES = "EiB";
	} else {
		K = 1000;

		KS = "KB";
		MS = "MB";
		GS = "GB";
		TS = "TB";
		PS = "PB";
		ES = "EB";
	}

	M = K * K;
	G = M * K;
	T = (u64)G * K;
	P = (u64)T * K;
	E = (u64)P * K;
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
	if (si == AC_SI_UNITS_YES)
		set_si_units(true);
	else
		set_si_units(false);

        if (bytes < K) {
		ppb->factor = AC_MISC_PPB_BYTES;
		snprintf(ppb->prefix, sizeof(ppb->prefix), "bytes");
		ppb->value.v_u16 = bytes;
	} else if (bytes < M) {
		ppb->factor = AC_MISC_PPB_KBYTES;
		snprintf(ppb->prefix, sizeof(ppb->prefix), "%s", KS);
		ppb->value.v_float = (float)bytes / K;
	} else if (bytes < G) {
		ppb->factor = AC_MISC_PPB_MBYTES;
		snprintf(ppb->prefix, sizeof(ppb->prefix), "%s", MS);
		ppb->value.v_float = (float)bytes / M;
	} else if (bytes < T) {
		ppb->factor = AC_MISC_PPB_GBYTES;
		snprintf(ppb->prefix, sizeof(ppb->prefix), "%s", GS);
		ppb->value.v_float = (float)bytes / G;
	} else if (bytes < P) {
		ppb->factor = AC_MISC_PPB_TBYTES;
		snprintf(ppb->prefix, sizeof(ppb->prefix), "%s", TS);
		ppb->value.v_float = (float)bytes / T;
	} else if (bytes < E) {
		ppb->factor = AC_MISC_PPB_PBYTES;
		snprintf(ppb->prefix, sizeof(ppb->prefix), "%s", PS);
		ppb->value.v_float = (float)bytes / P;
	} else {
		ppb->factor = AC_MISC_PPB_EBYTES;
		snprintf(ppb->prefix, sizeof(ppb->prefix), "%s", ES);
		ppb->value.v_float = (float)bytes / E;
	}
}
