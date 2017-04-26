/*
 * ac_str.c - String related utility functions.
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#include <stdio.h>
#include <stdlib.h>
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

static int minimum(int a, int b, int c)
{
	int min = a;

	if (b < min)
		min = b;
	if (c < min)
		min = c;

	return min;
}

/**
 * ac_str_levenshtein - calculate the Levenshtein distance between two
 * 			strings
 *
 * @s: First string
 * @t: Second string
 *
 * This is taken from the 'Iterative with two matrix rows' version from
 * https://en.wikipedia.org/wiki/Levenshtein_distance
 *
 * Returns:
 *
 * An integer representing the Levenshtein (edit) distance between the
 * two strings
 */
int ac_str_levenshtein(const char *s, const char *t)
{
	int i;
	int d;
	int *v0;
	int *v1;
	size_t slen = strlen(s);
	size_t tlen = strlen(t);

	if (strcmp(s, t) == 0)
		return 0;

	v0 = malloc((tlen + 1) * sizeof(int));
	v1 = malloc((tlen + 1) * sizeof(int));

	for (i = 0; i < tlen + 1; i++)
		v0[i] = i;

	for (i = 0; i < slen; i++) {
		int j;

		v1[0] = i + 1;

		for (j = 0; j < tlen; j++) {
			int cost = (s[i] == t[j]) ? 0 : 1;

			v1[j + 1] = minimum(v1[j] + 1, v0[j + 1] + 1,
					    v0[j] + cost);
		}

		for (j = 0; j < tlen + 1; j++)
			v0[j] = v1[j];
	}
	d = v1[tlen];

	free(v0);
	free(v1);

	return d;
}
