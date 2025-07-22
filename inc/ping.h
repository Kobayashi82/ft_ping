/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 18:20:43 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "options.h"
	#include "network.h"

#pragma endregion

#pragma region "Structures"

	typedef struct s_ping {
		bool			running;
		bool			in_preload;
		char			*name;
		t_data			data;
		t_options		options;
	}	t_ping;

#pragma endregion

#pragma region "Variables"

	extern t_ping g_ping;

#pragma endregion

#pragma region "Methods"

	void	show_header();
	void	show_stats();
	void	update_stats(double rtt);
	void	show_ip_header(struct iphdr *ip, struct icmphdr *icmp, size_t size);
	void	show_icmp_info(void *icmp, size_t size);

#pragma endregion
