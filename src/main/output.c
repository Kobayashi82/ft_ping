/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:54:09 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/21 23:11:42 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

	#include <math.h>

#pragma endregion

#pragma region "Header"

	void show_header() {
		char addr_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &g_ping.options.sockaddr.sin_addr, addr_str, INET_ADDRSTRLEN);

		size_t data_size = (g_ping.options.size) ? g_ping.options.size : DEFAULT_SIZE;

		printf("PING %s (%s): %zu data bytes\n", g_ping.options.hostname, addr_str, data_size);
		// if (g_ping.options.options & OPT_VERBOSE) printf (", id 0x%04x = %u", ping->ping_ident, ping->ping_ident);
	}

#pragma endregion

#pragma region "Stats"

	#pragma region "Show"

		void show_stats() {
			char addr_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &g_ping.options.sockaddr.sin_addr, addr_str, INET_ADDRSTRLEN);

			int total_sent = g_ping.data.sent;
			int total_received = g_ping.data.received;
			int total_lost = total_sent - total_received;
			int loss_percent = (total_sent > 0) ? (total_lost * 100) / total_sent : 0;

			printf("--- %s ping statistics ---\n", g_ping.options.hostname);
			printf("%d packets transmitted, %d packets received", total_sent, total_received);
			printf(", %d%% packet loss\n", loss_percent);

			if (g_ping.data.rtt_count > 0) {
				double avg_rtt = g_ping.data.sum_rtt / g_ping.data.rtt_count;
				double variance = 0.0;
				
				if (g_ping.data.rtt_count > 1) {
					double mean_sq = g_ping.data.sum_rtt_sq / g_ping.data.rtt_count;
					variance = mean_sq - (avg_rtt * avg_rtt);
					if (variance < 0) variance = 0.0;
				}
				
				double stddev = sqrt(variance);
				
				printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", 
					g_ping.data.min_rtt, avg_rtt, g_ping.data.max_rtt, stddev);
			}
		}

	#pragma endregion

	#pragma region "Update"

		void update_stats(double rtt) {
			if (rtt < 0) return;
			
			if (g_ping.data.min_rtt < 0 || rtt < g_ping.data.min_rtt) g_ping.data.min_rtt = rtt;
			if (g_ping.data.max_rtt < 0 || rtt > g_ping.data.max_rtt) g_ping.data.max_rtt = rtt;
			
			g_ping.data.sum_rtt += rtt;
			g_ping.data.sum_rtt_sq += rtt * rtt;
			g_ping.data.rtt_count++;
		}

	#pragma endregion

#pragma endregion

