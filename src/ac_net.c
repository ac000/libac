/*
 * ac_net.c - Network related functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 *
 * Licensed under the GNU Lesser General Public License (LGPL) version 2.1
 * See COPYING
 */

#define _GNU_SOURCE		/* getaddrinfo(3), freeaddrinfo(3) */

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "include/libac.h"


/**
 * ac_net_port_from_sa - extract the port number from a struct sockaddr
 *
 * @sa - The sockaddr structure to get the port number from
 *
 * Note: This is the same as the following open coded
 *
 *    (sa.sa_family == AF_INET6) ?
 *     ntohs(*(&((struct sockaddr_in6 *)&sa)->sin6_port)) :
 *     ntohs(*(&((struct sockaddr_in *)&sa)->sin_port))
 *
 * Returns:
 *
 * The port number in host byte order or 0 for an unknown address family
 */
u16 ac_net_port_from_sa(const struct sockaddr *sa)
{
	switch (sa->sa_family) {
	case AF_INET6:
		return ntohs(((struct sockaddr_in6 *)sa)->sin6_port);
	case AF_INET:
		return ntohs(((struct sockaddr_in *)sa)->sin_port);
	default:
		return 0;
	}
}

/**
 * ac_net_ns_lookup_by_host - lookup a host by hostname (get its IP(s))
 *
 * @hints: An addrinfo structure with criteria for selecting the IP addresses
 *         to be returned
 * @node: The host to be looked up
 * @ac_ns_lookup_cb: A function that is called for each IP returned. Return
 *                   true from this function to be called again or false to
 *                   stop
 *
 * Returns:
 *
 * 0 on success, -1 on failure check errno
 */
int ac_net_ns_lookup_by_host(const struct addrinfo *hints, const char *node,
			     bool (*ac_ns_lookup_cb)(const struct addrinfo *ai,
						     const char *res))
{
	int ret;
	struct addrinfo *res;
	struct addrinfo *resp;

	ret = getaddrinfo(node, NULL, hints, &res);
	if (ret == -1)
		return ret;

	for (resp = res; resp != NULL; resp = resp->ai_next) {
		bool again;
		char result[NI_MAXHOST];
		struct sockaddr_storage *ss =
			(struct sockaddr_storage *)resp->ai_addr;

		switch (ss->ss_family) {
		case AF_INET:
			inet_ntop(ss->ss_family,
				  &((struct sockaddr_in *)ss)->sin_addr,
				  result, sizeof(result));
			break;
		case AF_INET6:
			inet_ntop(ss->ss_family,
				  &((struct sockaddr_in6 *)ss)->sin6_addr,
				  result, sizeof(result));
			break;
		}

		again = ac_ns_lookup_cb(resp, result);
		if (!again)
			break;
	}
	freeaddrinfo(res);

	return 0;
}

/**
 * ac_net_ns_lookup_by_ip - lookup a host by IP address (get its hostname)
 *
 * @hints: An addrinfo structure with criteria for selecting the hostname
 *         to be returned
 * @node: The IP address to be looked up
 * @ac_ns_lookup_cb: A function that is called on the returned hostname
 *
 * Returns:
 *
 * 0 on success, -1 on failure check errno
 */
int ac_net_ns_lookup_by_ip(const struct addrinfo *hints, const char *node,
			   bool (*ac_ns_lookup_cb)(const struct addrinfo *ai,
						   const char *res))
{
	int ret;
	char host[NI_MAXHOST];
	struct addrinfo *res;

	ret = getaddrinfo(node, NULL, hints, &res);
	if (ret == -1)
		return ret;

	getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, NULL, 0,
                        0);

	ac_ns_lookup_cb(res, host);
	freeaddrinfo(res);

	return 0;
}
