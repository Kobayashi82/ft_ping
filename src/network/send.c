/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:37:11 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 16:22:43 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Send"

	int packet_send() {
		// Tomar timestamp justo antes del envío
		struct timeval send_time;
		gettimeofday(&send_time, NULL);
		
		// Enviar inmediatamente sin modificar el paquete
		ssize_t sent = sendto(g_ping.data.sockfd, g_ping.data.packet, g_ping.data.packet_len, 0, (struct sockaddr *)&g_ping.options.sockaddr, sizeof(g_ping.options.sockaddr));

		if (sent < 0) {
			fprintf(stderr, "ft_ping: sendto: %s\n", strerror(errno));
			g_ping.data.failed++; return (1);
		}

		if ((size_t)sent != g_ping.data.packet_len) {
			fprintf(stderr, "ft_ping: partial send: sent %zd bytes, expected %u bytes\n", sent, g_ping.data.packet_len);
			g_ping.data.failed++; return (1);
		}

		// Guardar timestamp en la estructura de tracking
		if (g_ping.data.index < PACKETS_SIZE) {
			struct icmphdr *icmp = (struct icmphdr *)g_ping.data.packet;
			g_ping.data.packets[g_ping.data.index].id = icmp->un.echo.sequence;
			g_ping.data.packets[g_ping.data.index].time_sent = send_time;
			g_ping.data.packets[g_ping.data.index].sent = true;
			g_ping.data.packets[g_ping.data.index].received = false;
			g_ping.data.index++;
		}

		g_ping.data.sent++;
		return (0);
	}

#pragma endregion
