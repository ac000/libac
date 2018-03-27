/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_time.c - Time related functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE		/* struct timespec, nanosleep(2) */

#include <time.h>
#include <errno.h>

#include "include/libac.h"

/**
 * ac_time_tspec_diff - subtract two struct timespec's
 *
 * @delta: Filled out with the result
 * @end: struct timespec containing the end time
 * @start: struct timespec containing the start time
 *
 * Returns:
 *
 * The time difference in seconds as a double
 */
double ac_time_tspec_diff(struct timespec *delta, const struct timespec *end,
			  const struct timespec *start)
{
	if (end->tv_nsec < start->tv_nsec) {
		delta->tv_nsec = 1000000000L - (start->tv_nsec - end->tv_nsec);
		delta->tv_sec = end->tv_sec - start->tv_sec - 1;
	} else {
		delta->tv_nsec = end->tv_nsec - start->tv_nsec;
		delta->tv_sec = end->tv_sec - start->tv_sec;
	}

	return (double)delta->tv_sec + (double)delta->tv_nsec / 1000000000L;
}

/**
 * ac_time_secs_to_hms - convert total seconds to broken down hrs, mins & secs
 *
 * @totel: Number of seconds
 * @hours: The returned number of hours
 * @minutes: The returned number of minutes
 * @seconds: The returned number of seconds
 */
void ac_time_secs_to_hms(long total, int *hours, int *minutes, int *seconds)
{
	*seconds = total % 60;
	total /= 60;
	*minutes = total % 60;
	*hours = total / 60;
}

/**
 *
 * ac_time_nsleep - wrapper around nanosleep(2) to sleep through interrupts
 *
 * @nsecs - Number of nanoseconds to sleep for
 *
 * Returns:
 *
 * 0 On success or -1 on failure
 */
int ac_time_nsleep(u64 nsecs)
{
	struct timespec req;
	struct timespec rem;
	int err;

	if (nsecs < AC_TIME_NS_SEC) {
		req.tv_sec = 0;
		req.tv_nsec = nsecs;
	} else {
		req.tv_sec = nsecs / AC_TIME_NS_SEC;
		req.tv_nsec = nsecs % AC_TIME_NS_SEC;
	}

do_sleep:
	err = nanosleep(&req, &rem);
	if (err && errno == EINTR) {
		req.tv_sec = rem.tv_sec;
		req.tv_nsec = rem.tv_nsec;
		goto do_sleep;
	} else if (err) {
		return -1;
	}

	return 0;
}
