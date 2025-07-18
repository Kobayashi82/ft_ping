/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 21:46:47 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 20:43:27 by vzurera-         ###   ########.fr       */
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
		if (result) return (result - 1);

		if (set_signals()) return (1);

		// char	packet[MAX_IP_LEN + MAX_ICMP_LEN + MAX_DATA_LEN];
		// size_t	packet_len = 0;

		if (socket_create()) return (1);

		fprintf(stderr, "Socket created OK\n");

		g_ping.running = true;
		while (g_ping.running) {
			;
		}

		fprintf(stderr, "Show stats\n");

		// socket_create_package(sockfd, packet, &packet_len);
		// socket_send(sockfd, packet, packet_len);

		close(g_ping.data.sockfd);

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
