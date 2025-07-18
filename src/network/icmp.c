/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 19:16:51 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 00:21:42 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Needs"

	static inline bool needs_raw(t_options *options) { return (options->type != ECHO || options->options & (OPT_FLOOD | OPT_IPTIMESTAMP | OPT_ROUTE) || options->preload); }

#pragma endregion

#pragma region "Checksum"

	// Calculate ICMP checksum (for SOCK_RAW only)
	static unsigned short checksum(void *data, int len) {
		unsigned long	sum = 0;
		unsigned short	*buf = data;

		while (len > 1) { sum += *buf++; len -= 2; }
		if (len == 1) sum += *(unsigned char *)buf;
		sum = (sum >> 16) + (sum & 0xFFFF);
		sum += (sum >> 16);

		return (unsigned short)(~sum);
	}

#pragma endregion

#pragma region "Create Package"

	int packet_create() {
		t_options		*options = &g_ping.options;
		struct timeval	send_time;

		// Construir y enviar paquete
		if (needs_raw(options)) {
			// SOCK_RAW: Construir paquete ICMP
			struct icmphdr *icmp = (struct icmphdr *)g_ping.data.packet;
			memset(g_ping.data.packet, 0, MAX_IP_LEN + MAX_ICMP_LEN + MAX_DATA_LEN);
			icmp->type = ICMP_ECHO;
			icmp->code = 0;
			icmp->un.echo.id = getpid() & 0xFFFF;
			icmp->un.echo.sequence = 1;

			g_ping.data.packet_len = sizeof(*icmp);
			if (options->size || options->pattern_len) {
				size_t data_len = options->size ? options->size : (options->pattern_len ? MAX_PATTERN : 0);
				if (data_len > MAX_DATA_LEN) {
					fprintf(stderr, "ft_ping: data length too large: %zu\n", data_len);
					close(g_ping.data.sockfd);
					return (1);
				}
				if (options->pattern_len) {
					for (size_t i = 0; i < data_len; i++)
						g_ping.data.packet[sizeof(*icmp) + i] = options->pattern[i % options->pattern_len];
				} else {
					memset(g_ping.data.packet + sizeof(*icmp), 0, data_len);
				}
				g_ping.data.packet_len += data_len;
			}
			// Agregar timestamp para RTT
			gettimeofday(&send_time, NULL);
			memcpy(g_ping.data.packet + g_ping.data.packet_len, &send_time, sizeof(send_time));
			g_ping.data.packet_len += sizeof(send_time);

			icmp->checksum = 0;
			icmp->checksum = checksum(g_ping.data.packet, g_ping.data.packet_len);
		} else {
			// SOCK_DGRAM: Solo datos (si los hay)
			g_ping.data.packet_len = options->size ? options->size : (options->pattern_len ? MAX_PATTERN : 0);
			if (g_ping.data.packet_len > MAX_DATA_LEN) {
				fprintf(stderr, "ft_ping: data length too large: %u\n", g_ping.data.packet_len);
				close(g_ping.data.sockfd); return (1);
			}
			if (g_ping.data.packet_len) {
				if (options->pattern_len) {
					for (size_t i = 0; i < g_ping.data.packet_len; i++)
						g_ping.data.packet[i] = options->pattern[i % options->pattern_len];
				} else {
					memset(g_ping.data.packet, 0, g_ping.data.packet_len);
				}
			}
			// Agregar timestamp para RTT
			gettimeofday(&send_time, NULL);
			memcpy(g_ping.data.packet + g_ping.data.packet_len, &send_time, sizeof(send_time));
			g_ping.data.packet_len += sizeof(send_time);
		}

		return (0);
	}

#pragma endregion
