/*
 * ac_fs.c - Filesystem related utility functions.
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#define _POSIX_C_SOURCE   1		/* strtok_r(3) */
#define _XOPEN_SOURCE	500		/* strdup(3) */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

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
