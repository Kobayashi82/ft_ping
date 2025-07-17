/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 21:25:52 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/17 23:59:48 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

// Calcula el checksum ICMP (para SOCK_RAW)
static unsigned short checksum(void *data, int len) {
	unsigned long	sum = 0;
	unsigned short	*buf = data;

	while (len > 1) {
		sum += *buf++;
		len -= 2;
	}
	if (len == 1) sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);

	return (unsigned short)(~sum);
}

static bool needs_raw_socket(t_options *options) {
	return (options->type != ECHO ||
			options->socket_type ||
			options->options & (OPT_FLOOD | OPT_IPTIMESTAMP | OPT_ROUTE) ||
			options->preload);
}

static bool needs_root(t_options *options) {
	return (options->type == ADDRESS ||
			options->options & (OPT_FLOOD) ||
			options->preload);
}

int socket_create() {
	t_options		*options = &ping.options;
	int				sockfd;
	bool			use_raw = needs_raw_socket(options);
	char			packet[MAX_IP_LEN + MAX_ICMP_LEN + MAX_DATA_LEN];
	size_t			packet_len = 0;
	struct timeval	send_time;
	struct protoent	*proto;

	// Verificar opciones que requieren root
	if (needs_root(options) && !options->is_root) { dprintf(2, "ft_ping: Lacking privilege for requested operation\n"); return (-1); }

	// Initialize raw ICMP socket
	if (!(proto = getprotobyname ("icmp"))) { dprintf (2, "ft_ping: unknown protocol icmp.\n"); return (-1); }

	// Try SOCK_RAW first
	sockfd = socket(AF_INET, SOCK_RAW, proto->p_proto);
	if (sockfd < 0 && (errno == EPERM || errno == EACCES) && !use_raw) {
		// Try SOCK_DGRAM if no extra options needed
		errno = 0;
		sockfd = socket(AF_INET, SOCK_DGRAM, proto->p_proto);
		if (sockfd < 0) {
			if (errno == EPERM || errno == EACCES || errno == EPROTONOSUPPORT)	dprintf(2, "ft_ping: Lacking privilege for icmp socket\n");
			else																dprintf(2, "ft_ping: socketA: %s\n", strerror(errno));
			return (-1);
		}
	} else if (sockfd < 0) { dprintf(2, "ft_ping: socketB: %s\n", strerror(errno)); return (-1); }

	// Configurar opciones del socket
	if (options->ttl) {
		if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &options->ttl, sizeof(options->ttl)) < 0) {
			dprintf(2, "ft_ping: setsockopt(IP_TTL): %s\n", strerror(errno));
			close(sockfd); return (-1);
		}
	}
	if (options->tos) {
		if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &options->tos, sizeof(options->tos)) < 0) {
			dprintf(2, "ft_ping: setsockopt(IP_TOS): %s\n", strerror(errno));
			close(sockfd); return (-1);
		}
	}
	if (options->socket_type & SO_DEBUG) {
		int optval = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_DEBUG, &optval, sizeof(optval)) < 0) {
			dprintf(2, "ft_ping: setsockopt(SO_DEBUG): %s\n", strerror(errno));
			close(sockfd); return (-1);
		}
	}
	if (options->socket_type & SO_DONTROUTE) {
		int optval = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE, &optval, sizeof(optval)) < 0) {
			dprintf(2, "ft_ping: setsockopt(SO_DONTROUTE): %s\n", strerror(errno));
			close(sockfd); return (-1);
		}
	}
	if (options->options & OPT_IPTIMESTAMP) {
		char optval[20];
		int optlen = 0;
		if (options->ip_timestamp == OPT_TSONLY) {
			optval[0] = IPOPT_TIMESTAMP;
			optval[1] = 20;
			optval[2] = 5;
			optval[3] = 1;
			optlen = 20;
		} else if (options->ip_timestamp == OPT_TSADDR) {
			optval[0] = IPOPT_TIMESTAMP;
			optval[1] = 20;
			optval[2] = 5;
			optval[3] = 3;
			optlen = 20;
		} else {
			dprintf(2, "ft_ping: invalid IP timestamp flag: %s\n", (options->ip_timestamp == OPT_TSONLY) ? "tsonly" : "tsaddr");
			close(sockfd); return (-1);
		}
		if (setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, optval, optlen) < 0) {
			dprintf(2, "ft_ping: setsockopt(IP_OPTIONS): %s\n", strerror(errno));
			close(sockfd); return (-1);
		}
	}
	if (options->options & OPT_ROUTE) {
		char optval[40];
		optval[0] = IPOPT_RR;
		optval[1] = 39;
		optval[2] = 4;
		if (setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, optval, 40) < 0) {
			dprintf(2, "ft_ping: setsockopt(IP_OPTIONS): %s\n", strerror(errno));
			close(sockfd); return (-1);
		}
	}

	// Construir y enviar paquete
	if (use_raw) {
		// SOCK_RAW: Construir paquete ICMP
		struct icmphdr *icmp = (struct icmphdr *)packet;
		memset(packet, 0, sizeof(packet));
		icmp->type = ICMP_ECHO;
		icmp->code = 0;
		icmp->un.echo.id = getpid() & 0xFFFF;
		icmp->un.echo.sequence = 1;

		packet_len = sizeof(*icmp);
		if (options->size || options->pattern_len) {
			size_t data_len = options->size ? options->size : (options->pattern_len ? MAX_PATTERN : 0);
			if (data_len > MAX_DATA_LEN) {
				dprintf(2, "ft_ping: data length too large: %zu\n", data_len);
				close(sockfd);
				return (-1);
			}
			if (options->pattern_len) {
				for (size_t i = 0; i < data_len; i++)
					packet[sizeof(*icmp) + i] = options->pattern[i % options->pattern_len];
			} else {
				memset(packet + sizeof(*icmp), 0, data_len);
			}
			packet_len += data_len;
		}
		// Agregar timestamp para RTT
		gettimeofday(&send_time, NULL);
		memcpy(packet + packet_len, &send_time, sizeof(send_time));
		packet_len += sizeof(send_time);

		icmp->checksum = 0;
		icmp->checksum = checksum(packet, packet_len);
	} else {
		// SOCK_DGRAM: Solo datos (si los hay)
		packet_len = options->size ? options->size : (options->pattern_len ? MAX_PATTERN : 0);
		if (packet_len > MAX_DATA_LEN) {
			dprintf(2, "ft_ping: data length too large: %zu\n", packet_len);
			close(sockfd); return (-1);
		}
		if (packet_len) {
			if (options->pattern_len) {
				for (size_t i = 0; i < packet_len; i++)
					packet[i] = options->pattern[i % options->pattern_len];
			} else {
				memset(packet, 0, packet_len);
			}
		}
		// Agregar timestamp para RTT
		gettimeofday(&send_time, NULL);
		memcpy(packet + packet_len, &send_time, sizeof(send_time));
		packet_len += sizeof(send_time);
	}

	// Enviar paquete
	ssize_t sent = sendto(sockfd, packet, packet_len, 0, (struct sockaddr *)&options->sockaddr, sizeof(options->sockaddr));
	if (sent < 0) {
		dprintf(2, "ft_ping: sendto: %s\n", strerror(errno));
		close(sockfd); return (-1);
	}
	if ((size_t)sent != packet_len) {
		dprintf(2, "ft_ping: partial send: sent %zd bytes, expected %zu bytes\n", sent, packet_len);
		close(sockfd); return (-1);
	}

	return (sockfd);
}

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
		dprintf(2, "ft_ping: setsockopt(SO_RCVTIMEO): %s\n", strerror(errno));
		return (-1);
	}

	// Recibir respuesta
	ssize_t received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len);
	if (received < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) dprintf(2, "ft_ping: recvfrom: %s\n", strerror(errno));
		return (-1);
	}

	// Procesar paquete recibido
	ip = (struct iphdr *)buffer;
	icmp = (struct icmphdr *)(buffer + (ip->ihl << 2));
	if (received < (ssize_t)(sizeof(*ip) + sizeof(*icmp))) {
		if (options->options & OPT_VERBOSE) dprintf(2, "ft_ping: packet too short: %zd bytes\n", received);
		return (-1);
	}

	// Verificar si es ECHO_REPLY o TIME_EXCEEDED
	if (icmp->type == ICMP_ECHOREPLY && (needs_raw_socket(options) ? icmp->un.echo.id == (getpid() & 0xFFFF) : true)) {
		// Calcular RTT
		size_t data_offset = (ip->ihl << 2) + sizeof(*icmp);
		size_t data_len = options->size ? options->size : (options->pattern_len ? MAX_PATTERN : 0);
		data_offset += data_len;
		if (received < (ssize_t)(data_offset + sizeof(send_time))) {
			if (options->options & OPT_VERBOSE) dprintf(2, "ft_ping: packet too short for timestamp: %zd bytes\n", received);
			return (-1);
		}
		memcpy(&send_time, buffer + data_offset, sizeof(send_time));
		gettimeofday(&recv_time, NULL);
		rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 +
			(recv_time.tv_usec - send_time.tv_usec) / 1000.0;

		// Mostrar salida estándar
		char from_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
		if (!(options->options & OPT_QUIET)) dprintf(1, "%zd bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n", received - (ip->ihl << 2), from_str, icmp->un.echo.sequence, ip->ttl, rtt);
	} else if (icmp->type == ICMP_TIME_EXCEEDED && (options->options & OPT_VERBOSE)) {
		// Mostrar error TTL excedido con -v
		char from_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
		dprintf(2, "From %s: Time to live exceeded\n", from_str);
	}

	return (0);
}
