/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_json.c - Functions for writing JSON
 *
 * Copyright (c) 2018		Andrew Clayton <andrew@digital-domain.net>
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

static void json_vsnprintf(ac_jsonw_t *json, const char *fmt, ...)
{
	va_list ap;
	int i = 0;
	int len;

	if (json->skip_tabs) {
		i = json->depth;
	} else if (json->len + json->depth >= json->allocated) {
		json->str = realloc(json->str, json->allocated + ALLOC_SZ);
		json->allocated += ALLOC_SZ;
	}
	for ( ; i < json->depth; i++)
		json->len += snprintf(json->str + json->len,
				      json->allocated - json->len, "\t");

again:
	va_start(ap, fmt);
	len = vsnprintf(json->str + json->len, json->allocated - json->len,
			fmt, ap);
	va_end(ap);

	if (json->len + len >= json->allocated) {
		json->str = realloc(json->str, json->allocated + ALLOC_SZ);
		json->allocated += ALLOC_SZ;
		goto again;
	}

	json->len += len;
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

	strcpy(json->str, "{\n");
	json->len = 2;

	return json;
}

/**
 * ac_json_add_str - adds a string to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 * @value: The value of the field
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_json_add_str(ac_jsonw_t *json, const char *name, const char *value)
{
	if (name)
		json_vsnprintf(json, "\"%s\": \"%s\",\n", name, value);
	else
		json_vsnprintf(json, "\"%s\",\n", value);
}

/**
 * ac_json_add_int - adds an integer to the JSON
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
void ac_json_add_int(ac_jsonw_t *json, const char *name, s64 value)
{
	if (name)
		json_vsnprintf(json, "\"%s\": %" PRId64 ",\n", name, value);
	else
		json_vsnprintf(json, "%" PRId64 ",\n", value);
}

/**
 * ac_json_add_float - adds an floating point number to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 * @value: The value of the field
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_json_add_float(ac_jsonw_t *json, const char *name, double value)
{
	if (name)
		json_vsnprintf(json, "\"%s\": %g,\n", name, value);
	else
		json_vsnprintf(json, "%g,\n", value);
}

/**
 * ac_json_add_bool - adds a boolean value to the JSON
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
void ac_json_add_bool(ac_jsonw_t *json, const char *name, bool value)
{
	if (name)
		json_vsnprintf(json, "\"%s\": %s,\n", name,
			       value ? "true" : "false");
	else
		json_vsnprintf(json, "%s,\n", value ? "true" : "false");
}

/**
 * ac_json_add_null - adds a null value to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The field name
 *
 * name can be NULL when no field name is required. i.e when adding
 * array items.
 */
void ac_json_add_null(ac_jsonw_t *json, const char *name)
{
	if (name)
		json_vsnprintf(json, "\"%s\": null,\n", name);
	else
		json_vsnprintf(json, "null,\n");
}

/**
 * ac_json_add_str_or_null - adds a string or a null value to the JSON
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
void ac_json_add_str_or_null(ac_jsonw_t *json, const char *name,
			     const char *value)
{
	if (value && strlen(value) > 0)
		ac_json_add_str(json, name, value);
	else
		ac_json_add_null(json, name);
}

/**
 * ac_json_add_array - adds an array to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The array name
 */
void ac_json_add_array(ac_jsonw_t *json, const char *name)
{
	json_vsnprintf(json, "\"%s\": [\n", name);
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
	json_vsnprintf(json, "%s", empty ? "" : "\n");
	if (!empty)
		json->skip_tabs = false;

	json_vsnprintf(json, "%s%s", closer, (json->depth > 0) ? ",\n" : "");
	json->skip_tabs = false;
}

/**
 * ac_json_end_array - ends a JSON array
 *
 * @json: The ac_jsonw_t to operate on
 */
void ac_json_end_array(ac_jsonw_t *json)
{
	__json_end(json, "]");
}

/**
 * ac_json_add_object - adds a new JSON object to the JSON
 *
 * @json: The ac_jsonw_t to operate on
 * @name: The array name
 */
void ac_json_add_object(ac_jsonw_t *json, const char *name)
{
	json_vsnprintf(json, "\"%s\": {\n", name);
	json->depth++;
}

/**
 * ac_json_end_object - ends a JSON object
 *
 * @json: The ac_jsonw_t to operate on
 */
void ac_json_end_object(ac_jsonw_t *json)
{
	__json_end(json, "}");
}

/**
 * ac_json_end - ends the JSON
 *
 * @json: The ac_jsonw_t to operate on
 */
void ac_json_end(ac_jsonw_t *json)
{
	json->depth = 1;
	__json_end(json, "}");
}

/**
 * ac_json_free - free's the ac_jsonw_t object
 *
 * @json: The ac_jsonw_t to operate on
 */
void ac_json_free(ac_jsonw_t *json)
{
	if (!json)
		return;

	free(json->str);
	free(json);
}

/**
 * ac_json_len - gets the length of the created JSON
 *
 * @json: The ac_jsonw_t to operate on
 *
 * Returns:
 *
 * The length in bytes of the created JSON
 */
size_t ac_json_len(const ac_jsonw_t *json)
{
	return json->len;
}

/**
 * ac_json_get - gets the created JSON
 *
 * @json: The ac_jsonw_t to operate on
 *
 * Returns:
 *
 * The created JSON string
 */
const char *ac_json_get(const ac_jsonw_t *json)
{
	return json->str;
}
