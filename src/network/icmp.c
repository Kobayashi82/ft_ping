/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 19:16:51 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 16:32:40 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Checksum"

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

#pragma region "Pattern"

	static int fill_pattern(size_t data_len) {
		if (!data_len) return (0);

		if (g_ping.options.pattern_len) {
			for (size_t i = 0; i < data_len; ++i)
				g_ping.data.packet[g_ping.data.packet_len + i] = g_ping.options.pattern[i % g_ping.options.pattern_len];
		} else {
			uint8_t pattern_len = strlen(DEFAULT_PATTERN);
			for (size_t i = 0; i < data_len; ++i)
				g_ping.data.packet[g_ping.data.packet_len + i] = DEFAULT_PATTERN[i % pattern_len];
		}

		g_ping.data.packet_len += data_len;

		return (0);
	}

#pragma endregion

#pragma region "RAW"

	static int create_raw_packet(t_options *options, int *sequence) {
		struct icmphdr *icmp = (struct icmphdr *)g_ping.data.packet;
		icmp->type = ICMP_ECHO;
		icmp->code = 0;
		icmp->checksum = 0;
		icmp->un.echo.id = getpid() & 0xFFFF;
		icmp->un.echo.sequence = (*sequence)++;
		g_ping.data.packet_len += sizeof(*icmp);

		// No incluir timestamp en el paquete para reducir overhead
		size_t data_len = (options->size) ? options->size : ((options->pattern_len) ? options->pattern_len : DEFAULT_SIZE);
		if (data_len > MAX_SIZE) {
			fprintf(stderr, "ft_ping: data length too large: %zu bytes\n", data_len);
			return (1);
		}

		if (fill_pattern(data_len)) return (2);

		// Calcular checksum sin timestamp
		icmp->checksum = checksum(g_ping.data.packet, g_ping.data.packet_len);

		return (0);
	}

#pragma endregion

#pragma region "UDP"

	static int create_udp_packet(t_options *options) {
		// No incluir timestamp en el paquete para reducir overhead
		size_t data_len = (options->size) ? options->size : ((options->pattern_len) ? options->pattern_len : DEFAULT_SIZE);
		if (data_len > MAX_SIZE) {
			fprintf(stderr, "ft_ping: data length too large: %zu bytes\n", data_len);
			return (1);
		}

		if (fill_pattern(data_len)) return (2);

		return (0);
	}

#pragma endregion

#pragma region "Create"

	int packet_create() {
		static int	sequence = 0;
		int			result = 0;

		memset(g_ping.data.packet, 0, IP_HEADER + ICMP_HEADER + MAX_SIZE);
		g_ping.data.packet_len = 0;

		switch (g_ping.data.type) {
			case SOCK_RAW:		result = create_raw_packet(&g_ping.options, &sequence);				break;
			case SOCK_DGRAM:	result = create_udp_packet(&g_ping.options);						break;
			default:			result = 1;	fprintf(stderr, "ft_ping: unknown protocol icmp.\n");	break;
		}

		return (result);
	}

#pragma endregion
