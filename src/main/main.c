/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 21:46:47 by vzurera-          #+#    #+#             */
/*   Updated: 2026/02/09 23:47:21 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

	#include <signal.h>
	#include <sys/select.h>

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

void initialize(char *arg) {
	g_ping.name				= arg;
	g_ping.data.min_rtt		= -1.0;
	g_ping.data.max_rtt		= -1.0;
	g_ping.data.sum_rtt		= 0.0;
	g_ping.data.sum_rtt_sq	= 0.0;
	g_ping.data.rtt_count	= 0;
	g_ping.data.sockfd		= -1;
	g_ping.data.sequence	= 0;
}

#pragma region "Main"

	int main(int argc, char **argv) {
		fd_set			readfds;
		struct timeval	start_time = {0, 0}, last_send = {0, 0};
		int				result = 0;
		double			implicit_timeout = 0.0;

		initialize(argv[0]);
		if ((result = parse_options(&g_ping.options, argc, argv)))	return (result == 2);
		if (socket_create())										return (1);
		if (packet_create())										return (1);
		set_signals();
		show_header();

		gettimeofday(&start_time, NULL);
		if (!g_ping.options.timeout && g_ping.options.count) {
			double interval_sec = (g_ping.options.interval) ? g_ping.options.interval / 1000.0 : DEFAULT_INTERVAL / 1000.0;
			size_t packet_timeout = (g_ping.options.linger) ? g_ping.options.linger : PACKET_TIMEOUT;
			implicit_timeout = (g_ping.options.count - 1) * interval_sec;

			if (packet_timeout > interval_sec)	implicit_timeout += (packet_timeout - interval_sec);
			else								implicit_timeout += packet_timeout;
		}
		g_ping.running = true;

		while (g_ping.running) {
			struct timeval now;
			gettimeofday(&now, NULL);

			double time_since_start = (now.tv_sec - start_time.tv_sec) + ((now.tv_usec - start_time.tv_usec) / 1000000.0);
			double time_since_last = (now.tv_sec - last_send.tv_sec) + ((now.tv_usec - last_send.tv_usec) / 1000000.0);
			double interval_sec = (g_ping.options.interval) ? g_ping.options.interval / 1000.0 : DEFAULT_INTERVAL / 1000.0;

			// Check total timeout	(-w or implicit deadline)
			double effective_timeout = (g_ping.options.timeout) ? (double)g_ping.options.timeout : implicit_timeout;
			if (effective_timeout > 0.0 && time_since_start > effective_timeout) {
				for (size_t i = 0; i < MAX_TRACKED_SEQ; ++i) {
					if (g_ping.data.pending[i]) {
						g_ping.data.pending[i] = false;
						g_ping.data.lost++;
					}
				}
				g_ping.running = false;
				break;
			}

			if ((!g_ping.options.count || g_ping.data.sent + g_ping.data.failed < g_ping.options.count) && time_since_last >= interval_sec) {
				if (packet_send() == 2) {
					result = 1;
					break;
				}
				last_send = now;
			}

			struct timeval select_timeout = {(long)SELECT_TIMEOUT, (long)((SELECT_TIMEOUT - (long)SELECT_TIMEOUT) * 1000000)};
			FD_ZERO(&readfds);
			FD_SET(g_ping.data.sockfd, &readfds);
			int activity = select(g_ping.data.sockfd + 1, &readfds, NULL, NULL, &select_timeout);
			if (activity < 0) {
				if (errno != EINTR) {
					fprintf(stderr, "%s: select failed\n", g_ping.name);
					result = 1;
					break;
				}
				continue;
			} else if (FD_ISSET(g_ping.data.sockfd, &readfds)) packet_receive();

			// Check packet timeout	(-W)
			size_t packet_timeout = (g_ping.options.linger) ? g_ping.options.linger : PACKET_TIMEOUT;
			gettimeofday(&now, NULL);
			for (size_t i = 0; i < MAX_TRACKED_SEQ; ++i) {
				if (!g_ping.data.pending[i]) continue;
				double elapsed = (now.tv_sec - g_ping.data.send_times[i].tv_sec) + ((now.tv_usec - g_ping.data.send_times[i].tv_usec) / 1000000.0);
				if (elapsed >= packet_timeout) {
					g_ping.data.pending[i] = false;
					g_ping.data.lost++;
				}
			}

			if (g_ping.options.count && g_ping.data.received + g_ping.data.lost + g_ping.data.corrupted >= g_ping.options.count) {
				g_ping.running = false;
				break;
			}
		}

		close(g_ping.data.sockfd);
		if (!result) show_stats();

		return (result);
	}

#pragma endregion
