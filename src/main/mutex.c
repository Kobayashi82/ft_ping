/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 00:34:21 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 00:42:16 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

int mutex(int action) {
	int result = 0;

	if (action == MTX_INIT && pthread_mutex_init(g_ping.mutex, NULL)) {
		fprintf(stderr, "ft_ping: mutex initialization failed\n");
		result = MTX_INIT;
	}
	if (action == MTX_LOCK && pthread_mutex_lock(g_ping.mutex)) {
		fprintf(stderr, "ft_ping: mutex lock failed\n");
		result = MTX_LOCK;
	}
	if (action == MTX_UNLOCK  && pthread_mutex_unlock(g_ping.mutex)) {
		fprintf(stderr, "ft_ping: mutex unlock failed\n");
		result = MTX_UNLOCK;
	}
	if (action == MTX_DESTROY && pthread_mutex_destroy(g_ping.mutex)) {
		fprintf(stderr, "ft_ping: mutex destroy failed\n");
		result = MTX_DESTROY;
	}

	if (result) {
		g_ping.running = false;
		g_ping.is_error = true;
		return (result);
	}

	return (0);
}
