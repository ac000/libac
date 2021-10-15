/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * platform/freebsd/fallocate.c - dummy fallocate(2)
 *
 * Copyright (C) 2021		Andrew Clayton <andrew@digital-domain.net>
 */

#include <sys/types.h>
#include <errno.h>

int fallocate(int fd __unused, int mode __unused, off_t offset __unused,
	      off_t len __unused)
{
	errno = EOPNOTSUPP;
	return -1;
}
