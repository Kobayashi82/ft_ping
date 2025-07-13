/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 17:28:32 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/13 17:32:25 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <stdbool.h>

	#define MAX_OPTIONS			128

#pragma endregion

#pragma region "Variables"

	#pragma region "Structures"

		typedef struct s_opt {
			const char		*options;
			char			valid[MAX_OPTIONS];
			char			invalid[MAX_OPTIONS];
			bool			too_many;
			int				opt_args[10];					// -t -c -i -L (--ttl) -w -W -T -l -s -I (--ip-timestamp)
			unsigned char	pattern[65507];					// -p (length is determine by the option --size or "65535 - IP header - ICMP header")
			char			target[254];					// IP Address or hostname
		}	t_opt;

	#pragma endregion

	// -I 	--ip-timestamp
	// -U	--usage
	// -L	--ttl
	// -A	--address
	// -E	--echo
	// -K	--mask
	// -M	--timestamp

	// Hostnames must not exceed 253 bytes in total, with each label (between dots) up to 63 bytes.
	// Allowed characters are alphanumeric and hyphens (-).
	// Hyphens cannot appear at the start or end of a label, nor can two hyphens appear consecutively.
	// Dots (.) are used to separate labels and cannot appear consecutively or at the start or end of the hostname.

#pragma endregion

#pragma region "Methods"

	void parse_options(t_opt *opt, int argc, char **argv);

#pragma endregion
