/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:36:35 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 23:23:21 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Resolve Host"

	static int resolve_host(const char *hostname, char *host) {
		struct addrinfo		hints, *res;
		struct sockaddr_in	sockaddr;
		char				resolved_hostname[NI_MAXHOST];

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_RAW;
		hints.ai_flags = AI_CANONNAME;

		if (getaddrinfo(hostname, NULL, &hints, &res)) return (1);
		memcpy(&sockaddr, res->ai_addr, res->ai_addrlen);
		if (getnameinfo((struct sockaddr*)&sockaddr, sizeof(sockaddr), resolved_hostname, sizeof(resolved_hostname), NULL, 0, 0) != 0) return (1);
		freeaddrinfo(res);

		if (!strcmp(resolved_hostname, hostname))	snprintf(host, sizeof(host), "%s", hostname);
		else										snprintf(host, NI_MAXHOST + 20, "%s (%s)", resolved_hostname, hostname);

		return (0);
	}

#pragma endregion

#pragma region "Receive"

	void packet_receive() {
		static char			buffer[IP_HEADER + ICMP_HEADER + MAX_SIZE];
		struct icmphdr		*icmp;
		struct iphdr		*ip;
		struct timeval		send_time, recv_time;

		struct sockaddr_in from; socklen_t from_len = sizeof(from);
		ssize_t received = recvfrom(g_ping.data.sockfd, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr *)&from, &from_len);
		if (received < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) fprintf(stderr, "%s: recvfrom: %s\n", g_ping.name, strerror(errno));
			return;
		}

		gettimeofday(&recv_time, NULL);

		ip = (struct iphdr *)buffer;
		icmp = (struct icmphdr *)(buffer + (ip->ihl << 2));

		size_t expected_size = sizeof(*ip) + sizeof(*icmp);
		if (received < (ssize_t)expected_size) {
			if (g_ping.options.options & OPT_VERBOSE) fprintf(stderr, "%s: packet too short (%zd bytes) from %s\n", g_ping.name, received, inet_ntoa(g_ping.options.sockaddr.sin_addr));
			return;
		}

		if (icmp->type == ICMP_ECHOREPLY && ntohs(icmp->un.echo.id) == (getpid() & 0xFFFF)) {
			if (checksum(icmp, received - (ip->ihl << 2))) {
				if (g_ping.options.options & OPT_VERBOSE) fprintf(stderr, "%s: checksum mismatch from %s\n", g_ping.name, inet_ntoa(from.sin_addr));
				g_ping.data.corrupted++; return;
			}

			// RTT
			double rtt = 0.0;
			size_t icmp_data_size = received - (ip->ihl << 2) - sizeof(struct icmphdr);
			if (icmp_data_size >= sizeof(struct timeval)) {
				memcpy(&send_time, (char*)icmp + sizeof(struct icmphdr), sizeof(struct timeval));
				rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 + (recv_time.tv_usec - send_time.tv_usec) / 1000.0;
				update_stats(rtt);
			} else rtt = -1.0;

			// Info
			char from_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
			if (!(g_ping.options.options & OPT_QUIET)) {
				size_t data_len = (g_ping.options.size != SIZE_MAX) ? g_ping.options.size : DEFAULT_SIZE;
				size_t data_size = data_len + 8;
				int ttl = ip->ttl;

				if (rtt >= 0)	printf("%zd bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", data_size, from_str, ntohs(icmp->un.echo.sequence), ttl, rtt);
				else         	printf("%zd bytes from %s: icmp_seq=%d ttl=%d\n", data_size, from_str, ntohs(icmp->un.echo.sequence), ttl);
			}
			g_ping.data.received++;
		} else if (icmp->type == ICMP_TIME_EXCEEDED) {
			if (checksum(icmp, received - (ip->ihl << 2))) {
				if (g_ping.options.options & OPT_VERBOSE) fprintf(stderr, "%s: checksum mismatch from %s\n", g_ping.name, inet_ntoa(from.sin_addr));
				g_ping.data.corrupted++; return;
			}

			char from_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
			if (!(g_ping.options.options & OPT_QUIET)) {
				char host[NI_MAXHOST + 32];
				snprintf(host, sizeof(host), "%s", from_str);
				if (!(g_ping.options.options & OPT_NUMERIC)) resolve_host(from_str, host);

				printf("%zu bytes from %s: Time to live exceeded\n", received - (ip->ihl << 2), host);

				if (g_ping.options.options & OPT_VERBOSE) {
					size_t icmp_payload_size = received - (ip->ihl << 2) - sizeof(struct icmphdr);
					show_ip_header(ip, icmp, icmp_payload_size);
					show_icmp_info(icmp, icmp_payload_size);
				}
			}
			g_ping.data.lost++;
		}
	}

#pragma endregion
