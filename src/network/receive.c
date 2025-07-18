/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:36:35 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 20:52:44 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Needs"

	static inline bool needs_raw(t_options *options)	{ return (options->type != ECHO || options->options & (OPT_FLOOD | OPT_IPTIMESTAMP | OPT_ROUTE) || options->preload); }

#pragma endregion

#pragma region "Receive"

	// Recibe y procesa respuestas ICMP
	int receive_echo_reply(t_options *options, int sockfd) {
		char buffer[1024];
		struct sockaddr_in from;
		socklen_t from_len = sizeof(from);
		struct icmphdr *icmp;
		struct iphdr *ip;
		struct timeval send_time, recv_time;
		double rtt;

		// Configurar timeout
		struct timeval timeout = { .tv_sec = options->linger ? options->linger : MAX_WAIT, .tv_usec = 0 };
		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
			fprintf(stderr, "ft_ping: setsockopt(SO_RCVTIMEO): %s\n", strerror(errno));
			return (-1);
		}

		// Recibir respuesta
		ssize_t received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len);
		if (received < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) fprintf(stderr, "ft_ping: recvfrom: %s\n", strerror(errno));
			return (-1);
		}

		// Procesar paquete recibido
		ip = (struct iphdr *)buffer;
		icmp = (struct icmphdr *)(buffer + (ip->ihl << 2));
		if (received < (ssize_t)(sizeof(*ip) + sizeof(*icmp))) {
			if (options->options & OPT_VERBOSE) fprintf(stderr, "ft_ping: packet too short (%zd bytes) from %s\n", received, inet_ntoa(g_ping.options.sockaddr.sin_addr));
			return (-1);
		}

		// Verificar si es ECHO_REPLY o TIME_EXCEEDED
		if (icmp->type == ICMP_ECHOREPLY && (needs_raw(options) ? icmp->un.echo.id == (getpid() & 0xFFFF) : true)) {
			// Calcular RTT
			size_t data_offset = (ip->ihl << 2) + sizeof(*icmp);
			size_t data_len = options->size ? options->size : (options->pattern_len ? MAX_PATTERN : 0);
			data_offset += data_len;
			if (received < (ssize_t)(data_offset + sizeof(send_time))) {
				if (options->options & OPT_VERBOSE) fprintf(stderr, "ft_ping: packet too short for timestamp: %zd bytes\n", received);
				return (-1);
			}
			memcpy(&send_time, buffer + data_offset, sizeof(send_time));
			gettimeofday(&recv_time, NULL);
			rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 +
				(recv_time.tv_usec - send_time.tv_usec) / 1000.0;

			// Mostrar salida estándar
			char from_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
			if (!(options->options & OPT_QUIET)) fprintf(stdout, "%zd bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n", received - (ip->ihl << 2), from_str, icmp->un.echo.sequence, ip->ttl, rtt);
		} else if (icmp->type == ICMP_TIME_EXCEEDED && (options->options & OPT_VERBOSE)) {
			// Mostrar error TTL excedido con -v
			char from_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
			fprintf(stderr, "From %s: Time to live exceeded\n", from_str);
		}

		return (0);
	}

#pragma endregion
