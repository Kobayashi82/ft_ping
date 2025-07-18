/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:40:48 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 20:53:00 by vzurera-         ###   ########.fr       */
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
	typedef struct data2 {
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
	}	t_data2;

	typedef struct s_data {
		int			sockfd;
	}	t_data;

#pragma endregion

#pragma region "Methods"

	int		socket_create();
	int		package_create(int sockfd, char *packet, size_t *packet_len);
	int		socket_send(int sockfd, char *packet, size_t packet_len);

#pragma endregion
