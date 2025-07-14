/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:45 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/14 14:02:50 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO: Validate input

#pragma region "Includes"

	#include "options.h"

#pragma endregion

#pragma region "Help"

	static int help() {
		dprintf(2, "Usage: ft_ping [OPTION...] HOST ...\n");
		dprintf(2, "Send ICMP ECHO_REQUEST packets to network hosts.\n");
		dprintf(2, "\n");
		dprintf(2, " Options controlling ICMP request types:\n");
		dprintf(2, "       --address              send ICMP_ADDRESS packets (root only)\n");
		dprintf(2, "       --echo                 send ICMP_ECHO packets (default)\n");
		dprintf(2, "       --mask                 same as --address\n");
		dprintf(2, "       --timestamp            send ICMP_TIMESTAMP packets\n");
		dprintf(2, "  -t,  --type=TYPE            send TYPE packets\n");
		dprintf(2, "\n");
		dprintf(2, " Options valid for all request types:\n");
		dprintf(2, "\n");
		dprintf(2, "  -c,  --count=NUMBER         stop after sending NUMBER packets\n");
		dprintf(2, "  -d,  --debug                set the SO_DEBUG option\n");
		dprintf(2, "  -i,  --interval=NUMBER      wait NUMBER seconds between sending each packet\n");
		dprintf(2, "  -n,  --numeric              do not resolve host addresses\n");
		dprintf(2, "  -r,  --ignore-routing       send directly to a host on an attached network\n");
		dprintf(2, "       --ttl=N                specify N as time-to-live\n");
		dprintf(2, "  -T,  --tos=NUM              set type of service (TOS) to NUM\n");
		dprintf(2, "  -v,  --verbose              verbose output\n");
		dprintf(2, "  -w,  --timeout=N            stop after N seconds\n");
		dprintf(2, "  -W,  --linger=N             number of seconds to wait for response\n");
		dprintf(2, "\n");
		dprintf(2, " Options valid for --echo requests:\n");
		dprintf(2, "\n");
		dprintf(2, "  -f,  --flood                flood ping (root only)\n");
		dprintf(2, "       --ip-timestamp=FLAG    IP timestamp of type FLAG, which is one of\n");
		dprintf(2, "                              \"tsonly\" and \"tsaddr\"\n");
		dprintf(2, "  -l,  --preload=NUMBER       send NUMBER packets as fast as possible before\n");
		dprintf(2, "                              falling into normal mode of behavior (root only)\n");
		dprintf(2, "  -p,  --pattern=PATTERN      fill ICMP packet with given pattern (hex)\n");
		dprintf(2, "  -q,  --quiet                quiet output\n");
		dprintf(2, "  -R,  --route                record route\n");
		dprintf(2, "  -s,  --size=NUMBER          send NUMBER data octets\n");
		dprintf(2, "\n");
		dprintf(2, "  -h?, --help                 give this help list\n");
		dprintf(2, "       --usage                give a short usage message\n");
		dprintf(2, "  -V,  --version              print program version\n");
		dprintf(2, "\n");
		dprintf(2, "Mandatory or optional arguments to long options are also mandatory or optional\n");
		dprintf(2, "for any corresponding short options.\n");
		dprintf(2, "\n");
		dprintf(2, "Options marked with (root only) are available only to superuser.\n");
		dprintf(2, "\n");
		dprintf(2, "Report bugs to <kobayashi82@outlook.com>.\n");

		return (1);
	}

#pragma endregion

#pragma region "Usage"

	int usage() {
		dprintf(2, "Usage: ft_ping [-dnrvfqR?V] [-t TYPE] [-c NUMBER] [-i NUMBER] [-T NUM] [-w N]\n");
		dprintf(2, "               [-W N] [-l NUMBER] [-p PATTERN] [-s NUMBER] [--address] [--echo]\n");
		dprintf(2, "               [--mask] [--timestamp] [--type=TYPE] [--count=NUMBER] [--debug]\n");
		dprintf(2, "               [--interval=NUMBER] [--numeric] [--ignore-routing] [--ttl=N]\n");
		dprintf(2, "               [--tos=NUM] [--verbose] [--timeout=N] [--linger=N] [--flood]\n");
		dprintf(2, "               [--ip-timestamp=FLAG] [--preload=NUMBER] [--pattern=PATTERN]\n");
		dprintf(2, "               [--quiet] [--route] [--size=NUMBER] [--help] [--usage] [--version]\n");
		dprintf(2, "               HOST ...\n");

		return (1);
	}

