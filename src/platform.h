/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * platform.h - Platform dependant stuff
 *
 * Copyright (c) 2021		Andrew Clayton <andrew@digital-domain.net>
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <sys/types.h>

extern ssize_t file_copy(int in_fd, int out_fd);
extern char *gen_uuid(char *buf);

#endif /* _PLATFORM_H_ */
