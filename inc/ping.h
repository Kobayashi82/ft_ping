/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 20:50:00 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "options.h"
	#include "network.h"

#pragma endregion

#pragma region "Structures"

	typedef struct s_ping {
		bool		running;
		t_data		data;
		t_options	options;
	}	t_ping;

#pragma endregion

#pragma region "Variables"

	extern t_ping g_ping;

#pragma endregion

#pragma region "Methods"

	int	set_signals();

#pragma endregion
