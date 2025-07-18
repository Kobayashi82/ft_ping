/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 19:16:51 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 20:52:49 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Needs"

	static inline bool needs_raw(t_options *options)	{ return (options->type != ECHO || options->options & (OPT_FLOOD | OPT_IPTIMESTAMP | OPT_ROUTE) || options->preload); }

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

	int package_create(int sockfd, char *packet, size_t *packet_len) {
		t_options		*options = &g_ping.options;
		struct timeval	send_time;

		// Construir y enviar paquete
		if (needs_raw(options)) {
			// SOCK_RAW: Construir paquete ICMP
			struct icmphdr *icmp = (struct icmphdr *)packet;
			memset(packet, 0, MAX_IP_LEN + MAX_ICMP_LEN + MAX_DATA_LEN);
			icmp->type = ICMP_ECHO;
			icmp->code = 0;
			icmp->un.echo.id = getpid() & 0xFFFF;
			icmp->un.echo.sequence = 1;

			*packet_len = sizeof(*icmp);
			if (options->size || options->pattern_len) {
				size_t data_len = options->size ? options->size : (options->pattern_len ? MAX_PATTERN : 0);
				if (data_len > MAX_DATA_LEN) {
					fprintf(stderr, "ft_ping: data length too large: %zu\n", data_len);
					close(sockfd);
					return (-1);
				}
				if (options->pattern_len) {
					for (size_t i = 0; i < data_len; i++)
						packet[sizeof(*icmp) + i] = options->pattern[i % options->pattern_len];
				} else {
					memset(packet + sizeof(*icmp), 0, data_len);
				}
				*packet_len += data_len;
			}
			// Agregar timestamp para RTT
			gettimeofday(&send_time, NULL);
			memcpy(packet + *packet_len, &send_time, sizeof(send_time));
			*packet_len += sizeof(send_time);

			icmp->checksum = 0;
			icmp->checksum = checksum(packet, *packet_len);
		} else {
			// SOCK_DGRAM: Solo datos (si los hay)
			*packet_len = options->size ? options->size : (options->pattern_len ? MAX_PATTERN : 0);
			if (*packet_len > MAX_DATA_LEN) {
				fprintf(stderr, "ft_ping: data length too large: %zu\n", *packet_len);
				close(sockfd); return (-1);
			}
			if (*packet_len) {
				if (options->pattern_len) {
					for (size_t i = 0; i < *packet_len; i++)
						packet[i] = options->pattern[i % options->pattern_len];
				} else {
					memset(packet, 0, *packet_len);
				}
			}
			// Agregar timestamp para RTT
			gettimeofday(&send_time, NULL);
			memcpy(packet + *packet_len, &send_time, sizeof(send_time));
			*packet_len += sizeof(send_time);
		}

		return (0);
	}

#pragma endregion
