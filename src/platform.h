/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * platform.h - Platform dependant stuff
 *
 * Copyright (c) 2021		Andrew Clayton <andrew@digital-domain.net>
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <sys/types.h>

#ifdef __FreeBSD__
#include <fcntl.h>
#include <search.h>

struct random_data {
	int32_t *fptr;		    /* Front pointer.			   */
	int32_t *rptr;		    /* Rear pointer.			   */
	int32_t *state;		    /* Array of state values.		   */
	int rand_type;		    /* Type of random number generator.	   */
	int rand_deg;		    /* Degree of random number generator.  */
	int rand_sep;		    /* Distance between front and rear.	   */
	int32_t *end_ptr;	    /* Pointer behind state table.	   */
};

extern int fallocate(int fd, int mode, off_t offset, off_t len);

extern void tdestroy(void *root, void (*destroy_func)(void *));
extern int random_r(struct random_data *buf, int32_t *result);
extern int srandom_r(unsigned int seed, struct random_data *buf);
extern int initstate_r(unsigned int seed, char *statebuf,
		       size_t statelen, struct random_data *buf);
extern int setstate_r(char *statebuf, struct random_data *buf);
#endif

#if defined(__FreeBSD__) || (__GLIBC__ <= 2 && __GLIBC_MINOR__ < 30)
extern void twalk_r(const void *vroot,
		    void (*action) (const void *, VISIT, void *),
		    void *closure);
#endif

extern ssize_t file_copy(int in_fd, int out_fd);
extern char *gen_uuid(char *buf);

#endif /* _PLATFORM_H_ */
