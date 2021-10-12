/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_fs.c - Filesystem related utility functions.
 *
 * Copyright (c) 2017, 2020 - 2021	Andrew Clayton
 *					<andrew@digital-domain.net>
 */

#define _GNU_SOURCE		/* strtok_r(3) & strdup(3) */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>

#include "include/libac.h"

/**
 * ac_fs_is_posix_name - checks if a filename follows POSIX guidelines
 *
 * @name: The name to check
 *
 * Returns:
 *
 * true if name follows the guidelines, false otherwise
 */
bool ac_fs_is_posix_name(const char *name)
{
	if (!name || *name == '\0' || *name == '-')
		return false;
	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
		return false;

	/*
	 * Check for valid POSIX filename characters as defined by:
         * http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap03.html#tag_03_281
	 */
	while (*name) {
		switch (*name++) {
		case 'A' ... 'Z':
		case 'a' ... 'z':
		case '0' ... '9':
		case '.':
		case '_':
		case '-':
			break;
		default:
			return false;
		}
	}

	return true;
}

/**
 * ac_fs_mkdir_p - mkdir with parents
 *
 * @dirfd: File descriptor for opened directory, can be ignored
 * @path: The path to make
 * @mode: Mode for the new directory (modified by the process's umask)
 *
 * Returns:
 *
 * 0 on success, -1 on failure check errno
 */
int ac_fs_mkdir_p(int dirfd, const char *path, mode_t mode)
{
	char *dir;
	char *ptr;
	char mdir[4096] = "\0";
	int ret = 0;

	if (strlen(path) >= sizeof(mdir)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	if (*path == '/')
		strcat(mdir, "/");

	dir = strdup(path);
	ptr = dir;
	for (;;) {
		char *token;

		token = strsep(&dir, "/");
		if (!token)
			break;

		strcat(mdir, token);
		ret = mkdirat(dirfd, mdir, mode);
		if (ret == -1 && errno != EEXIST) {
			ret = -1;
			break;
		}
		strcat(mdir, "/");
	}
	free(ptr);

	return ret;
}

#define IO_SIZE	(1024*1024 * 8ul)
/**
 * ac_fs_copy - copy a file
 *
 * @from: The source
 * @to: The destination
 * @flags: Can be 0 or AC_FS_COPY_OVERWRITE (to overwrite the destination)
 *
 * This is implemented using sendfile(2) and should work fine if your using
 * a 2.4 or >= 2.6.33 kernel.
 *
 * Returns:
 *
 * number of bytes written on success, -1 otherwise, check errno
 */
ssize_t ac_fs_copy(const char *from, const char *to, int flags)
{
	int ifd;
	int ofd;
	int ret;
	int oflags = O_EXCL;
	unsigned window = 0;
	ssize_t bytes_wrote = -1;
	struct stat sb;

	/* Check for unknown flags */
	if (flags & ~(AC_FS_COPY_OVERWRITE)) {
		errno = EINVAL;
		return -1;
	}

	ifd = open(from, O_RDONLY | O_CLOEXEC);
	if (ifd == -1)
		return -1;

	if (flags & AC_FS_COPY_OVERWRITE) {
		oflags &= ~(O_EXCL);
		oflags |= O_TRUNC;
	}

	ofd = open(to, O_WRONLY | O_CREAT | O_CLOEXEC | oflags, 0666);
	if (ofd == -1) {
		close(ifd);
		return -1;
	}

	fstat(ifd, &sb);
	ret = fallocate(ofd, 0, 0, sb.st_size);
	if (ret == -1 && errno == EOPNOTSUPP)
		ret = ftruncate(ofd, sb.st_size);
	if (ret == -1)
		/*
		 * ret may still be -1 from fallocate() or may be -1 from
		 * ftruncate(), either way, bail out.
		 */
		goto cleanup;

	do {
		bytes_wrote = sendfile(ofd, ifd, NULL, IO_SIZE);

		/*
		 * Try not to blow the page cache. After each sendfile() we
		 * write out the data and then make sure the previous lot
		 * of data got written out and tell the kernel we no longer
		 * need those pages in memory any more for both the input
		 * and output files.
		 */
		sync_file_range(ofd, window * IO_SIZE, IO_SIZE,
				SYNC_FILE_RANGE_WRITE);

		if (window == 0)
			goto incr_win;

		sync_file_range(ofd, (window-1) * IO_SIZE, IO_SIZE,
				SYNC_FILE_RANGE_WAIT_BEFORE |
				SYNC_FILE_RANGE_WRITE |
				SYNC_FILE_RANGE_WAIT_AFTER);
		posix_fadvise(ofd, (window-1) * IO_SIZE, IO_SIZE,
			      POSIX_FADV_DONTNEED);
		posix_fadvise(ifd, (window-1) * IO_SIZE, IO_SIZE,
			      POSIX_FADV_DONTNEED);

incr_win:
		window++;
	} while (bytes_wrote > 0);

cleanup:
	close(ifd);
	close(ofd);

	return bytes_wrote;
}
