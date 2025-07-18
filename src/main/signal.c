/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 18:13:11 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/18 20:33:27 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

	#include <signal.h>

#pragma endregion

#pragma region "Termination"

	static void termination_handler(int sig) {
		(void) sig;
		g_ping.running = false;
	}

#pragma endregion

#pragma region "Set Signals"

	int set_signals() {
		sighandler_t result = SIG_DFL;

		if (result != SIG_ERR) result = signal(SIGINT,  termination_handler);	// Enviada por Ctrl+C (interrupción del usuario). Detiene el programa y muestra estadísticas
		if (result != SIG_ERR) result = signal(SIGQUIT, termination_handler);	// Similar a SIGINT, detiene el programa y muestra estadísticas
		if (result != SIG_ERR) result = signal(SIGTERM, termination_handler);	// Enviada por kill o por otros procesos/sistema para solicitar terminación. Detiene el programa y muestra estadísticas
		if (result != SIG_ERR) result = signal(SIGHUP,  termination_handler);	// Enviada al cerrar la terminal. Detiene el programa y muestra estadísticas

		if (result == SIG_ERR) { fprintf(stderr, "ft_ping: Failed to set signal handler: %s\n", strerror(errno)); return (1); }

		return (0);
	}

#pragma endregion
