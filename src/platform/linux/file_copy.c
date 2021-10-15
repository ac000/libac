/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * platform/linux/file_copy.c - Copy from src fd to dest fd
 *
 * Copyright (C) 2021		Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stddef.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define IO_SIZE (1024*1024 * 8ul)
ssize_t file_copy(int in_fd, int out_fd)
{
	ssize_t bytes_wrote;
	ssize_t total = 0;
	unsigned window = 0;

	do {
		bytes_wrote = sendfile(out_fd, in_fd, NULL, IO_SIZE);
		if (bytes_wrote == -1)
			return -1;
		total += bytes_wrote;

		/*
		 * Try not to blow the page cache. After each sendfile() we
		 * write out the data and then make sure the previous lot
		 * of data got written out and tell the kernel we no longer
		 * need those pages in memory any more for both the input
		 * and output files.
		 */
		sync_file_range(out_fd, window * IO_SIZE, IO_SIZE,
				SYNC_FILE_RANGE_WRITE);

		if (window == 0)
			goto incr_win;

		sync_file_range(out_fd, (window-1) * IO_SIZE, IO_SIZE,
				SYNC_FILE_RANGE_WAIT_BEFORE |
				SYNC_FILE_RANGE_WRITE |
				SYNC_FILE_RANGE_WAIT_AFTER);
		posix_fadvise(out_fd, (window-1) * IO_SIZE, IO_SIZE,
			      POSIX_FADV_DONTNEED);
		posix_fadvise(in_fd, (window-1) * IO_SIZE, IO_SIZE,
			      POSIX_FADV_DONTNEED);

incr_win:
		window++;
	} while (bytes_wrote > 0);

	return total;
}
