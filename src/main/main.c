/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 21:46:47 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 18:54:17 by vzurera-         ###   ########.fr       */
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
		int result = parse_options(&g_ping.options, argc, argv);
		if (result)				return (result - 1);
		if (set_signals())		return (1);
		if (socket_create())	return (1);

		show_header();

		fd_set readfds;
		struct timeval tv, last_send = {0, 0};

		g_ping.running = true;
		while (g_ping.running) {
			struct timeval now; gettimeofday(&now, NULL);
			double time_since_last = (now.tv_sec - last_send.tv_sec) + (now.tv_usec - last_send.tv_usec) / 1000000.0;
			double interval_sec = (g_ping.options.interval) ? g_ping.options.interval / 1000.0 : 1.0;

			if ((!g_ping.options.count || g_ping.data.sent + g_ping.data.failed < g_ping.options.count) && (!last_send.tv_sec || time_since_last >= interval_sec)) {
				result = packet_create();
				if		(result == 2) { result = 1; break; }
				else if	(result == 1) { result = 0; g_ping.data.failed++; }
				else if (result == 0) { packet_send(); last_send = now; }
			}

			FD_ZERO(&readfds);
			FD_SET(g_ping.data.sockfd, &readfds);
			tv.tv_sec = 0;
			tv.tv_usec = 10000; // 10ms

			int activity = select(g_ping.data.sockfd + 1, &readfds, NULL, NULL, &tv);
			if (activity > 0 && FD_ISSET(g_ping.data.sockfd, &readfds)) packet_receive();

			if (g_ping.options.count && g_ping.data.received + g_ping.data.lost >= g_ping.options.count) { g_ping.running = false; break; }
		}

		close(g_ping.data.sockfd);
		if (!result) show_stats();

		return (0);
	}

#pragma endregion

// Flujo de ejecución:
// 
// main.c → options.c	(parseo)
// main.c → socket.c	(creación socket)
// ping.c → icmp.c		(crear paquetes)
// ping.c → output.c	(mostrar resultados)
// main.c → stats.c		(estadísticas finales)
