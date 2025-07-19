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
		ssize_t sent = sendto(g_ping.data.sockfd, g_ping.data.packet, g_ping.data.packet_len, 0, (struct sockaddr *)&g_ping.options.sockaddr, sizeof(g_ping.options.sockaddr));

		if (sent < 0) {
			fprintf(stderr, "ft_ping: sendto: %s\n", strerror(errno));
			g_ping.data.failed++; return (1);
		}

		if ((size_t)sent != g_ping.data.packet_len) {
			fprintf(stderr, "ft_ping: partial send: sent %zd bytes, expected %u bytes\n", sent, g_ping.data.packet_len);
			g_ping.data.failed++; return (1);
		}

		// fprintf(stderr, "ft_ping: packet sent with %zd bytes of %u\n", sent, g_ping.data.packet_len);
		g_ping.data.sent++;

		return (0);
	}

#pragma endregion
