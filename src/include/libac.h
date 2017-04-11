/*
 * libac.h - Miscellaneous functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#ifndef _LIBAC_H_
#define _LIBAC_H_

#ifdef __cplusplus
extern "C" {
#endif

char *ac_str_chomp(char *string);
char *ac_str_substr(const char *src, void *dest, size_t dest_size, int start,
		    int len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _LIBAC_H_ */
