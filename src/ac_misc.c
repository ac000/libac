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
