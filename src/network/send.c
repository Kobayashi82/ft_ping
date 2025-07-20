/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:37:11 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/20 23:45:13 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Send"

	int packet_send() {
		struct timeval send_time;
		gettimeofday(&send_time, NULL);

		ssize_t sent = sendto(g_ping.data.sockfd, g_ping.data.packet, g_ping.data.packet_len, 0, (struct sockaddr *)&g_ping.options.sockaddr, sizeof(g_ping.options.sockaddr));

		if (sent < 0) {
			fprintf(stderr, "%s: sending packet: %s\n", g_ping.name, strerror(errno));
			g_ping.data.failed++; return (2);
		}

		if ((size_t)sent != g_ping.data.packet_len) {
			fprintf(stderr, "%s: partial send: sent %zd bytes, expected %u bytes\n", g_ping.name, sent, g_ping.data.packet_len);
			g_ping.data.failed++; return (1);
		}

		struct icmphdr *icmp = (struct icmphdr *)g_ping.data.packet;
		if (g_ping.data.packet_len >= sizeof(struct icmphdr)) {
			int circular_index = g_ping.data.sent % PACKETS_SIZE;
			g_ping.data.packets[circular_index].id = ntohs(icmp->un.echo.sequence);
			g_ping.data.packets[circular_index].time_sent = send_time;
			g_ping.data.packets[circular_index].sent = true;
			g_ping.data.packets[circular_index].received = false;
			
			if (g_ping.data.index < PACKETS_SIZE) g_ping.data.index++;
		}

		g_ping.data.sent++;
		
		if ((g_ping.options.options & OPT_FLOOD) && g_ping.options.is_root && !g_ping.in_preload) { printf("."); fflush(stdout); }
		
		return (0);
	}

#pragma endregion
