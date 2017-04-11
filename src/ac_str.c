/*
 * ac_str.c - String related utility functions.
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#include <stdio.h>
#include <string.h>

char *ac_str_chomp(char *string)
{
	size_t len = strlen(string);
	char *ptr = string;

	ptr += len;
	while (*(--ptr)) {
		if (*ptr == ' ' ||
		    *ptr == '\t' ||
		    *ptr == '\n' ||
		    *ptr == '\r')
			*ptr = '\0';
		else
			break;
	}

	return string;
}

char *ac_str_substr(const char *src, void *dest, size_t dest_size, int start,
		    int len)
{
	snprintf(dest, dest_size, "%.*s", len, src + start);

	return dest;
}
