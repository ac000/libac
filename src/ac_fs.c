/*
 * ac_fs.c - Filesystem related utility functions.
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
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
 * true if if follows the guidelines, false otherwise
 */
bool ac_fs_is_posix_name(const char *name)
{
	if (name[0] == '-')
		return false;

	/*
	 * Check for valid POSIX filename characters as defined by:
         * http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap03.html#tag_03_281
	 */
	for ( ; *name != '\0'; name++) {
		switch (*name) {
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
 * @path: The path to make
 *
 * Returns:
 *
 * 0 on success, -1 on failure check errno
 */
int ac_fs_mkdir_p(const char *path)
{
	int ret = 0;
	char *dir;
	char *ptr;
	char *saveptr;
	char mdir[4096] = "";

	if (strlen(path) >= sizeof(mdir)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	if (path[0] == '/')
		strcat(mdir, "/");

	dir = strdup(path);
	ptr = dir;
	for ( ; ; ) {
		char *token;

		token = strtok_r(dir, "/", &saveptr);
		if (token == NULL)
			break;
		strcat(mdir, token);
		ret = mkdir(mdir, 0777);
		if (ret == -1 && errno != EEXIST)
			break;
		strcat(mdir, "/");
		dir = NULL;
	}
	free(ptr);

	return ret;
}

#define IO_SIZE	(1024*1024 * 2)
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
 * 0 on success, -1 otherwise, check errno
 */
ssize_t ac_fs_copy(const char *from, const char *to, int flags)
{
	int ifd;
	int ofd;
	int ret;
	int oflags = O_EXCL;
	int window = 0;
	ssize_t bytes_wrote = -1;
	struct stat sb;

	ifd = open(from, O_RDONLY | O_CLOEXEC);
	if (ifd == -1)
		return -1;

	if (flags & AC_FS_COPY_OVERWRITE) {
		oflags &= ~(O_EXCL);
		oflags |= O_TRUNC;
	}

	ofd = open(to, O_WRONLY | O_CREAT | O_CLOEXEC | oflags, 0666);
	if (ofd == -1)
		goto cleanup;

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
		if (window) {
			sync_file_range(ofd, (window-1) * IO_SIZE, IO_SIZE,
					SYNC_FILE_RANGE_WAIT_BEFORE |
					SYNC_FILE_RANGE_WRITE |
					SYNC_FILE_RANGE_WAIT_AFTER);
			posix_fadvise(ofd, (window-1) * IO_SIZE, IO_SIZE,
					POSIX_FADV_DONTNEED);
			posix_fadvise(ifd, (window-1) * IO_SIZE, IO_SIZE,
					POSIX_FADV_DONTNEED);
		}
		window++;
	} while (bytes_wrote > 0);

cleanup:
	close(ifd);
	close(ofd);

	return bytes_wrote;
}
