/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 17:28:26 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/13 18:10:45 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "options.h"
	#include <string.h>

#pragma endregion

#pragma region "Parse"

	int long_option(t_opt *opt, int argc, char **argv, int i) {
		if (!argv[i]) return (i);

		if (!strcmp(argv[i], "--"))					return ("--");
		if (!strcmp(argv[i], "--address"))			return ("-A");
		if (!strcmp(argv[i], "--echo"))				return ("-E");
		if (!strcmp(argv[i], "--mask"))				return ("-K");
		if (!strcmp(argv[i], "--timestamp"))		return ("-M");
		if (!strcmp(argv[i], "--type="))			return ("-t");
		if (!strcmp(argv[i], "--count="))			return ("-c");
		if (!strcmp(argv[i], "--debug"))			return ("-d");
		if (!strcmp(argv[i], "--interval="))		return ("-i");
		if (!strcmp(argv[i], "--numeric"))			return ("-n");
		if (!strcmp(argv[i], "--ignore-routing"))	return ("-r");
		if (!strcmp(argv[i], "--ttl="))				return ("-L");
		if (!strcmp(argv[i], "--tos="))				return ("-T");
		if (!strcmp(argv[i], "--verbose"))			return ("-v");
		if (!strcmp(argv[i], "--timeout="))			return ("-w");
		if (!strcmp(argv[i], "--linger="))			return ("-W");
		if (!strcmp(argv[i], "--flood"))			return ("-f");
		if (!strcmp(argv[i], "--ip-timestamp="))	return ("-I");
		if (!strcmp(argv[i], "--preload="))			return ("-l");
		if (!strcmp(argv[i], "--pattern="))			return ("-p");
		if (!strcmp(argv[i], "--quiet"))			return ("-q");
		if (!strcmp(argv[i], "--route"))			return ("-R");
		if (!strcmp(argv[i], "--size="))			return ("-s");
		if (!strcmp(argv[i], "--help"))				return ("-h");
		if (!strcmp(argv[i], "--usage"))			return ("-U");
		if (!strcmp(argv[i], "--version"))			return ("-V");

		return (i);
	}

	void parse_options(t_opt *opt, int argc, char **argv) {
		if (!opt || argc < 2 || !argv) return ;

		static char options[] = "AEKMtcdinrLTvwWfIlpqRsh?UV";

		int index = 0;
		int valid_index = 0, invalid_index = 0, done = 0;

		for (int i = 1; i < argc; ++i) {
			if		(argv[i] && !strncmp(argv[i], "--", 2)) i = long_option(opt, argc, argv, i);
			else if	(argv[i] && !strncmp(argv[i], "-", 1)) {

			}
		}

		while (argv && argv[index] && argv[index][0] == '-' && !done) {
			if (valid_index >= MAX_OPTIONS - 1 || invalid_index >= MAX_OPTIONS - 1) { opt->too_many = true; break; }

			char *arg = argv[index];
			if (!strcmp(arg, "--")) { index++; break; } // check ip if no ip, else, ignore the rest

			else if (!strcmp(arg, "--help"))	{ if (!strchr(opt->valid, '?')) opt->valid[valid_index++] = '?'; }
			else if (!strcmp(arg, "--version"))	{ if (!strchr(opt->valid, '#')) opt->valid[valid_index++] = '#'; }

			else {
				for (int i = 1; arg[i]; ++i) {
					if (valid_index >= MAX_OPTIONS - 1 || invalid_index >= MAX_OPTIONS - 1)	{ opt->too_many = true; break; }
					if (strchr(options, arg[i])) {
						if (!strchr(opt->valid, arg[i])) opt->valid[valid_index++] = arg[i];
					} else {
						if (!strchr(opt->invalid, arg[i])) opt->invalid[invalid_index++] = arg[i];
					} 
				}
			} if (!done && !opt->too_many) index++; // ??
		}

		opt->valid[valid_index] = '\0';
		opt->invalid[invalid_index] = '\0';

		return (opt);
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
