/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:36:35 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/19 18:12:33 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Needs"

	static inline bool needs_raw(t_options *options) { return (options->type != ECHO || options->options & (OPT_FLOOD | OPT_IPTIMESTAMP | OPT_ROUTE) || options->preload); }

#pragma endregion

#pragma region "Receive"

	void packet_receive() {
		t_options			*options = &g_ping.options;
		char				buffer[1024];
		struct sockaddr_in	from;
		socklen_t			from_len = sizeof(from);
		struct icmphdr		*icmp;
		struct iphdr		*ip;
		struct timeval		send_time, recv_time;
		double				rtt;

		ssize_t received = recvfrom(g_ping.data.sockfd, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr *)&from, &from_len);
		if (received < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) fprintf(stderr, "ft_ping: recvfrom: %s\n", strerror(errno));
			return;
		}

		gettimeofday(&recv_time, NULL);

		ip = (struct iphdr *)buffer;
		icmp = (struct icmphdr *)(buffer + (ip->ihl << 2));
		if (received < (ssize_t)(sizeof(*ip) + sizeof(*icmp))) {
			if (options->options & OPT_VERBOSE) fprintf(stderr, "ft_ping: packet too short (%zd bytes) from %s\n", received, inet_ntoa(g_ping.options.sockaddr.sin_addr));
			return;
		}

		if (icmp->type == ICMP_ECHOREPLY && (needs_raw(options) ? icmp->un.echo.id == (getpid() & 0xFFFF) : true)) {
			bool found = false, duplicated = false;
			for (int i = 0; i < g_ping.data.index; ++i) {
				if (g_ping.data.packets[i].id == icmp->un.echo.sequence && g_ping.data.packets[i].sent) {
					if (!g_ping.data.packets[i].received) {
						send_time = g_ping.data.packets[i].time_sent;
						rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 + (recv_time.tv_usec - send_time.tv_usec) / 1000.0;
						g_ping.data.packets[i].time_received = recv_time;
						g_ping.data.packets[i].received = true;
						found = true;
						break;
					} else {
						g_ping.data.duplicated++;
						found = true; duplicated = true;
						break;
					}
				}
			}
			
			if (found) {
				char from_str[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
				if (!(options->options & OPT_QUIET)) {
					if (duplicated) fprintf(stdout, "%zd bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms (duplicated)\n", received - (ip->ihl << 2), from_str, icmp->un.echo.sequence, ip->ttl, rtt);
					else			fprintf(stdout, "%zd bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", received - (ip->ihl << 2), from_str, icmp->un.echo.sequence, ip->ttl, rtt);
				}
				if (!duplicated) g_ping.data.received++;
			}
		} else if (icmp->type == ICMP_TIME_EXCEEDED) { // TTL == 0
			bool found = false, duplicated = false;
			for (int i = 0; i < g_ping.data.index; ++i) {
				if (g_ping.data.packets[i].id == icmp->un.echo.sequence && g_ping.data.packets[i].sent) {
					if (!g_ping.data.packets[i].received) {
						send_time = g_ping.data.packets[i].time_sent;
						rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 + (recv_time.tv_usec - send_time.tv_usec) / 1000.0;
						g_ping.data.packets[i].time_received = recv_time;
						g_ping.data.packets[i].received = true;
						found = true;
						break;
					} else {
						g_ping.data.duplicated++;
						found = true; duplicated = true;
						break;
					}
				}
			}

			if (found) {
				char from_str[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
				if (options->options & OPT_VERBOSE) {
					if (duplicated)	fprintf(stderr, "From %s: Time to live exceeded (duplicated)\n", from_str);
					else			fprintf(stderr, "From %s: Time to live exceeded\n", from_str);
				}
				if (!duplicated) g_ping.data.received++;
			}
		}
	}

#pragma endregion
