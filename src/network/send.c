/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:37:11 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 20:50:50 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Send"

	int socket_send(int sockfd, char *packet, size_t packet_len) {
		t_options		*options = &g_ping.options;

		// Enviar paquete
		ssize_t sent = sendto(sockfd, packet, packet_len, 0, (struct sockaddr *)&options->sockaddr, sizeof(options->sockaddr));
		if (sent < 0) {
			fprintf(stderr, "ft_ping: sendto: %s\n", strerror(errno));
			close(sockfd); return (-1);
		}
		if ((size_t)sent != packet_len) {
			fprintf(stderr, "ft_ping: partial send: sent %zd bytes, expected %zu bytes\n", sent, packet_len);
			close(sockfd); return (-1);
		}

		return (0);
	}

#pragma endregion
