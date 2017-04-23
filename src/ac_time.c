/*
 * ac_time.c - Time related functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#define _GNU_SOURCE		/* struct timespec */

#include <time.h>

/**
 * ac_time_tspec_diff - subtract two struct timespec's
 *
 * @delta: Filled out with the result
 * @end: struct timespec containing the end time
 * @start: struct timespec containing the start time
 *
 * Returns:
 *
 * The time difference as a double
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
