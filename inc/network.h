/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:40:48 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 12:00:53 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"



#pragma endregion

#pragma region "Structures"

	typedef struct s_data {
		int				sockfd;
		size_t			sent;
		size_t			failed;
		size_t			received;
		size_t			lost;
		size_t			corrupted;
		uint8_t			packet[IP_HEADER + ICMP_HEADER + MAX_SIZE];
		uint32_t		packet_len;
		double			min_rtt;
		double			max_rtt;
		double			sum_rtt;
		double			sum_rtt_sq;
		size_t			rtt_count;
	}	t_data;

#pragma endregion

#pragma region "Methods"

	unsigned short	checksum(void *data, int len);
	int				socket_create();
	int				packet_create();
	int				packet_send();
	void			packet_receive();

#pragma endregion
