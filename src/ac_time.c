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

void ac_time_secs_to_hms(long total, int *hours, int *minutes, int *seconds)
{
	*seconds = total % 60;
	total /= 60;
	*minutes = total % 60;
	*hours = total / 60;
}
