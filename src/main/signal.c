/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 18:13:11 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/17 20:42:57 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

	#include <signal.h>

#pragma endregion

#pragma region "Alarm"

	static void sigalarm_handler(int sig) {
		(void) sig;
		// Enviar un paquete ICMP Echo Request
		// Aumentar contador de paquetes enviados
		// Programar el próximo envío
		alarm(1); // Intervalo de 1 segundo
	}

#pragma endregion

#pragma region "Termination"

	static void termination_handler(int sig) {
		(void) sig;
		// Detener el bucle principal (running = false o algo asi)
		// Imprimir estadisticas
	}

#pragma endregion

#pragma region "Set Signals"

	void set_signals() {
		signal(SIGALRM, sigalarm_handler);			// Usada internamente para programar el envío de paquetes ICMP y manejar tiempos de espera (timeouts)
		signal(SIGINT, termination_handler);		// Enviada por Ctrl+C (interrupción del usuario). Detiene el programa y muestra estadísticas
		signal(SIGQUIT, termination_handler);		// Similar a SIGINT, detiene el programa y muestra estadísticas
		signal(SIGTERM, termination_handler);		// Enviada por kill o por otros procesos/sistema para solicitar terminación. Detiene el programa y muestra estadísticas
		signal(SIGHUP, termination_handler);		// Enviada al cerrar la terminal. Detiene el programa y muestra estadísticas
	}

#pragma endregion
