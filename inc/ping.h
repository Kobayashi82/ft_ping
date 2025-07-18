/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 13:14:56 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "options.h"

#pragma endregion

#pragma region "Structures"

	// por investigar
	typedef struct s_stats {
		double tmin;								// minimum round trip time
		double tmax;								// maximum round trip time
		double tsum;								// sum of all times, for doing average
		double tsumsq;								// sum of all times squared, for std. dev.
	}	t_stats;

	// por investigar
	typedef struct data {
		int ping_fd;								// Raw socket descriptor
		int ping_type;								// Type of packets to send
		size_t ping_count;							// Number of packets to send
		struct timeval ping_start_time;				// Start time
		size_t ping_interval;						// Number of seconds to wait between sending pkts
		char *ping_hostname;						// Printable hostname
		size_t ping_datalen;						// Length of data
		int ping_ident;								// Our identifier
		void *ping_closure;							// User-defined data

		int ping_cktab_size;						// Runtime info
		char *ping_cktab;							// Runtime info

		unsigned char *ping_buffer;					// I/O buffer
		size_t ping_num_xmit;						// Number of packets transmitted
		size_t ping_num_recv;						// Number of packets received
		size_t ping_num_rept;						// Number of duplicates received
	}	t_data;

	typedef struct s_ping {
		t_options	options;
		t_data		data;
	}	t_ping;

#pragma endregion

#pragma region "Variables"

	extern t_ping ping;

#pragma endregion

#pragma region "Methods"

	void	set_signals();
	int		socket_create_main();
	int		receive_echo_reply(t_options *options, int sockfd);
	void	main_loop();

#pragma endregion
