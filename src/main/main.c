/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 21:46:47 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/21 22:02:35 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

	#include <signal.h>

#pragma endregion

#pragma region "Variables"

	t_ping g_ping;

#pragma endregion

#pragma region "Signals"

	static void termination_handler(int sig) { (void) sig; g_ping.running = false; }

	static void set_signals() {
		signal(SIGINT,  termination_handler);
		signal(SIGQUIT, termination_handler);
		signal(SIGTERM, termination_handler);
		signal(SIGHUP,  termination_handler);
	}

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {	
		g_ping.name = argv[0];
		int result = parse_options(&g_ping.options, argc, argv);
		if (result)				return (result - 1);
		if (socket_create())	return (1);
		if (packet_create())	return (1);

		set_signals();
		show_header();

		fd_set readfds;
		struct timeval tv, last_send = {0, 0}, start_time;
		gettimeofday(&start_time, NULL);

		g_ping.running = true;
		while (g_ping.running) {
			struct timeval now; gettimeofday(&now, NULL);
			double time_since_start = (now.tv_sec - start_time.tv_sec) + (now.tv_usec - start_time.tv_usec) / 1000000.0;
			double time_since_last = (now.tv_sec - last_send.tv_sec) + (now.tv_usec - last_send.tv_usec) / 1000000.0;
			double interval_sec = (g_ping.options.interval) ? g_ping.options.interval / 1000.0 : DEFAULT_INTERVAL / 1000.0;
			double timeout_to_use = g_ping.options.linger ? g_ping.options.linger : MAX_WAIT;

			bool can_send = !last_send.tv_sec || time_since_last >= interval_sec;

			if (g_ping.options.timeout && time_since_start >= g_ping.options.timeout) { g_ping.running = false; break; }
			int timeout_check_limit = (g_ping.data.sent < PACKETS_SIZE) ? g_ping.data.index : PACKETS_SIZE;
			for (int i = 0; i < timeout_check_limit; ++i) {
				if (g_ping.data.packets[i].sent && !g_ping.data.packets[i].received) {
					double packet_time = (now.tv_sec - g_ping.data.packets[i].time_sent.tv_sec) + (now.tv_usec - g_ping.data.packets[i].time_sent.tv_usec) / 1000000.0;
					if (packet_time >= timeout_to_use) { g_ping.data.packets[i].received = true; g_ping.data.lost++; }
				}
			}

		if ((!g_ping.options.count || g_ping.data.sent + g_ping.data.failed < g_ping.options.count) && can_send) {
			if (packet_send() == 2) { result = 1; break; }
			last_send = now;
		}

		FD_ZERO(&readfds);
		FD_SET(g_ping.data.sockfd, &readfds);
		double select_timeout = 0.1; // 100ms timeout
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