#pragma endregion

#pragma region "Version"

	int version() {
		dprintf(2, "ft_ping 1.0 (based on ping GNU inetutils 2.0).\n");
		dprintf(2, "Copyright (C) 2025 Kobayashi Corp ⓒ.\n");
		dprintf(2, "License WTFPL: DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE.\n");
		dprintf(2, "This is free software: you are free to change and redistribute it.\n");
		dprintf(2, "There is NO WARRANTY, to the extent permitted by law.\n");
		dprintf(2, "\n");
		dprintf(2, "Written by Kobayashi82 (vzurera-).\n");

		return (1);
	}

#pragma endregion

#pragma region "Invalid"

	static int invalid() {
		dprintf(2, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");

		return (2);
	}

#pragma endregion

#pragma region "Parse"

	int parse_options(t_options *options, int argc, char **argv) {
		memset(options, 0, sizeof(t_options));

		struct option long_options[] = {
			// ICMP request types
			{"address", no_argument, 0, 'A'},				// [	--address]
			{"echo", no_argument, 0, 'E'},					// [	--echo]
			{"mask", no_argument, 0, 'K'},					// [	--mask]
			{"timestamp", no_argument, 0, 'M'},				// [	--timestamp]
			{"type", required_argument, 0, 't'},			// [-t, --type=TYPE]
			// All request
			{"count", required_argument, 0, 'c'},			// [-c, --count=NUMBER]
			{"debug", no_argument, 0, 'd'},					// [-d, --debug]	
			{"interval", required_argument, 0, 'i'},		// [-i, --interval=NUMBER]
			{"numeric", no_argument, 0, 'n'},				// [-n, --numeric]	
			{"ignore-routing", no_argument, 0, 'r'},		// [-r, --ignore-routing]
			{"ttl", required_argument, 0, 'L'},				// [	--ttl=N]	
			{"tos", required_argument, 0, 'T'},				// [-T, --tos=NUM]
			{"verbose", no_argument, 0, 'v'},				// [-v, --verbose]
			{"timeout", required_argument, 0, 'w'},			// [-w, --timeout=N]
			{"linger", required_argument, 0, 'W'},			// [-W, --linger=N]
			// Echo requests
			{"flood", no_argument, 0, 'f'},					// [-f, --count=NUMBER]
			{"ip-timestamp", required_argument, 0, 'I'},	// [	--ip-timestamp=FLAG]
			{"preload", required_argument, 0, 'l'},			// [-l, --preload=NUMBER]
			{"pattern", required_argument, 0, 'p'},			// [-p, --pattern=PATTERN]
			{"quiet", no_argument, 0, 'q'},					// [-q, --quiet]
			{"route", no_argument, 0, 'R'},					// [-R, --route]
			{"size", required_argument, 0, 's'},			// [-s, --size=NUMBER]
			// Info
			{"help", no_argument, 0, 'h'},					// [-h?, --help]
			{"usage", no_argument, 0, 'U'},					// [	--usage]
			{"version", no_argument, 0, 'V'},				// [-V, --version]
			{0, 0, 0, 0}
		};

		int opt;
		while ((opt = getopt_long(argc, argv, "t:c:di:nrT:vw:W:fl:p:qRs:h?V", long_options, NULL)) != -1) {
			switch (opt) {
				case 't':	options->type = atoi(optarg);							break;
				case 'c':	options->count = atoi(optarg);							break;
				case 'd':	options->debug = true;									break;
				case 'i':	options->interval = atoi(optarg);						break;
				case 'n':	options->numeric = true;								break;
				case 'r':	options->ignore_routing = true;							break;
				case 'T':	options->tos = atoi(optarg);							break;
				case 'v':	options->verbose = true;								break;
				case 'w':	options->timeout = atoi(optarg);						break;
				case 'W':	options->linger = atoi(optarg);							break;
				case 'f':	options->flood = true;									break;
				case 'l':	options->preload = atoi(optarg);						break;
				case 'p':	memcpy(options->pattern, optarg, strlen(optarg) + 1);	break;
				case 'q':	options->quiet = true;									break;
				case 'R':	options->route = true;									break;
				case 's':	options->size = atoi(optarg);							break;
				case 'V':	return (version());
				case '?':	if (!strcmp(argv[optind - 1], "-?")) return (help()); else return (invalid());
				case 'h':	return (help());
				case 'A' :	options->type = ADDRESS;								break;
				case 'E' :	options->type = ECHO;									break;
				case 'K' :	options->type = MASK;									break;
				case 'M' :	options->type = TIMESTAMP;								break;
				case 'L' :	options->ttl = atoi(optarg);							break;
				case 'I' : 	options->ip_timestamp = atoi(optarg);					break;
				case 'U' :	return (usage());
			}
		}

		if (optind < argc) strcpy(options->target, argv[optind]);
		else {
			dprintf(2, "ft_ping: missing host operand\n");
			dprintf(2, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");

			return (2);
		}

		// validate host

		return (0);
	}

#pragma endregion

#pragma region "Information"

	// BONUS: -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp flags...


	// Usage: ping [OPTION...] HOST ...
	// Send ICMP ECHO_REQUEST packets to network hosts.

	//  Options controlling ICMP request types:
	//       --address              send ICMP_ADDRESS packets (root only)
	//       --echo                 send ICMP_ECHO packets (default)
	//       --mask                 same as --address
	//       --timestamp            send ICMP_TIMESTAMP packets
	//   -t, --type=TYPE            send TYPE packets

	//  Options valid for all request types:

	//   -c, --count=NUMBER         stop after sending NUMBER packets
	//   -d, --debug                set the SO_DEBUG option
	//   -i, --interval=NUMBER      wait NUMBER seconds between sending each packet
	//   -n, --numeric              do not resolve host addresses
	//   -r, --ignore-routing       send directly to a host on an attached network
	//       --ttl=N                specify N as time-to-live
	//   -T, --tos=NUM              set type of service (TOS) to NUM
	//   -v, --verbose              verbose output
	//   -w, --timeout=N            stop after N seconds
	//   -W, --linger=N             number of seconds to wait for response

	//  Options valid for --echo requests:

	//   -f, --flood                flood ping (root only)
	//       --ip-timestamp=FLAG    IP timestamp of type FLAG, which is one of
	//                              "tsonly" and "tsaddr"
	//   -l, --preload=NUMBER       send NUMBER packets as fast as possible before
	//                              falling into normal mode of behavior (root only)
	//   -p, --pattern=PATTERN      fill ICMP packet with given pattern (hex)
	//   -q, --quiet                quiet output
	//   -R, --route                record route
	//   -s, --size=NUMBER          send NUMBER data octets

	//   -?, --help                 give this help list
	//       --usage                give a short usage message
	//   -V, --version              print program version

	// Mandatory or optional arguments to long options are also mandatory or optional
	// for any corresponding short options.

	// Options marked with (root only) are available only to superuser.


	// Usage: ping [-dnrvfqR?V] [-t TYPE] [-c NUMBER] [-i NUMBER] [-T NUM] [-w N]
	//             [-W N] [-l NUMBER] [-p PATTERN] [-s NUMBER] [--address] [--echo]
	//             [--mask] [--timestamp] [--type=TYPE] [--count=NUMBER] [--debug]
	//             [--interval=NUMBER] [--numeric] [--ignore-routing] [--ttl=N]
	//             [--tos=NUM] [--verbose] [--timeout=N] [--linger=N] [--flood]
	//             [--ip-timestamp=FLAG] [--preload=NUMBER] [--pattern=PATTERN]
	//             [--quiet] [--route] [--size=NUMBER] [--help] [--usage] [--version]
	//             HOST ...

#pragma endregion
