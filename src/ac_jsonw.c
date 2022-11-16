/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_jsonw.c - Functions for writing JSON
 *
 * Copyright (c) 2018, 2020 - 2021	Andrew Clayton
 *					<andrew@digital-domain.net>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>

#include "include/libac.h"

static const size_t ALLOC_SZ = 4096;
static const char *JSON_INDENT = "    ";

static void json_build_str(ac_jsonw_t *json, const char *fmt, ...)
{
	va_list ap;
	int i = 0;
	int len;
	char *buf;
	const char *indenter = !json->indenter ? JSON_INDENT : json->indenter;

	if (json->skip_tabs) {
		i = json->depth;
	} else if (json->len + (json->depth * strlen(indenter)) >=
		   json->allocated) {
		json->str = realloc(json->str, json->allocated + ALLOC_SZ);
		json->allocated += ALLOC_SZ;
	}
	for ( ; i < json->depth; i++)
		json->len += snprintf(json->str + json->len,
				      json->allocated - json->len, "%s",
				      indenter);

	va_start(ap, fmt);
	len = vasprintf(&buf, fmt, ap);
	va_end(ap);

	while (json->len + len >= json->allocated) {
		json->str = realloc(json->str, json->allocated + ALLOC_SZ);
		json->allocated += ALLOC_SZ;
	}

	memcpy(json->str + json->len, buf, len + 1);
	json->len += len;

	free(buf);
}

/**
 * ac_jsonw_init - initialises a new ac_jsonw_t object
 *
 * Returns:
 *
 * A newly initialised ac_jsonw_t object.
 */
ac_jsonw_t *ac_jsonw_init(void)
{
	ac_jsonw_t *json = malloc(sizeof(ac_jsonw_t));

	json->str = malloc(ALLOC_SZ);
	json->allocated = ALLOC_SZ;
	json->depth = 1;
	json->skip_tabs = false;
	json->indenter = NULL;

	strcpy(json->str, "{\n");
	json->len = 2;

	return json;
}

/**
 * ac_jsonw_indent_sz - set the number of spaces to use for indentation
 *
 * @json: The ac_jsonw_t to operate on
 * @size: The number of spaces to use (between 1 and 16)
 */
void ac_jsonw_indent_sz(ac_jsonw_t *json, int size)
{
	if (size < 1 || size > 16)
		return;

	json->indenter = malloc(size + 1);
	memset(json->indenter, ' ', size);
	json->indenter[size] = '\0';
}

/**
 * ac_jsonw_set_indenter - set the indentation character/string
 *
 * @json: The ac_jsonw_t to operate on
 * @indenter: The indentation character/string to use; defaults to '\t'
 */
void ac_jsonw_set_indenter(ac_jsonw_t *json, const char *indenter)
{
	json->indenter = strdup(indenter);
}

static inline void add_char(char *string, char c, size_t *offset)
{
	memset(string + *offset, c, 1);
	(*offset)++;
}

static inline void add_escaped_str(char *string, const char *escaped,
				   size_t *offset, unsigned int len)
{
	memcpy(string + *offset, escaped, len);
	*offset += len;
}

static char *make_escaped_string(const char *str)
{
	char *estring;
	char c;
	size_t offset = 0;

	/* strlen(str) * 6 for worst case scenario; all \uXXXX */
	estring = malloc((strlen(str) * 6) + 1);

	while ((c = *str++) != '\0') {
		if (c < 0x20 &&
		    (c != '\b' && c != '\f' && c != '\t' && c != '\n' &&
		     c != '\r')) {
			snprintf(estring + offset, 7, "\\u%04x", c);
			offset += 6;
			continue;
		}

		if (c == '"')
			add_escaped_str(estring, "\\\"", &offset, 2);
		else if (c == '\\')
			add_escaped_str(estring, "\\\\", &offset, 2);
		else if (c == '\b')
			add_escaped_str(estring, "\\b", &offset, 2);
		else if (c == '\f')
			add_escaped_str(estring, "\\f", &offset, 2);
		else if (c == '\n')
			add_escaped_str(estring, "\\n", &offset, 2);
		else if (c == '\r')
			add_escaped_str(estring, "\\r", &offset, 2);
		else if (c == '\t')
			add_escaped_str(estring, "\\t", &offset, 2);
		else
			add_char(estring, c, &offset);
	}
	estring[offset] = '\0';

	return estring;
}

