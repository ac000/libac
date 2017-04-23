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

/**
 * ac_str_chomp - remove trailing white space from a string
 *
 * @string: String to be chomp'd
 *
 * Returns:
 *
 * A pointer to the chomped string
 */
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

/**
 * ac_str_substr - extract a substring from a string
 *
 * @src: The string containing the substring to be extracted
 * @dest: A buffer to place the substring into
 * @dest_size: Size of the output buffer
 * @start: The start position in the string to start extraction
 * @len: How many bytes to extract
 *
 * Returns:
 *
 * A pointer to the substring
 */
char *ac_str_substr(const char *src, void *dest, size_t dest_size, int start,
		    int len)
{
	snprintf(dest, dest_size, "%.*s", len, src + start);

	return dest;
}
