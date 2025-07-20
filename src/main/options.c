/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:45 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/20 20:21:44 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "options.h"

#pragma endregion

#pragma region "Strtoul"

	static int ft_strtoul(char **argv, const char *optarg, size_t *value, size_t max_value, bool allow_zero) {
		char *endptr;
		*value = strtoul(optarg, &endptr, 0);

		if (*endptr)							{ fprintf(stderr, "%s: invalid value (`%s' near `%s')\n", argv[0], optarg, endptr);	return (1); }
		if (!*value && !allow_zero)				{ fprintf(stderr, "%s: option value too small: %s\n", argv[0], optarg);				return (1); }
		if (max_value && *value > max_value)	{ fprintf(stderr, "%s: option value too big: %s\n", argv[0], optarg);				return (1); }

		return (0);
	}

#pragma endregion

#pragma region "Help"

	static int help() {
		fprintf(stderr, "Usage: ft_ping [OPTION...] HOST ...\n");
		fprintf(stderr, "Send ICMP ECHO_REQUEST packets to network hosts.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, " Options controlling ICMP request types:\n");
		fprintf(stderr, "       --address *            send ICMP_ADDRESS packets\t\t\t\t(deprecated)\n");
		fprintf(stderr, "       --echo                 send ICMP_ECHO packets\t\t\t\t(default)\n");
		fprintf(stderr, "       --mask *               same as --address\t\t\t\t\t(deprecated)\n");
		fprintf(stderr, "       --timestamp *          send ICMP_TIMESTAMP packets\t\t\t(deprecated)\n");
		fprintf(stderr, "  -t,  --type=TYPE            send TYPE packets\t\t\t\t\t(only '-t echo' is supported)\n");
		fprintf(stderr, "\n");
		fprintf(stderr, " Options valid for all request types:\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "  -c,  --count=NUMBER         stop after sending NUMBER packets\n");
		fprintf(stderr, "  -d,  --debug                set the SO_DEBUG option\t\t\t\t(kernel-dependent)\n");
		fprintf(stderr, "  -i,  --interval=NUMBER      wait NUMBER seconds between sending each packet\n");
		fprintf(stderr, "  -n,  --numeric              do not resolve host addresses\n");
		fprintf(stderr, "  -r,  --ignore-routing       send directly to a host on an attached network\n");
		fprintf(stderr, "       --ttl=N                specify N as time-to-live\n");
		fprintf(stderr, "  -T,  --tos=NUM              set type of service (TOS) to NUM\t\t\t(ignored by networks)\n");
		fprintf(stderr, "  -v,  --verbose              verbose output\n");
		fprintf(stderr, "  -w,  --timeout=N            stop after N seconds\n");
		fprintf(stderr, "  -W,  --linger=N             number of seconds to wait for response\n");
		fprintf(stderr, "\n");
		fprintf(stderr, " Options valid for --echo requests:\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "  -f,  --flood                flood ping\t\t\t\t\t(root only)\n");
		fprintf(stderr, "       --ip-timestamp=FLAG *  IP timestamp of type FLAG, which is one of\n");
		fprintf(stderr, "                              \"tsonly\" and \"tsaddr\"\t\t\t\t(blocked by networks)\n");
		fprintf(stderr, "  -l,  --preload=NUMBER       send NUMBER packets as fast as possible before\n");
		fprintf(stderr, "                              falling into normal mode of behavior\t\t(root only)\n");
		fprintf(stderr, "  -p,  --pattern=PATTERN      fill ICMP packet with given pattern (hex)\n");
		fprintf(stderr, "  -q,  --quiet                quiet output\n");
		fprintf(stderr, "  -R,  --route *              record route\t\t\t\t\t(blocked by networks)\n");
		fprintf(stderr, "  -s,  --size=NUMBER          send NUMBER data octets\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "  -h?, --help                 give this help list\n");
		fprintf(stderr, "       --usage                give a short usage message\n");
		fprintf(stderr, "  -V,  --version              print program version\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "  * Not implemented.\n\n");
		fprintf(stderr, "Mandatory or optional arguments to long options are also mandatory or optional\n");
		fprintf(stderr, "for any corresponding short options.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "Options marked with (root only) are available only to superuser.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "Report bugs to <kobayashi82@outlook.com>.\n");

		return (1);
	}

#pragma endregion

#pragma region "Usage"

	static int usage() {
		fprintf(stderr, "Usage: ft_ping [-dnrvfqR?V] [-t TYPE] [-c NUMBER] [-i NUMBER] [-T NUM] [-w N]\n");
		fprintf(stderr, "               [-W N] [-l NUMBER] [-p PATTERN] [-s NUMBER] [--address] [--echo]\n");
		fprintf(stderr, "               [--mask] [--timestamp] [--type=TYPE] [--count=NUMBER] [--debug]\n");
		fprintf(stderr, "               [--interval=NUMBER] [--numeric] [--ignore-routing] [--ttl=N]\n");
		fprintf(stderr, "               [--tos=NUM] [--verbose] [--timeout=N] [--linger=N] [--flood]\n");
		fprintf(stderr, "               [--ip-timestamp=FLAG] [--preload=NUMBER] [--pattern=PATTERN]\n");
		fprintf(stderr, "               [--quiet] [--route] [--size=NUMBER] [--help] [--usage] [--version]\n");
		fprintf(stderr, "               HOST ...\n");

		return (1);
	}

#pragma endregion

#pragma region "Version"

	static int version() {
		fprintf(stderr, "ft_ping 1.0 (based on ping GNU inetutils 2.0).\n");
		fprintf(stderr, "Copyright (C) 2025 Kobayashi Corp ⓒ.\n");
		fprintf(stderr, "License WTFPL: DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE.\n");
		fprintf(stderr, "This is free software: you are free to change and redistribute it.\n");
		fprintf(stderr, "There is NO WARRANTY, to the extent permitted by law.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "Written by Kobayashi82 (vzurera-).\n");

		return (1);
	}

#pragma endregion

#pragma region "Invalid"

	static int invalid() {
		fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
		return (2);
	}

#pragma endregion

#pragma region "Validate Host"

	static int resolve_host(t_options *options, const char *hostname) {
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
		memset(options, 0, sizeof(t_options));
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

		options->pid = getpid();
		if (!getuid()) options->is_root = true;

		int opt;
		while ((opt = getopt_long(argc, argv, "t:c:di:nrT:vw:W:fl:p:qRs:h?V", long_options, NULL)) != -1) {
			switch (opt) {
				case 't': {
					if		(!strcasecmp(optarg, "echo"))		{ options->type = ECHO;																					break;		}
					else if	(!strcasecmp(optarg, "address"))	{ fprintf(stderr, "%s: not implemented packet type: address\n", argv[0]);	options->type = ADDRESS;	return (1); }
					else if	(!strcasecmp(optarg, "mask"))		{ fprintf(stderr, "%s: not implemented packet type: mask\n", argv[0]);		options->type = ADDRESS;	return (1); }
					else if	(!strcasecmp(optarg, "timestamp"))	{ fprintf(stderr, "%s: not implemented packet type: timestamp\n", argv[0]);	options->type = TIMESTAMP;	return (1); }
					else										{ fprintf(stderr, "%s: unsupported packet type: %s\n", argv[0], optarg);								return (2); }
				}
				case 'E' :	options->type = ECHO;																					break;
				case 'A' :	fprintf(stderr, "%s: not implemented packet type: address\n", argv[0]);		options->type = ADDRESS;	return (1);
				case 'K' :	fprintf(stderr, "%s: not implemented packet type: mask\n", argv[0]);		options->type = ADDRESS;	return (1);
				case 'M' :	fprintf(stderr, "%s: not implemented packet type: timestamp\n", argv[0]);	options->type = TIMESTAMP;	return (1);
				case 'c':	if (ft_strtoul(argv, optarg, &options->count, 0, true))						return (2);					break;
				case 'L' :	if (ft_strtoul(argv, optarg, &options->ttl, 255, false))					return (2);					break;
				case 'T':	if (ft_strtoul(argv, optarg, &options->tos, 255, true))						return (2);					break;
				case 'w':	if (ft_strtoul(argv, optarg, &options->timeout, INT_MAX, false))			return (2);					break;
				case 'W':	if (ft_strtoul(argv, optarg, &options->linger, INT_MAX, false))				return (2);					break;
				case 's':	if (ft_strtoul(argv, optarg, &options->size, MAX_SIZE, true))				return (2);					break;
				case 'i': {
					char *endptr;
					double value = strtod(optarg, &endptr);
					if (*endptr) {
						fprintf(stderr, "ft_ping: invalid value (`%s' near `%s')\n", optarg, endptr);
						fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
						return (2);
					}
					options->options |= OPT_INTERVAL;
					options->interval = value * 1000;
					if (options->interval < 100) { fprintf(stderr, "%s: option value too small: %s\n", argv[0], optarg); return (2); }
					break;
				}
				case 'l': {
					char *endptr;
					options->preload = strtoul(optarg, &endptr, 0);
      				if (*endptr || options->preload > INT_MAX) { fprintf(stderr, "%s: invalid preload value (%s)\n", argv[0], optarg); return (2); }
					break;
				}
				case 'I' : {
					options->options |= OPT_IPTIMESTAMP;
					if		(!strcasecmp(optarg, "tsonly"))	{ fprintf(stderr, "%s: not implemented timestamp type: tsonly\n", argv[0]);	options->ip_timestamp = OPT_TSONLY;	return (1); }
					else if	(!strcasecmp(optarg, "tsaddr"))	{ fprintf(stderr, "%s: not implemented timestamp type: tsaddr\n", argv[0]);	options->ip_timestamp = OPT_TSADDR;	return (1); }
					else									{ fprintf(stderr, "%s: unsupported timestamp type: %s\n", argv[0], optarg);										return (2); }
				}
				case 'p': {
					int i, c, off;
					options->pattern_len = MAX_PATTERN;
					for (i = 0; *optarg && i < options->pattern_len; ++i) {
						if (sscanf(optarg, "%2x%n", &c, &off) != 1) { fprintf(stderr, "%s: error in pattern near %s\n", argv[0], optarg); return (2); }
						optarg += off;
						options->pattern[i] = c;
					}
					options->pattern_len = i;
					break;
				}
				case 'd':	options->socket_type |= OPT_DEBUG;																break;
				case 'r':	options->socket_type |= OPT_DONTROUTE;															break;
				case 'n':	options->options |= OPT_NUMERIC;																break;
				case 'v':	options->options |= OPT_VERBOSE;																break;
				case 'f':	options->options |= OPT_FLOOD;																	break;
				case 'q':	options->options |= OPT_QUIET;																	break;
				case 'R':	fprintf(stderr, "%s: not implemented record route\n", argv[0]);	options->options |= OPT_ROUTE;	return (1);
				case '?':	if (!strcmp(argv[optind - 1], "-?"))															return (help());	return (invalid());
				case 'h':																									return (help());
				case 'U':																									return (usage());
				case 'V':																									return (version());
			}
		}

		if (optind >= argc) {
			fprintf(stderr, "ft_ping: missing host operand\n");
			fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
			return (2);
		}

		if (resolve_host(options, argv[optind])) { fprintf(stderr, "%s: unknown host\n", argv[0]); return (2); }

		return (0);
	}

#pragma endregion
