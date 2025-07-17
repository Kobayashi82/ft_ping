/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/17 23:40:06 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#define _GNU_SOURCE

	#include <errno.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <limits.h>
	#include <getopt.h>
	#include <stdbool.h>
	#include <sys/time.h>

	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netinet/ip_icmp.h>

#pragma endregion

#pragma region "Defines"

	#define MAX_WAIT		10						// Max seconds to wait for response
	#define MAX_PATTERN		16						// Maximal length of pattern

	#define ECHO			256
	#define ADDRESS			257
	#define TIMESTAMP		258

	#define OPT_FLOOD		0x001
	#define OPT_INTERVAL	0x002
	#define OPT_NUMERIC		0x004
	#define OPT_QUIET		0x008
	#define OPT_ROUTE		0x010
	#define OPT_VERBOSE		0x020
	#define OPT_IPTIMESTAMP	0x040
	#define OPT_FLOWINFO	0x080
	#define OPT_TCLASS		0x100

	#define OPT_TSONLY		0x001
	#define OPT_TSADDR		0x002

	#define MAX_IP_LEN		60
	#define MAX_ICMP_LEN	76
	#define MAX_DATA_LEN	(65535 - MAX_IP_LEN - MAX_ICMP_LEN)

	#define PING_PRECISION			1000
	#define PING_MIN_USER_INTERVAL	(200000 / PING_PRECISION)
	#define PING_DEFAULT_INTERVAL	1000

#pragma endregion

#pragma region "Structures"

	typedef struct s_options {
		bool				is_root;				// 
		unsigned long		type;					// ADDRESS, ECHO, MASK, TIMESTAMP
		unsigned long		options;				// NUMERIC, VERVOSE, FLOOD, QUIET, ROUTE
		int					socket_type;			// DEBUG, IGNORE_ROUTING

		size_t				count;					// [-c, --count=NUMBER]			stop after sending NUMBER packets
		size_t				interval;				// [-i, --interval=NUMBER]		wait NUMBER seconds between sending each packet
		size_t				ttl;					// [	--ttl=N]				specify N as time-to-live
		size_t				tos;					// [-T, --tos=NUM]				set type of service (TOS) to NUM
		size_t				timeout;				// [-w, --timeout=N]			stop after N seconds
		size_t				linger;					// [-W, --linger=N]				number of seconds to wait for response
		size_t				size;					// [-s, --size=NUMBER]			send NUMBER data octets

		unsigned long		ip_timestamp;			// [	--ip-timestamp=FLAG]	IP timestamp of type FLAG, which is one of "tsonly" and "tsaddr"
		unsigned long		preload;				// [-l, --preload=NUMBER]		send NUMBER packets as fast as possible before falling into normal mode of behavior (root only)

		unsigned char		pattern[MAX_PATTERN];	// [-p, --pattern=PATTERN]		fill ICMP packet with given pattern (hex)
		int					pattern_len;			// 

		char				hostname[254];			// IP address or hostname
		char				host[INET_ADDRSTRLEN];	// IP address resolved
		struct sockaddr_in	sockaddr;				// 
	}	t_options;

	// Hostnames must not exceed 253 bytes in total, with each label (between dots) up to 63 bytes.
	// Allowed characters are alphanumeric and hyphens (-).
	// Hyphens cannot appear at the start or end of a label, nor can two hyphens appear consecutively.
	// Dots (.) are used to separate labels and cannot appear consecutively or at the start or end of the hostname.

	// Pattern length is determine by the option --size or maxsize (65535 - IP header - ICMP header)

#pragma endregion

#pragma region "Variables"



#pragma endregion

#pragma region "Methods"

	int parse_options(t_options *options, int argc, char **argv);

#pragma endregion