/**
 * ac_jsonw_add_str - adds a string to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 * @value: The value of the field
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_jsonw_add_str(ac_jsonw_t *json, const char *name, const char *value)
{
	char *escaped_string = make_escaped_string(value);

	if (name)
		json_build_str(json, "\"%s\": \"%s\",\n", name,
			       escaped_string);
	else
		json_build_str(json, "\"%s\",\n", escaped_string);

	free(escaped_string);
}

/**
 * ac_jsonw_add_int - adds an integer to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 * @value: The value of the field
 *
 * The integer accepted is signed 64bit.
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_jsonw_add_int(ac_jsonw_t *json, const char *name, s64 value)
{
	if (name)
		json_build_str(json, "\"%s\": %" PRId64 ",\n", name, value);
	else
		json_build_str(json, "%" PRId64 ",\n", value);
}

/**
 * ac_jsonw_add_real - adds a real number to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 * @value: The value of the field
 * @dp: The number of decimal places to show, -1 for default
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_jsonw_add_real(ac_jsonw_t *json, const char *name, double value,
		       int dp)
{
	char fmt[32] = "\0";
	const char *pfmt = fmt;
	int len = 0;

	if (name)
		len = sprintf(fmt, "\"%%s\": ");

	if (dp == -1)
		sprintf(fmt + len, "%%f,\n");
	else
		snprintf(fmt + len, sizeof(fmt) - len, "%%.%df,\n", dp);

	if (name)
		json_build_str(json, pfmt, name, value);
	else
		json_build_str(json, pfmt, value);
}

/**
 * ac_jsonw_add_bool - adds a boolean value to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 * @value: The value of the field
 *
 * value should be a bool true or false, 0 or 1
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_jsonw_add_bool(ac_jsonw_t *json, const char *name, bool value)
{
	if (name)
		json_build_str(json, "\"%s\": %s,\n", name,
			       value ? "true" : "false");
	else
		json_build_str(json, "%s,\n", value ? "true" : "false");
}

/**
 * ac_jsonw_add_null - adds a null value to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_jsonw_add_null(ac_jsonw_t *json, const char *name)
{
	if (name)
		json_build_str(json, "\"%s\": null,\n", name);
	else
		json_build_str(json, "null,\n");
}

/**
 * ac_jsonw_add_str_or_null - adds a string or a null value to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 * @value: The value of the field
 *
 * If value is NULL or 0 length then it will be put in as null
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_jsonw_add_str_or_null(ac_jsonw_t *json, const char *name,
			      const char *value)
{
	if (value && strlen(value) > 0)
		ac_jsonw_add_str(json, name, value);
	else
		ac_jsonw_add_null(json, name);
}

/**
 * ac_jsonw_add_array - adds an array to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The array name
 */
void ac_jsonw_add_array(ac_jsonw_t *json, const char *name)
{
	json_build_str(json, "\"%s\": [\n", name);
	json->depth++;
}

static void __json_end(ac_jsonw_t *json, const char *closer)
{
	bool empty = false;

	/* cater for empty arrays/objects */
	if (json->str[json->len-2] == '[' || json->str[json->len-2] == '{') {
		empty = true;
		json->len -= 1;
	} else {
		json->len -= 2;
	}

	json->str[json->len] = '\0';
	json->depth--;

	json->skip_tabs = true;
	json_build_str(json, "%s", empty ? "" : "\n");
	if (!empty)
		json->skip_tabs = false;

	json_build_str(json, "%s%s", closer, (json->depth > 0) ? ",\n" : "");
	json->skip_tabs = false;
}

/**
 * ac_jsonw_end_array - ends a JSON array
 *
 * @json: The ac_jsonw_t to operate on
 */
void ac_jsonw_end_array(ac_jsonw_t *json)
{
	__json_end(json, "]");
}

/**
 * ac_jsonw_add_object - adds a new JSON object to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The array name
 */
void ac_jsonw_add_object(ac_jsonw_t *json, const char *name)
{
	if (name)
		json_build_str(json, "\"%s\": {\n", name);
	else
		json_build_str(json, "{\n");

	json->depth++;
}

/**
 * ac_jsonw_end_object - ends a JSON object
 *
 * @json: The ac_jsonw_t to operate on
 */
void ac_jsonw_end_object(ac_jsonw_t *json)
{
	__json_end(json, "}");
}

/**
 * ac_jsonw_end - ends the JSON
 *
 * @json: The ac_jsonw_t to operate on
 */
void ac_jsonw_end(ac_jsonw_t *json)
{
	json->depth = 1;
	__json_end(json, "}");
}

/**
 * ac_jsonw_free - free's the ac_jsonw_t object
 *
 * @json: The ac_jsonw_t to operate on
 */
void ac_jsonw_free(const ac_jsonw_t *json)
{
	if (!json)
		return;

	free(json->str);
	free(json->indenter);
	free((void *)json);
}

/**
 * ac_jsonw_len - gets the length of the created JSON
 *
 * @json: The ac_jsonw_t to operate on
 *
 * Returns:
 *
 * The length in bytes of the created JSON
 */
size_t ac_jsonw_len(const ac_jsonw_t *json)
{
	return json->len;
}

/**
 * ac_jsonw_get - gets the created JSON
 *
 * @json: The ac_jsonw_t to operate on
 *
 * Returns:
 *
 * The created JSON string
 */
const char *ac_jsonw_get(const ac_jsonw_t *json)
{
	return json->str;
}
