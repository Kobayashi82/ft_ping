/* *************************************************		bool flood_mode = (g_ping.options.options & OPT_FLOOD) && g_ping.options.is_root;
		bool can_send = !last_send.tv_sec || (!flood_mode && time_since_last >= interval_sec) || 
						(flood_mode && time_since_last >= 0.01); // 10ms in flood mode*********************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 21:46:47 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/20 23:29:24 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Variables"

	t_ping g_ping;

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		g_ping.name = argv[0];
		int result = parse_options(&g_ping.options, argc, argv);
		if (result)				return (result - 1);
		if (set_signals())		return (1);
		if (socket_create())	return (1);

		show_header();

		fd_set readfds;
		struct timeval tv, last_send = {0, 0}, start_time;
		gettimeofday(&start_time, NULL);

		bool original_flood = (g_ping.options.options & OPT_FLOOD) != 0;
		size_t preload_remaining = g_ping.options.preload;
		if (g_ping.options.preload && g_ping.options.is_root) { g_ping.options.options |= OPT_FLOOD; g_ping.in_preload = true; }

		g_ping.running = true;
		while (g_ping.running) {
			struct timeval now; gettimeofday(&now, NULL);
			double time_since_start = (now.tv_sec - start_time.tv_sec) + (now.tv_usec - start_time.tv_usec) / 1000000.0;
			double time_since_last = (now.tv_sec - last_send.tv_sec) + (now.tv_usec - last_send.tv_usec) / 1000000.0;
			double interval_sec = (g_ping.options.interval) ? g_ping.options.interval / 1000.0 : DEFAULT_INTERVAL / 1000.0;
			double timeout_to_use = g_ping.options.linger ? g_ping.options.linger : MAX_WAIT;

			bool flood_mode = (g_ping.options.options & OPT_FLOOD) && g_ping.options.is_root;
			bool can_send = !last_send.tv_sec || (!flood_mode && time_since_last >= interval_sec) || (flood_mode && time_since_last >= 0.01);
			bool all_packets_sent = g_ping.options.count && (g_ping.data.sent + g_ping.data.failed >= g_ping.options.count);

			if (g_ping.options.timeout && time_since_start >= g_ping.options.timeout) { g_ping.running = false; break; }

			int timeout_check_limit = (g_ping.data.sent < PACKETS_SIZE) ? g_ping.data.index : PACKETS_SIZE;
			for (int i = 0; i < timeout_check_limit; ++i) {
				if (g_ping.data.packets[i].sent && !g_ping.data.packets[i].received) {
					double packet_time = (now.tv_sec - g_ping.data.packets[i].time_sent.tv_sec) + (now.tv_usec - g_ping.data.packets[i].time_sent.tv_usec) / 1000000.0;
					if (packet_time >= timeout_to_use) { g_ping.data.packets[i].received = true; g_ping.data.lost++; }
				}
			}

			if ((!g_ping.options.count || g_ping.data.sent + g_ping.data.failed < g_ping.options.count) && can_send) {
				result = packet_create();
				if		(result == 2) { result = 1; break; }
				else if	(result == 1) { result = 0; g_ping.data.failed++; }
				else if (result == 0) {
					if (packet_send() == 2) { result = 1; break; }
					last_send = now;

					if (preload_remaining > 0) {
						preload_remaining--;
						if (preload_remaining == 0 && !original_flood) { g_ping.options.options &= ~OPT_FLOOD; g_ping.in_preload = false; }
					}
				}
			}

			if (flood_mode && all_packets_sent && time_since_last > timeout_to_use) { g_ping.running = false; break; }

			FD_ZERO(&readfds);
			FD_SET(g_ping.data.sockfd, &readfds);

			double select_timeout = (flood_mode) ? 0.001 : 0.1; // 1ms for flood, 100ms normal
			tv.tv_sec = (long)select_timeout;
			tv.tv_usec = (long)((select_timeout - tv.tv_sec) * 1000000);

			int activity = select(g_ping.data.sockfd + 1, &readfds, NULL, NULL, &tv);
			if (activity > 0 && FD_ISSET(g_ping.data.sockfd, &readfds)) packet_receive();

			if (g_ping.options.count) {
				size_t total_processed = g_ping.data.received + g_ping.data.lost + g_ping.data.corrupted;
				if (total_processed >= g_ping.options.count) { g_ping.running = false; break; }
			}
		}

		close(g_ping.data.sockfd);
		if (!result) show_stats();

		return (result);
	}

#pragma endregion
