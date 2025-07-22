/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 14:13:24 by vzurera-         ###   ########.fr       */
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
	#include <arpa/inet.h>
	#include <netinet/ip_icmp.h>

#pragma endregion

#pragma region "Defines"

	#define SELECT_TIMEOUT			0.1									// 
	#define PACKET_TIMEOUT			10									// Max seconds to wait for response

	#define IP_HEADER				60									// 
	#define ICMP_HEADER				8									// 
	#define MAX_SIZE				(65535 - IP_HEADER - ICMP_HEADER)	// 
	#define DEFAULT_SIZE			56									// 
	#define MAX_PATTERN				16									// 

	#define OPT_DEBUG				0x001
	#define OPT_DONTROUTE			0x002

	#define OPT_INTERVAL			0x002
	#define OPT_NUMERIC				0x004
	#define OPT_QUIET				0x008
	#define OPT_ROUTE				0x010
	#define OPT_VERBOSE				0x020

	#define DEFAULT_INTERVAL		1000

#pragma endregion

#pragma region "Structures"

	typedef struct s_options {
		int					pid;					// 
		unsigned long		options;				// NUMERIC, VERVOSE, FLOOD, QUIET, ROUTE
		int					socket_type;			// DEBUG, IGNORE_ROUTING

		size_t				count;					// [-c, --count=NUMBER]			stop after sending NUMBER packets
		size_t				interval;				// [-i, --interval=NUMBER]		wait NUMBER seconds between sending each packet
		size_t				ttl;					// [	--ttl=N]				specify N as time-to-live
		size_t				tos;					// [-T, --tos=NUM]				set type of service (TOS) to NUM
		size_t				timeout;				// [-w, --timeout=N]			stop after N seconds
		size_t				linger;					// [-W, --linger=N]				number of seconds to wait for response
		size_t				size;					// [-s, --size=NUMBER]			send NUMBER data octets

		unsigned char		pattern[MAX_PATTERN];	// [-p, --pattern=PATTERN]		fill ICMP packet with given pattern (hex)
		int					pattern_len;			// 

		char				hostname[254];			// IP address or hostname
		char				host[INET_ADDRSTRLEN];	// IP address resolved
		struct sockaddr_in	sockaddr;				// 
	}	t_options;

#pragma endregion

#pragma region "Methods"

	int	parse_options(t_options *options, int argc, char **argv);

#pragma endregion
