/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_json.c - Functions for reading JSON
 *
 * Copyright (c) 2020		Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "include/libac.h"

/**
 * ac_json_load_from_fd - loads json from an open file descriptor
 *
 * @fd: The file descriptor to load the JSON from
 * @offset: The offset into the file to read from (0 for the start)
 *
 * Returns:
 *
 * A malloc'd buffer containing the JSON, should be free'd
 */
char *ac_json_load_from_fd(int fd, off_t offset)
{
	FILE *out;
	size_t size;
	char *ptr = NULL;

	out = open_memstream(&ptr, &size);

	lseek(fd, offset, SEEK_SET);
	for (;;) {
		char buf[4097];
		ssize_t bytes_read;

		bytes_read = read(fd, buf, sizeof(buf) - 1);
		if (bytes_read <= 0)
			break;

		buf[bytes_read] = '\0';
		fprintf(out, "%s", buf);
	}
	fclose(out);

	return ptr;
}

/**
 * ac_json_load_from_file - loads json from the specified file
 *
 * @file: The file to load the JSON from
 * @offset: The offset into the file to read from (0 for the start)
 *
 * Returns:
 *
 * A malloc'd buffer containing the JSON, should be free'd
 */
char *ac_json_load_from_file(const char *file, off_t offset)
{
	int fd;
	char *ptr;

	fd = open(file, O_RDONLY|O_CLOEXEC);
	if (fd == -1)
		return NULL;

	ptr = ac_json_load_from_fd(fd, offset);

	close(fd);

	return ptr;
}
