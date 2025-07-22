/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:37:11 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 20:27:50 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Send"

	int packet_send() {
		static int	sequence = 0;
		struct timeval send_time;

		struct icmphdr *icmp = (struct icmphdr *)g_ping.data.packet;
		if (g_ping.data.packet_len < sizeof(struct icmphdr)) {
			fprintf(stderr, "%s: sending packet: packet too short: %u bytes\n", g_ping.name, g_ping.data.packet_len);
			g_ping.data.failed++; return (1);
		}

		icmp->un.echo.sequence = htons(sequence++);
		gettimeofday(&send_time, NULL);

		if (g_ping.data.packet_len >= sizeof(struct icmphdr) + sizeof(struct timeval))
			memcpy(g_ping.data.packet + sizeof(struct icmphdr), &send_time, sizeof(send_time));

		icmp->checksum = 0;
		icmp->checksum = checksum(g_ping.data.packet, g_ping.data.packet_len);
		ssize_t sent = sendto(g_ping.data.sockfd, g_ping.data.packet, g_ping.data.packet_len, 0, (struct sockaddr *)&g_ping.options.sockaddr, sizeof(g_ping.options.sockaddr));

		if (sent < 0) {
			fprintf(stderr, "%s: sending packet: %s\n", g_ping.name, strerror(errno));
			g_ping.data.failed++; return (1);
		}

		if ((size_t)sent != g_ping.data.packet_len) {
			fprintf(stderr, "%s: ping: wrote %s %u chars, ret=%ld\n", g_ping.name, g_ping.options.hostname, g_ping.data.packet_len, sent);
			if (sent >= (ssize_t)sizeof(struct icmphdr)) g_ping.data.failed++;
			return (0);
		}

		g_ping.data.sent++;

		return (0);
	}

#pragma endregion
