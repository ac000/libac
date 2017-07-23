/*
 * ac_str.c - String related utility functions.
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "include/libac.h"

/**
 * ac_str_freev - free a string vector
 *
 * @stringv: The string vector to be free'd
 */
void ac_str_freev(char **stringv)
{
	char **pp;

	if (!stringv)
		return;

	for (pp = stringv; *stringv != NULL; stringv++)
		free(*stringv);
	free(pp);
}

/**
 * ac_str_split - split a string up into a NULL terminated vector
 *
 * @string: String to be split
 * @delim: The character to use as the delimiter
 * @flags: Can be 0 or AC_STR_SPLIT_STRICT (to return an empty vector when
 *	   there's no delimiters)
 *
 * An empty string is returned as an empty vector, i.e vec[0] == NULL,
 *
 * In either case the returned vector should be free'd by a call to
 * ac_str_freev
 *
 * Returns:
 *
 * A NULL terminated vector (array of string pointers)
 */
char **ac_str_split(const char *string, int delim, int flags)
{
	char *strd;
	char *p;
	char **fields;
	char *tok;
	int i = 1;

	/* Check for unknown flags */
	if (flags & ~(AC_STR_SPLIT_STRICT)) {
		errno = EINVAL;
		return NULL;
	}

	fields = malloc(sizeof(char *));
	fields[0] = NULL;

	if (strlen(string) == 0 ||
	    (!strchr(string, delim) && (flags & AC_STR_SPLIT_STRICT)))
		return fields;

	strd = strdup(string);
	p = strd;

	tok = strsep(&strd, (char *)&delim);
	while (tok) {
		fields = realloc(fields, sizeof(char *) * i);
		fields[i - 1] = strdup(tok);
		tok = strsep(&strd, (char *)&delim);
		i++;
	}
	fields = realloc(fields, sizeof(char *) * i);
	fields[i - 1] = NULL;

	free(p);

	return fields;
}

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
	size_t i;
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
		size_t j;

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
