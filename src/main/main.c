/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 21:46:47 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 11:58:09 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		int result = parse_options(&ping.options, argc, argv);
		if (result) return (result - 1);
		set_signals();
		int sockfd = socket_create_main();
		if (sockfd < 0) return (1);
		fprintf(stderr, "Socket created OK\n");
		// while (1) {
		// 	if (receive_echo_reply(&ping.options, sockfd) == 0)
		// 		usleep(1000000); // 1 segundo entre pings
		// 	break;
		// }

		return (result);
	}

#pragma endregion

// Flujo de ejecución:

// 1. main.c → options.c	(parseo)
// 2. main.c → network.c	(resolución DNS)
// 3. main.c → socket.c		(creación socket)
// 4. main.c → ping.c		(bucle principal)
// 5. ping.c → icmp.c		(crear paquetes)
// 6. ping.c → output.c		(mostrar resultados)
// 7. main.c → stats.c		(estadísticas finales)
