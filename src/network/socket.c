/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 21:25:52 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 17:56:52 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Set"

	static void socket_set() {
		t_options	*options = &g_ping.options;
		int			sockfd = g_ping.data.sockfd;

		int optval = 6;	setsockopt(sockfd, SOL_SOCKET, SO_PRIORITY, &optval, sizeof(optval));
		optval = 1;		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

		if (options->socket_type & OPT_DEBUG)	  { int optval = 1; setsockopt(sockfd, SOL_SOCKET, SO_DEBUG,	 &optval, sizeof(optval)); }
		if (options->socket_type & OPT_DONTROUTE) { int optval = 1; setsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE, &optval, sizeof(optval)); }

		if (options->ttl) setsockopt(sockfd, IPPROTO_IP, IP_TTL, &options->ttl, sizeof(options->ttl));
		if (options->tos) setsockopt(sockfd, IPPROTO_IP, IP_TOS, &options->tos, sizeof(options->tos));
	}

#pragma endregion

#pragma region "Create"

	int socket_create() {
		struct protoent	*proto;
		g_ping.data.sockfd = -1;

		if (!(proto = getprotobyname ("icmp"))) { fprintf(stderr, "%s: unknown protocol icmp.\n", g_ping.name); return (1); }

		g_ping.data.sockfd = socket(AF_INET, SOCK_RAW, proto->p_proto);
		if (g_ping.data.sockfd < 0 && (errno == EPERM || errno == EACCES)) {
			if (errno == EPERM || errno == EACCES || errno == EPROTONOSUPPORT)	fprintf(stderr, "%s: Lacking privilege for icmp socket.\n", g_ping.name);
			else																fprintf(stderr, "%s: %s\n", g_ping.name, strerror(errno));
			return (1);
		}

		socket_set();

		return (0);
	}

#pragma endregion
