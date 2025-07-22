/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 19:16:51 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 15:15:31 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Checksum"

	unsigned short checksum(void *data, int len) {
		unsigned long	sum = 0;
		unsigned short	*buf = data;

		while (len > 1) { sum += *buf++; len -= 2; }
		if (len == 1) sum += *(unsigned char *)buf;
		sum = (sum >> 16) + (sum & 0xFFFF);
		sum += (sum >> 16);

		return (unsigned short)(~sum);
	}

#pragma endregion

#pragma region "Pattern"

	static void fill_pattern(size_t data_len) {
		if (data_len) {
			if (g_ping.options.pattern_len) {
				for (size_t i = 0; i < data_len; ++i)
					g_ping.data.packet[g_ping.data.packet_len + i] = g_ping.options.pattern[i % g_ping.options.pattern_len];
			} else {
				for (size_t i = 0; i < data_len; ++i)
					g_ping.data.packet[g_ping.data.packet_len + i] = (uint8_t)(i % 256);
			}

			g_ping.data.packet_len += data_len;
		}
	}

#pragma endregion

#pragma region "Create"

	int packet_create() {
		memset(g_ping.data.packet, 0, IP_HEADER + ICMP_HEADER + MAX_SIZE);
		g_ping.data.packet_len = 0;

		struct icmphdr *icmp = (struct icmphdr *)g_ping.data.packet;
		icmp->type = ICMP_ECHO;
		icmp->code = 0;
		icmp->checksum = 0;
		icmp->un.echo.id = htons(getpid() & 0xFFFF);
		g_ping.data.packet_len += sizeof(*icmp);

		size_t data_len = (g_ping.options.size != SIZE_MAX) ? g_ping.options.size : DEFAULT_SIZE;
		if (data_len > MAX_SIZE) { fprintf(stderr, "%s: data length too large: %zu bytes\n", g_ping.name, data_len); return (1); }
		if (data_len >= sizeof(struct timeval)) {
			data_len -= sizeof(struct timeval);
			g_ping.data.packet_len += sizeof(struct timeval);
		}

		fill_pattern(data_len);

		return (0);
	}

#pragma endregion
