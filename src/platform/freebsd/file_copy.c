/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * platform/freebsd/file_copy.c - Copy from src fd to dest fd
 *
 * Copyright (C) 2021		Andrew Clayton <andrew@digital-domain.net>
 */

#include <stddef.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE	4096
ssize_t file_copy(int in_fd, int out_fd)
{
	ssize_t bytes_wrote;
	ssize_t total = 0;

	do {
		ssize_t bytes_read;
		char buf[BUF_SIZE];

		bytes_read = read(in_fd, buf, BUF_SIZE);
		if (bytes_read == -1)
			return -1;
		else if (bytes_read == 0)
			break;

		bytes_wrote = write(out_fd, buf, bytes_read);
		if (bytes_wrote == -1)
			return -1;

		total += bytes_wrote;
	} while (bytes_wrote > 0);

	return total;
}
