/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/17 21:33:37 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "options.h"

#pragma endregion

#pragma region "Structures"

	typedef struct s_stats {
		double tmin;								// minimum round trip time
		double tmax;								// maximum round trip time
		double tsum;								// sum of all times, for doing average
		double tsumsq;								// sum of all times squared, for std. dev.
	}	t_stats;

	typedef struct s_ping {
		t_options	options;
		
	}	t_ping;

#pragma endregion

#pragma region "Variables"

	extern t_ping ping;

#pragma endregion

#pragma region "Methods"

	void set_signals();
	void socket_create();
	void main_loop();

#pragma endregion
