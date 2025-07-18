/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 21:46:47 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 01:23:54 by vzurera-         ###   ########.fr       */
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

		fprintf(stderr, "Socket created OK\n");

		if (pthread_create(&g_ping.thread, NULL, packet_receive, NULL)) { fprintf(stderr, "ft_ping: thread creation failed\n"); return (1); }

		g_ping.running = false;
		while (g_ping.running) {
			packet_create();
			packet_send();
		}

	    pthread_cancel(g_ping.thread);
		pthread_join(g_ping.thread, NULL);
		close(g_ping.data.sockfd);
		show_stats();	

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
