/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 21:25:52 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 10:54:22 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Needs"

	static inline bool needs_root(t_options *options)	{ return (options->type == ADDRESS || options->options & (OPT_FLOOD) || options->preload); }
	static inline bool needs_raw(t_options *options)	{ return (options->type != ECHO || options->options & (OPT_FLOOD | OPT_IPTIMESTAMP | OPT_ROUTE) || options->preload); }

#pragma endregion

#pragma region "Set"

	static int socket_set() {
		t_options	*options = &g_ping.options;
		int			sockfd = g_ping.data.sockfd;

		if (options->socket_type & OPT_DEBUG)	  { int optval = 1; setsockopt(sockfd, SOL_SOCKET, SO_DEBUG,	 &optval, sizeof(optval)); }
		if (options->socket_type & OPT_DONTROUTE) { int optval = 1; setsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE, &optval, sizeof(optval)); }

		if (options->ttl) {
			if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &options->ttl, sizeof(options->ttl)) < 0) {
				fprintf(stderr, "ft_ping: setsockopt(IP_TTL): %s\n", strerror(errno));
				close(sockfd); return (1);
			}
		}
		if (options->tos) {
			if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &options->tos, sizeof(options->tos)) < 0) {
				fprintf(stderr, "ft_ping: setsockopt(IP_TOS): %s\n", strerror(errno));
				close(sockfd); return (1);
			}
		}

		if (options->options & OPT_IPTIMESTAMP) {
			if (options->ip_timestamp != OPT_TSONLY && options->ip_timestamp != OPT_TSADDR) {
				fprintf(stderr, "ft_ping: invalid IP timestamp flag: %s\n", (options->ip_timestamp == OPT_TSONLY) ? "tsonly" : "tsaddr");
				close(sockfd); return (1);
			}

			char optval[20];
			optval[0] = IPOPT_TIMESTAMP;
			optval[1] = 20;
			optval[2] = 5;
			optval[3] = (options->ip_timestamp == OPT_TSONLY) ? 1 : 3;

			if (setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, optval, 20) < 0) {
				fprintf(stderr, "ft_ping: setsockopt(IP_OPTIONS): %s\n", strerror(errno));
				close(sockfd); return (1);
			}
		}

		if (options->options & OPT_ROUTE) {
			char optval[40];
			optval[0] = IPOPT_RR;
			optval[1] = 39;
			optval[2] = 4;

			if (setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, optval, 40) < 0) {
				fprintf(stderr, "ft_ping: setsockopt(IP_OPTIONS): %s\n", strerror(errno));
				close(sockfd); return (1);
			}
		}

		return (0);
	}

#pragma endregion

#pragma region "Create"

	int socket_create() {
		t_options		*options = &g_ping.options;
		struct protoent	*proto;

		g_ping.data.sockfd = -1;

		if (needs_root(options) && !options->is_root)	{ fprintf(stderr, "ft_ping: Lacking privilege for requested operation\n");	return (1); }
		if (!(proto = getprotobyname ("icmp")))			{ fprintf(stderr, "ft_ping: unknown protocol icmp.\n");						return (1); }

		g_ping.data.sockfd = socket(AF_INET, SOCK_RAW, proto->p_proto);
		if (g_ping.data.sockfd < 0 && (errno == EPERM || errno == EACCES) && !needs_raw(options)) { errno = 0;
			g_ping.data.sockfd = socket(AF_INET, SOCK_DGRAM, proto->p_proto);
			if (g_ping.data.sockfd < 0) {
				if (errno == EPERM || errno == EACCES || errno == EPROTONOSUPPORT)	fprintf(stderr, "ft_ping: Lacking privilege for icmp socket.\n");
				else																fprintf(stderr, "ft_ping: %s\n", strerror(errno));
				return (1);
			}
			g_ping.data.type = SOCK_DGRAM;
		} else if (g_ping.data.sockfd < 0) { fprintf(stderr, "ft_ping: %s\n", strerror(errno)); return (1); }
		else g_ping.data.type = SOCK_RAW;

		if (socket_set()) { close(g_ping.data.sockfd); return (1); }

		return (0);
	}

#pragma endregion
