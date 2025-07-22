/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:45 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 21:17:40 by vzurera-         ###   ########.fr       */
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
		fprintf(stderr, "  -c,  --count=NUM            stop after sending NUM packets\n");
		fprintf(stderr, "  -d,  --debug                set the SO_DEBUG option (kernel-dependent)\n");
		fprintf(stderr, "  -i,  --interval=NUM         wait NUM seconds between sending each packet\n");
		fprintf(stderr, "  -n,  --numeric              do not resolve host addresses\n");
		fprintf(stderr, "  -r,  --ignore-routing       send directly to a host on an attached network\n");
		fprintf(stderr, "       --ttl=N                specify N as time-to-live\n");
		fprintf(stderr, "  -T,  --tos=NUM              set type of service (TOS) to NUM (often ignored)\n");
		fprintf(stderr, "                                 - Low Delay:              16\n");
		fprintf(stderr, "                                 - High Reliability:       4\n");
		fprintf(stderr, "                                 - High Throughput:        8\n");
		fprintf(stderr, "                                 - High Priority:          136\n");
		fprintf(stderr, "                                 - Expedited Forwarding:   184\n");
		fprintf(stderr, "  -v,  --verbose              verbose output\n");
		fprintf(stderr, "  -w,  --timeout=NUM          stop after NUM seconds\n");
		fprintf(stderr, "  -W,  --linger=NUM           number of seconds to wait for response\n");
		fprintf(stderr, "  -p,  --pattern=PATTERN      fill ICMP packet with given pattern (hex)\n");
		fprintf(stderr, "  -q,  --quiet                quiet output\n");
		fprintf(stderr, "  -s,  --size=NUM             send NUM data octets\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "  -h?, --help                 give this help list\n");
		fprintf(stderr, "       --usage                give a short usage message\n");
		fprintf(stderr, "  -V,  --version              print program version\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "Mandatory or optional arguments to long options are also mandatory or optional\n");
		fprintf(stderr, "for any corresponding short options.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "Report bugs to <kobayashi82@outlook.com>.\n");

		return (1);
	}

#pragma endregion

#pragma region "Usage"

	static int usage() {
		fprintf(stderr, "Usage: ft_ping [-c NUM, --count=NUMBER] [-i NUM, --interval=NUM]\n");
		fprintf(stderr, "               [-w NUM, --timeout=NUM]  [-W NUM, --linger=NUM]\n");
		fprintf(stderr, "               [-s NUM, --size=NUM]     [-p PATTERN, --pattern=PATTERN]\n");
		fprintf(stderr, "               [-T NUM, --tos=NUM]      [-r, --ignore-routing] [--ttl=NUM]\n");
		fprintf(stderr, "               [-d, --debug] [-n, --numeric] [-v, --verbose] [-q, --quiet]\n");
		fprintf(stderr, "               [-h?, --help] [-u, --usage]   [-V, --version]\n");
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

	static int validate_host(t_options *options, const char *hostname) {
		struct addrinfo hints, *res;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_RAW;
		hints.ai_flags = AI_CANONNAME;

		if (getaddrinfo(hostname, NULL, &hints, &res)) return (1);

		memcpy(&options->sockaddr, res->ai_addr, res->ai_addrlen);
		snprintf(options->hostname, sizeof(options->hostname), "%s", res->ai_canonname ? res->ai_canonname : hostname);

		inet_ntop(AF_INET, &options->sockaddr.sin_addr, options->host, INET_ADDRSTRLEN);
		freeaddrinfo(res);

		return (0);
	}

#pragma endregion

#pragma region "Parse"

	int parse_options(t_options *options, int argc, char **argv) {
		memset(options, 0, sizeof(t_options));
		options->size = SIZE_MAX;
		struct option long_options[] = {
			{"count",			required_argument,	0, 'c'},	// [-c, --count=NUM]
			{"debug",			no_argument,		0, 'd'},	// [-d, --debug]	
			{"interval",		required_argument,	0, 'i'},	// [-i, --interval=NUM]
			{"numeric",			no_argument,		0, 'n'},	// [-n, --numeric]	
			{"ignore-routing",	no_argument,		0, 'r'},	// [-r, --ignore-routing]
			{"ttl",				required_argument,	0, 'L'},	// [	--ttl=N]	
			{"tos",				required_argument,	0, 'T'},	// [-T, --tos=NUM]
			{"verbose",			no_argument,		0, 'v'},	// [-v, --verbose]
			{"timeout",			required_argument,	0, 'w'},	// [-w, --timeout=N]
			{"linger",			required_argument,	0, 'W'},	// [-W, --linger=N]
			{"pattern",			required_argument,	0, 'p'},	// [-p, --pattern=PATTERN]
			{"quiet",			no_argument,		0, 'q'},	// [-q, --quiet]
			{"size",			required_argument,	0, 's'},	// [-s, --size=NUM]
			{"help",			no_argument,		0, 'h'},	// [-h?, --help]
			{"usage",			no_argument,		0, 'u'},	// [	--usage]
			{"version",			no_argument,		0, 'V'},	// [-V, --version]
			{0, 0, 0, 0}
		};

		options->pid = getpid();

		int opt;
		while ((opt = getopt_long(argc, argv, "c:di:nrT:vw:W:p:qs:h?uV", long_options, NULL)) != -1) {
			switch (opt) {
				case 'c':	if (ft_strtoul(argv, optarg, &options->count, 0, true))						return (2);					break;
				case 'L':	if (ft_strtoul(argv, optarg, &options->ttl, 255, false))					return (2);					break;
				case 'T':	if (ft_strtoul(argv, optarg, &options->tos, 255, true))						return (2);					break;
				case 'w':	if (ft_strtoul(argv, optarg, &options->timeout, INT_MAX, false))			return (2);					break;
				case 'W':	if (ft_strtoul(argv, optarg, &options->linger, INT_MAX, false))				return (2);					break;
				case 's':	if (ft_strtoul(argv, optarg, &options->size, MAX_SIZE, true))				return (2);					break;
				case 'i': {
					char *endptr;
					double value = strtod(optarg, &endptr);
					if (*endptr) {
						fprintf(stderr, "ft_ping: invalid value (`%s' near `%s')\n", optarg, endptr);
						invalid(); return (2);
					}
					options->options |= OPT_INTERVAL;
					options->interval = value * 1000;
					if (options->interval < 100) { fprintf(stderr, "%s: option value too small: %s\n", argv[0], optarg); return (2); }
					break;
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
				case 'q':	options->options |= OPT_QUIET;																	break;
				case '?':	if (!strcmp(argv[optind - 1], "-?"))															return (help());	return (invalid());
				case 'h':																									return (help());
				case 'u':																									return (usage());
				case 'V':																									return (version());
			}
		}

		if (optind >= argc) {
			fprintf(stderr, "ft_ping: missing host operand\n");
			invalid(); return (2);
		}

		if (validate_host(options, argv[optind])) { fprintf(stderr, "%s: unknown host\n", argv[0]); return (2); }

		return (0);
	}

#pragma endregion
