/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:54:09 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/20 18:00:09 by vzurera-         ###   ########.fr       */
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

	void show_stats() {
		char addr_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &g_ping.options.sockaddr.sin_addr, addr_str, INET_ADDRSTRLEN);

		int total_sent = g_ping.data.sent;
		int total_received = g_ping.data.received;
		int total_lost = total_sent - total_received;
		int loss_percent = (total_sent > 0) ? (total_lost * 100) / total_sent : 0;

		printf("--- %s ping statistics ---\n", g_ping.options.hostname);
		printf("%d packets transmitted, %d packets received", total_sent, total_received);

		if (g_ping.data.duplicated > 0) printf(", +%zu duplicates", g_ping.data.duplicated);

		printf(", %d%% packet loss\n", loss_percent);

		if (total_received > 0) {
			double min_time = -1, max_time = -1, sum_time = 0;
			int count = 0;

			for (int i = 0; i < g_ping.data.index; i++) {
				if (g_ping.data.packets[i].received && g_ping.data.packets[i].sent) {
					struct timeval *sent = &g_ping.data.packets[i].time_sent;
					struct timeval *recv = &g_ping.data.packets[i].time_received;
					double rtt = (recv->tv_sec - sent->tv_sec) * 1000.0 + (recv->tv_usec - sent->tv_usec) / 1000.0;

					if (min_time < 0 || rtt < min_time) min_time = rtt;
					if (max_time < 0 || rtt > max_time) max_time = rtt;
					sum_time += rtt;
					count++;
				}
			}

			if (count > 0) {
				double avg_time = sum_time / count;
				double variance = 0;
				for (int i = 0; i < g_ping.data.index; i++) {
					if (g_ping.data.packets[i].received && g_ping.data.packets[i].sent) {
						struct timeval *sent = &g_ping.data.packets[i].time_sent;
						struct timeval *recv = &g_ping.data.packets[i].time_received;
						double rtt = (recv->tv_sec - sent->tv_sec) * 1000.0 + (recv->tv_usec - sent->tv_usec) / 1000.0;
						double diff = rtt - avg_time;
						variance += diff * diff;
					}
				}
				double stddev = (count > 1) ? sqrt(variance / (count - 1)) : 0.0;

				printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", min_time, avg_time, max_time, stddev);
			}
		}
	}

#pragma endregion

