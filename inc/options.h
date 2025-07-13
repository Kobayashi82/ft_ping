/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/13 22:31:19 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <getopt.h>
	#include <stdbool.h>
	#include <string.h>
	#include <stdlib.h>

#pragma endregion

#pragma region "Structures"

	typedef struct s_options {
		// ICMP request types
		bool			address;				// [	--address]				send ICMP_ADDRESS packets (root only)
		bool			echo;					// [	--echo]					send ICMP_ECHO packets (default)
		bool			mask;					// [	--mask]					same as --address
		bool			timestamp;				// [	--timestamp]			send ICMP_TIMESTAMP packets
		int				type;					// [-t, --type=TYPE]			send TYPE packets
		// All request
		int				count;					// [-c, --count=NUMBER]			stop after sending NUMBER packets
		bool			debug;					// [-d, --debug]				set the SO_DEBUG option
		int				interval;				// [-i, --interval=NUMBER]		wait NUMBER seconds between sending each packet
		bool			numeric;				// [-n, --numeric]				do not resolve host addresses
		bool			ignore_routing;			// [-r, --ignore-routing]		send directly to a host on an attached network
		int				ttl;					// [	--ttl=N]				specify N as time-to-live
		int				tos;					// [-T, --tos=NUM]				set type of service (TOS) to NUM
		bool			verbose;				// [-v, --verbose]				verbose output
		int				timeout;				// [-w, --timeout=N]			stop after N seconds
		int				linger;					// [-W, --linger=N]				number of seconds to wait for response
		// Echo requests
		bool			flood;					// [-f, --count=NUMBER]			flood ping (root only)
		int				ip_timestamp;			// [	--ip-timestamp=FLAG]	IP timestamp of type FLAG, which is one of "tsonly" and "tsaddr"
		int				preload;				// [-l, --preload=NUMBER]		send NUMBER packets as fast as possible before falling into normal mode of behavior (root only)
		unsigned char	pattern[65507];			// [-p, --pattern=PATTERN]		fill ICMP packet with given pattern (hex)
		bool			quiet;					// [-q, --quiet]				quiet output
		bool			route;					// [-R, --route]				record route
		int				size;					// [-s, --size=NUMBER]			send NUMBER data octets
		// Info
		bool			help;					// [-h?, --help]				give this help list
		bool			usage;					// [	--usage]				give a short usage message
		bool			version;				// [-V, --version]				print program version
		// Target
		char			target[254];			// IP address or hostname
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

	void parse_options(t_options *options, int argc, char **argv);

#pragma endregion
