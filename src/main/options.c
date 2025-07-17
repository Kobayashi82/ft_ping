/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:45 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/17 21:31:08 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "options.h"

#pragma endregion

#pragma region "Strtoul"

	static int ft_strtoul(const char *optarg, size_t *value, size_t max_value, bool allow_zero) {
		char *endptr;
		*value = strtoul(optarg, &endptr, 0);

		if (*endptr)							{ dprintf(2, "ft_ping: invalid value (`%s' near `%s')\n", optarg, endptr);	return (1); }
		if (!*value && !allow_zero)				{ dprintf(2, "ft_ping: option value too small: %s\n", optarg);				return (1); }
		if (max_value && *value > max_value)	{ dprintf(2, "ft_ping: option value too big: %s\n", optarg);				return (1); }

		return (0);
	}

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

	static int usage() {
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

	static int version() {
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

#pragma region "Validate Host"

	int validate_host(t_options *options, const char *hostname) {
		struct addrinfo hints, *res;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_RAW;
		hints.ai_flags = AI_CANONNAME;

		if (getaddrinfo(hostname, NULL, &hints, &res)) return (1);

		memcpy(&options->sockaddr, res->ai_addr, res->ai_addrlen);
		strlcpy(options->hostname, res->ai_canonname ? res->ai_canonname : hostname, sizeof(options->hostname));

		inet_ntop(AF_INET, &options->sockaddr.sin_addr, options->host, INET_ADDRSTRLEN);
		freeaddrinfo(res);

		return (0);
	}

#pragma endregion

#pragma region "Parse"

	int parse_options(t_options *options, int argc, char **argv) {
		struct option long_options[] = {
			// ICMP request types
			{"address",			no_argument,		0, 'A'},	// [	--address]
			{"echo",			no_argument,		0, 'E'},	// [	--echo]
			{"mask",			no_argument,		0, 'K'},	// [	--mask]
			{"timestamp",		no_argument,		0, 'M'},	// [	--timestamp]
			{"type",			required_argument,	0, 't'},	// [-t, --type=TYPE]
			// All request
			{"count",			required_argument,	0, 'c'},	// [-c, --count=NUMBER]
			{"debug",			no_argument,		0, 'd'},	// [-d, --debug]	
			{"interval",		required_argument,	0, 'i'},	// [-i, --interval=NUMBER]
			{"numeric",			no_argument,		0, 'n'},	// [-n, --numeric]	
			{"ignore-routing",	no_argument,		0, 'r'},	// [-r, --ignore-routing]
			{"ttl",				required_argument,	0, 'L'},	// [	--ttl=N]	
			{"tos",				required_argument,	0, 'T'},	// [-T, --tos=NUM]
			{"verbose",			no_argument,		0, 'v'},	// [-v, --verbose]
			{"timeout",			required_argument,	0, 'w'},	// [-w, --timeout=N]
			{"linger",			required_argument,	0, 'W'},	// [-W, --linger=N]
			// Echo requests
			{"flood",			no_argument,		0, 'f'},	// [-f, --count=NUMBER]
			{"ip-timestamp",	required_argument,	0, 'I'},	// [	--ip-timestamp=FLAG]
			{"preload",			required_argument,	0, 'l'},	// [-l, --preload=NUMBER]
			{"pattern",			required_argument,	0, 'p'},	// [-p, --pattern=PATTERN]
			{"quiet",			no_argument,		0, 'q'},	// [-q, --quiet]
			{"route",			no_argument,		0, 'R'},	// [-R, --route]
			{"size",			required_argument,	0, 's'},	// [-s, --size=NUMBER]
			// Info
			{"help",			no_argument,		0, 'h'},	// [-h?, --help]
			{"usage",			no_argument,		0, 'U'},	// [	--usage]
			{"version",			no_argument,		0, 'V'},	// [-V, --version]
			{0, 0, 0, 0}
		};

		if (!getuid()) options->is_root = true;

		int		opt;
		while ((opt = getopt_long(argc, argv, "t:c:di:nrT:vw:W:fl:p:qRs:h?V", long_options, NULL)) != -1) {
			switch (opt) {
				case 't': {
					if		(!strcasecmp(optarg, "address"))	options->type = ADDRESS;
					else if	(!strcasecmp(optarg, "echo"))		options->type = ECHO;
					else if	(!strcasecmp(optarg, "mask"))		options->type = ADDRESS;
					else if	(!strcasecmp(optarg, "timestamp"))	options->type = TIMESTAMP;
					else { dprintf(2, "ft_ping: unsupported packet type: %s\n", optarg);	return (2); }
					break;
				}
				case 'A' :	options->type = ADDRESS;															break;
				case 'E' :	options->type = ECHO;																break;
				case 'K' :	options->type = ADDRESS;															break;
				case 'M' :	options->type = TIMESTAMP;															break;
				case 'c':	if (ft_strtoul(optarg, &options->count, 0, true))				return (2);			break;
				case 'L' :	if (ft_strtoul(optarg, &options->ttl, 255, false))				return (2);			break;
				case 'T':	if (ft_strtoul(optarg, &options->tos, 255, true))				return (2);			break;
				case 'w':	if (ft_strtoul(optarg, &options->timeout, INT_MAX, false))		return (2);			break;
				case 'W':	if (ft_strtoul(optarg, &options->linger, INT_MAX, false))		return (2);			break;
				case 's':	if (ft_strtoul(optarg, &options->size, MAX_DATA_LEN, true))		return (2);			break;
				case 'i': {
					char *endptr;
					double value = strtod(optarg, &endptr);
					if (*endptr) { dprintf(2, "ft_ping: invalid value (`%s' near `%s')\n", optarg, endptr); return (2); }
					options->options |= OPT_INTERVAL;
					options->interval = value * PING_PRECISION;
					if (!options->is_root && options->interval < PING_MIN_USER_INTERVAL) { dprintf(2, "ft_ping: option value too small: %s\n", optarg); return (2); }
					break;
				}
				case 'l': {
					char *endptr;
					options->preload = strtoul(optarg, &endptr, 0);
      				if (*endptr || options->preload > INT_MAX) {
						dprintf(2, "ft_ping: invalid preload value (%s)\n", optarg); return (2);
					}
					break;
				}
				case 'I' : {
					options->options |= OPT_IPTIMESTAMP;
					if		(!strcasecmp(optarg, "tsonly"))		options->ip_timestamp = OPT_TSONLY;
					else if	(!strcasecmp(optarg, "tsaddr"))		options->ip_timestamp = OPT_TSADDR;
					else { dprintf(2, "ft_ping: unsupported timestamp type: %s\n", optarg); return (2); }
					break;
				}
				case 'p': {
					int i, c, off;
					options->pattern_len = MAX_PATTERN;
					for (i = 0; *optarg && i < options->pattern_len; ++i) {
						if (sscanf(optarg, "%2x%n", &c, &off) != 1) { dprintf(2, "ft_ping: error in pattern near %s\n", optarg); return (2); }
						optarg += off;
						options->pattern[i] = c;
					}
					options->pattern_len = i;
					break;
				}
				case 'd':	options->socket_type |= SO_DEBUG;													break;
				case 'r':	options->socket_type |= SO_DONTROUTE;												break;
				case 'n':	options->options |= OPT_NUMERIC;													break;
				case 'v':	options->options |= OPT_VERBOSE;													break;
				case 'f':	options->options |= OPT_FLOOD;														break;
				case 'q':	options->options |= OPT_QUIET;														break;
				case 'R':	options->options |= OPT_ROUTE;														break;
				case '?':	if (!strcmp(argv[optind - 1], "-?"))							return (help()); 	return (invalid());
				case 'h':																	return (help());
				case 'U' :																	return (usage());
				case 'V':																	return (version());
			}
		}

		if (optind >= argc) {
			dprintf(2, "ft_ping: missing host operand\n");
			dprintf(2, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
			return (2);
		}

		if (validate_host(options, argv[optind])) { dprintf(2, "ft_ping: unknown host\n"); return (2); }

		return (0);
	}

#pragma endregion
