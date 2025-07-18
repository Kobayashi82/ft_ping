/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:37:11 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 00:21:52 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Send"

	int packet_send() {
		t_options		*options = &g_ping.options;

		// Enviar paquete
		ssize_t sent = sendto(g_ping.data.sockfd, g_ping.data.packet, g_ping.data.packet_len, 0, (struct sockaddr *)&options->sockaddr, sizeof(options->sockaddr));
		if (sent < 0) {
			fprintf(stderr, "ft_ping: sendto: %s\n", strerror(errno));
			close(g_ping.data.sockfd); return (1);
		}
		if ((size_t)sent != g_ping.data.packet_len) {
			fprintf(stderr, "ft_ping: partial send: sent %zd bytes, expected %u bytes\n", sent, g_ping.data.packet_len);
			close(g_ping.data.sockfd); return (1);
		}

		return (0);
	}

#pragma endregion
