/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:40:48 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/20 23:27:22 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <netinet/tcp.h>

#pragma endregion

#pragma region "Defines"

	#define PACKETS_SIZE	1000

#pragma endregion

#pragma region "Structures"

	typedef struct s_packet {
		int				id;
		bool			sent;
		bool			received;
		struct timeval	time_sent;
		struct timeval	time_received;
	}	t_packet;

	typedef struct s_data {
		int				sockfd;
		uint8_t			type;
		char			*hostname;
		size_t			sent;
		size_t			failed;
		size_t			received;
		size_t			duplicated;
		size_t			lost;
		size_t			corrupted;
		size_t			count;
		size_t			interval;
		uint8_t			packet[IP_HEADER + ICMP_HEADER + MAX_SIZE];
		uint32_t		packet_len;
		uint16_t		index;
		t_packet		packets[PACKETS_SIZE];
	}	t_data;

#pragma endregion

#pragma region "Methods"

	unsigned short	checksum(void *data, int len);
	int				socket_create();
	int				packet_create();
	int				packet_send();
	void			packet_receive();

#pragma endregion
