/* SPDX-License-Identifier: LGPL-2.1 */

/*
 * ac_net.c - Network related functions
 *
 * Copyright (c) 2017		Andrew Clayton <andrew@digital-domain.net>
 */

#define _GNU_SOURCE		/* getaddrinfo(3), freeaddrinfo(3) */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "include/libac.h"

/**
 * ac_net_port_from_sa - extract the port number from a struct sockaddr
 *
 * @sa: The sockaddr structure to get the port number from
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
 * ac_net_inet_pton - address family agnostic wrapper around inet_pton(3)
 *
 * @src - IP address
 * @dst - A buffer to hold the returned network address structure
 *
 * Returns:
 *
 * 1 on success, 0 if src does not contain a valid IP adress, -1 otherwise
 */
int ac_net_inet_pton(const char *src, void *dst)
{
	if (strchr(src, ':'))
                return inet_pton(AF_INET6, src, dst);
	else
		return inet_pton(AF_INET, src, dst);
}

/**
 * ac_net_inet_ntop - address family agnostic wrapper around inet_ntop(3)
 *
 * @src - Structure containing the address to be converted
 * @dst - A buffer to hold the returned string
 * @size - The size of the buffer
 *
 * Returns:
 *
 * A pointer to dst
 */
const char *ac_net_inet_ntop(const void *src, char *dst, socklen_t size)
{
	const struct sockaddr *sa = (struct sockaddr *)src;

	switch (sa->sa_family) {
	case AF_INET6:
                return inet_ntop(sa->sa_family,
				&((struct sockaddr_in6 *)sa)->sin6_addr, dst,
				size);
	case AF_INET:
		return inet_ntop(sa->sa_family,
				&((struct sockaddr_in *)sa)->sin_addr, dst,
				size);
	default:
		return NULL;
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
		char addrp[INET6_ADDRSTRLEN];

		ac_net_inet_ntop(resp->ai_addr, addrp, sizeof(addrp));
		again = ac_ns_lookup_cb(resp, addrp);
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

static inline bool __ipv4_isin(const char *network, u8 cidr,
			       const struct in_addr *addr)
{
	struct in_addr ip_addr;
	struct in_addr net_addr;

	inet_pton(AF_INET, network, &net_addr);

	ip_addr.s_addr = addr->s_addr & htonl(~0UL << (32 - cidr));

	return ip_addr.s_addr == net_addr.s_addr;
}

/**
 * ac_net_ipv4_isin - check if an IPv4 address is within the given network
 *
 * @network: The IPv4 network to check against
 * @cidr: The netmask
 * @addr: The IPv4 address
 *
 * Returns:
 *
 * true for a match, false otherwise
 */
bool ac_net_ipv4_isin(const char *network, u8 cidr, const char *addr)
{
	struct in_addr ip_addr;

	inet_pton(AF_INET, addr, &ip_addr);

	return __ipv4_isin(network, cidr, &ip_addr);
}

/**
 * ac_net_ipv4_isin_sa - check if an IPv4 address is within the given network
 *
 * @network: The IPv4 network to check against
 * @cidr: The netmask
 * @sa: A struct sockaddr containing the IPv4 address to check
 *
 * Note: This is like ac_net_ipv4_isin except that the IPv4 address to be
 * checked is in a struct sockaddr_in passed into this function cast as
 * a struct sockaddr * such as that you might get back from accept(2)
 *
 * Returns:
 *
 * true for a match, false otherwise
 */
bool ac_net_ipv4_isin_sa(const char *network, u8 cidr,
			 const struct sockaddr *sa)
{
	struct in_addr *ip_addr = &((struct sockaddr_in *)sa)->sin_addr;

	return __ipv4_isin(network, cidr, ip_addr);
}

static inline bool __ipv6_isin(const char *network, u8 prefixlen,
			       const u8 *addr)
{
	u8 i;
	u8 mask[sizeof(struct in6_addr)];
	u8 net[sizeof(struct in6_addr)];

	inet_pton(AF_INET6, network, net);

	/* Create a mask based on prefixlen */
	for (i = 0; i < 16; i++) {
		u8 s = (prefixlen > 8) ? 8 : prefixlen;

		prefixlen -= s;
		mask[i] = (0xffu << (8 - s));
	}

	for (i = 0; i < 16; i++) {
		if ((addr[i] & mask[i]) != net[i])
			return false;
	}

	return true;
}

/**
 * ac_net_ipv6_isin - check if an IPv6 address is within the given network
 *
 * @network: The IPv6 network to check against
 * @prefixlen: The prefixlen
 * @addr: The IPv6 address
 *
 * Returns:
 *
 * true for a match, false otherwise
 */
bool ac_net_ipv6_isin(const char *network, u8 prefixlen, const char *addr)
{
	u8 addr6[sizeof(struct in6_addr)];

	inet_pton(AF_INET6, addr, addr6);

	return __ipv6_isin(network, prefixlen, addr6);
}

/**
 * ac_net_ipv6_isin_sa - check if an IPv6 address is within the given network
 *
 * @network: The IPv6 network to check against
 * @prefixlen: The prefixlen
 * @sa: A struct sockaddr containing the IPv6 address to check
 *
 * Note: This is like ac_net_ipv6_isin except that the IPv6 address to be
 * checked is in a struct sockaddr_in6 passed into this function cast as
 * a struct sockaddr * such as that you might get back from accept(2)
 *
 * Returns:
 *
 * true for a match, false otherwise
 */
bool ac_net_ipv6_isin_sa(const char *network, u8 prefixlen,
			 const struct sockaddr *sa)
{
	const u8 *addr = (u8 *)&((struct sockaddr_in6 *)sa)->sin6_addr;

	return __ipv6_isin(network, prefixlen, addr);
}
