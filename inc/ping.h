/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 22:27:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 00:42:48 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include "options.h"
	#include "network.h"
	#include "output.h"

	#include <pthread.h>
	#include <sys/select.h>

#pragma endregion

enum e_actions { MTX_INIT = 100, MTX_LOCK, MTX_UNLOCK, MTX_DESTROY, THRD_CREATE, THRD_JOIN, THRD_DETACH };

#pragma region "Structures"

	typedef struct s_ping {
		bool			running;
		bool			is_error;
		t_data			data;
		t_stats			stats;
		t_options		options;
		pthread_t		thread;
		pthread_mutex_t	*mutex;
	}	t_ping;

#pragma endregion

#pragma region "Variables"

	extern t_ping g_ping;

#pragma endregion

#pragma region "Methods"

	int	set_signals();
	int	mutex(int action);

#pragma endregion
