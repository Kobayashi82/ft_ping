/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:45 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/13 22:39:32 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "options.h"

#pragma endregion

#pragma region "Parse"

	void parse_options(t_options *options, int argc, char **argv) {
		int opt;

		struct option long_options[] = {
			// ICMP request types
			{"address", no_argument, 0, 0},				// [	--address]
			{"echo", no_argument, 0, 0},				// [	--echo]
			{"mask", no_argument, 0, 0},				// [	--mask]
			{"timestamp", no_argument, 0, 0},			// [	--timestamp]
			{"type", required_argument, 0, 't'},		// [-t, --type=TYPE]
			// All request
			{"count", required_argument, 0, 'c'},		// [-c, --count=NUMBER]
			{"debug", no_argument, 0, 'd'},				// [-d, --debug]	
			{"interval", required_argument, 0, 'i'},	// [-i, --interval=NUMBER]
			{"numeric", no_argument, 0, 'n'},			// [-n, --numeric]	
			{"ignore-routing", no_argument, 0, 'r'},	// [-r, --ignore-routing]
			{"ttl", required_argument, 0, 0},			// [	--ttl=N]	
			{"tos", required_argument, 0, 'T'},			// [-T, --tos=NUM]
			{"verbose", no_argument, 0, 'v'},			// [-v, --verbose]
			{"timeout", required_argument, 0, 'w'},		// [-w, --timeout=N]
			{"linger", required_argument, 0, 'W'},		// [-W, --linger=N]
			// Echo requests
			{"flood", no_argument, 0, 'f'},				// [-f, --count=NUMBER]
			{"ip-timestamp", required_argument, 0, 0},	// [	--ip-timestamp=FLAG]
			{"preload", required_argument, 0, 'l'},		// [-l, --preload=NUMBER]
			{"pattern", required_argument, 0, 'p'},		// [-p, --pattern=PATTERN]
			{"quiet", no_argument, 0, 'q'},				// [-q, --quiet]
			{"route", no_argument, 0, 'R'},				// [-R, --route]
			{"size", required_argument, 0, 's'},		// [-s, --size=NUMBER]
			// Info
			{"help", no_argument, 0, 'h'},				// [-h?, --help]
			{"usage", no_argument, 0, 0},				// [	--usage]
			{"version", no_argument, 0, 'V'},			// [-V, --version]
			{0, 0, 0, 0}
		};

		while ((opt = getopt_long(argc, argv, "t:c:di:nrT:vw:W:fl:p:qRs:h?V", long_options, NULL)) != -1) {
			switch (opt) {
				case 't':
					options->type = atoi(optarg);		break;
				case 'c':
					options->count = atoi(optarg);		break;
				case 'd':
					options->debug = true;				break;
				case 'i':
					options->interval = atoi(optarg);	break;
				case 'n':
					options->numeric = true;			break;
				case 'r':
					options->ignore_routing = true;		break;
				case 'T':
					options->tos = atoi(optarg);		break;
				case 'v':
					options->verbose = true;			break;
				case 'w':
					options->timeout = atoi(optarg);	break;
				case 'W':
					options->linger = atoi(optarg);		break;
				case 'f':
					options->flood = true;				break;
				case 'l':
					options->preload = atoi(optarg);	break;
				case 'p':
					memcpy(options->pattern, optarg, strlen(optarg) + 1);	break;
				case 'q':
					options->quiet = true;				break;
				case 'R':
					options->route = true;				break;
				case 's':
					options->size = atoi(optarg);		break;
				case 'V':
					options->version = true;			break;
				case '?':
				case 'h':
					options->help = true;				break;
				case 0:
					if (!strcmp(long_options[optind -1].name, "address"))
						options->address = true;
					else if (!strcmp(long_options[optind -1].name, "echo"))
						options->echo = true;
					else if (!strcmp(long_options[optind -1].name, "mask"))
						options->mask = true;
					else if (!strcmp(long_options[optind -1].name, "timestamp"))
						options->timestamp = true;
					else if (!strcmp(long_options[optind -1].name, "ttl"))
						options->ttl = atoi(optarg);
					else if (!strcmp(long_options[optind -1].name, "usage"))
						options->usage = true;
					else if (!strcmp(long_options[optind -1].name, "ip-timestamp"))
						options->ip_timestamp = true;
					break;
				default:
					// printf("Unknown option: %c\n", opt);
					break;
			}
		}

		if (optind < argc) strcpy(options->target, argv[optind]);
	}

#pragma endregion

#pragma region "Invalid"

	// int invalid_option(char *name, char *opts, char *usage) {
	// 	if (!name) return (0);

	// 	print(STDERR_FILENO, NULL, RESET);
	// 	if (opts) {
	// 		print(STDERR_FILENO, name, JOIN);
	// 		print(STDERR_FILENO, ": -", JOIN);
	// 		print(STDERR_FILENO, opts, JOIN);
	// 		if (ft_strlen(opts) > 1)	print(STDERR_FILENO, ": invalid options\n", JOIN);
	// 		else 						print(STDERR_FILENO, ": invalid option\n", JOIN);
	// 	}
	// 	if (usage) {
	// 		print(STDERR_FILENO, "usage: ", JOIN);
	// 		print(STDERR_FILENO, name, JOIN);
	// 		print(STDERR_FILENO, " ", JOIN);
	// 		print(STDERR_FILENO, usage, JOIN);
	// 		print(STDERR_FILENO, "\n", JOIN);
	// 	}
	// 	print(STDERR_FILENO, "\nTry '", JOIN);
	// 	print(STDERR_FILENO, name, JOIN);
	// 	print(STDERR_FILENO, " --help' for more information\n", PRINT);

	// 	return (0);
	// }

#pragma endregion

#pragma region "Version"

	// int print_version(char *name, char *version) {
	// 	if (!name || !version) return (0);

	// 	print(STDOUT_FILENO, name, RESET);
	// 	print(STDOUT_FILENO, " version ", JOIN);
	// 	print(STDOUT_FILENO, version, JOIN);
	// 	print(STDOUT_FILENO, "\n\n", JOIN);

	// 	print(STDOUT_FILENO, "This is free software: you are free to change and redistribute it.\n", JOIN);
	// 	print(STDOUT_FILENO, "There is NO WARRANTY, to the extent permitted by law.\n\n", JOIN);

	// 	print(STDOUT_FILENO, "Copyright (C) 2025 The "PROYECTNAME" Project.\n\n", JOIN);
	// 	print(STDOUT_FILENO, "Written by "DEVELOPER".\n", PRINT);

	// 	return (0);
	// }

#pragma endregion
