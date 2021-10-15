/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * platform/linux/gen_uuid.c - generate a version 4 UUID
 *
 * Copyright (C) 2021		Andrew Clayton <andrew@digital-domain.net>
 */

#include <stdio.h>
#include <sys/types.h>

#include "../../include/libac.h"

char *gen_uuid(char *buf)
{
	FILE *fp;
	size_t bytes_read;

	fp = fopen("/proc/sys/kernel/random/uuid", "re");
	if (!fp)
		return NULL;

	bytes_read = fread(buf, 1, AC_UUID4_LEN, fp);
	if (bytes_read < AC_UUID4_LEN) {
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	buf[AC_UUID4_LEN] = '\0';

	return buf;
}
