/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:36:35 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 12:01:18 by vzurera-         ###   ########.fr       */
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

		if (!strcmp(resolved_hostname, hostname))	strlcpy(host, hostname, strlen(hostname));
		else										snprintf(host, NI_MAXHOST + 20, "%s (%s)", resolved_hostname, hostname);

		return (0);
	}

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
		double				rtt = 0.0;

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
			if (options->options & OPT_VERBOSE) fprintf(stderr, "%s: packet too short (%zd bytes) from %s\n", g_ping.name, received, inet_ntoa(g_ping.options.sockaddr.sin_addr));
			return;
		}

		if (icmp->type == ICMP_ECHOREPLY && ntohs(icmp->un.echo.id) == (getpid() & 0xFFFF)) {
			if (checksum(icmp, received - (ip->ihl << 2))) {
				g_ping.data.corrupted++;
				if (options->options & OPT_VERBOSE) fprintf(stderr, "%s: invalid checksum from %s\n", g_ping.name, inet_ntoa(from.sin_addr));
				return;
			}

			// Calculate RTT
			size_t icmp_data_size = received - (ip->ihl << 2) - sizeof(struct icmphdr);
			if (icmp_data_size >= sizeof(struct timeval)) {
				memcpy(&send_time, (char*)icmp + sizeof(struct icmphdr), sizeof(struct timeval));
				rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 + (recv_time.tv_usec - send_time.tv_usec) / 1000.0;
				update_stats(rtt);
			} else rtt = -1.0;
			
			char from_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
			if (!(options->options & OPT_QUIET)) {
				size_t data_len = (g_ping.options.size) ? g_ping.options.size : DEFAULT_SIZE;
				size_t data_size = data_len + 8;
				int ttl = ip->ttl;

				if (rtt >= 0) fprintf(stdout, "%zd bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", data_size, from_str, ntohs(icmp->un.echo.sequence), ttl, rtt);
				else         fprintf(stdout, "%zd bytes from %s: icmp_seq=%d ttl=%d\n", data_size, from_str, ntohs(icmp->un.echo.sequence), ttl);
			}
			g_ping.data.received++;
			
		} else if (icmp->type == ICMP_TIME_EXCEEDED) {
			struct iphdr *orig_ip = (struct iphdr *)((char *)icmp + sizeof(struct icmphdr));
			struct icmphdr *orig_icmp = (struct icmphdr *)((char *)orig_ip + (orig_ip->ihl << 2));

			if (checksum(icmp, received - (ip->ihl << 2))) {
				g_ping.data.corrupted++;
				if (options->options & OPT_VERBOSE) fprintf(stderr, "%s: invalid checksum from %s\n", g_ping.name, inet_ntoa(from.sin_addr));
				return;
			}

			// Calculate RTT
			size_t orig_icmp_data_size = ntohs(orig_ip->tot_len) - (orig_ip->ihl << 2) - sizeof(struct icmphdr);
			if (orig_icmp_data_size >= sizeof(struct timeval)) {
				memcpy(&send_time, (char*)orig_icmp + sizeof(struct icmphdr), sizeof(struct timeval));
				rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 + (recv_time.tv_usec - send_time.tv_usec) / 1000.0;
			} else rtt = -1.0;

			char from_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &from.sin_addr, from_str, INET_ADDRSTRLEN);
			size_t data_len = (g_ping.options.size) ? g_ping.options.size : DEFAULT_SIZE;
			size_t data_size = data_len + 8;

			char host[NI_MAXHOST + 32];
			strlcpy(host, from_str, sizeof(host));
			if (!(options->options & OPT_NUMERIC)) resolve_host(from_str, host);

			fprintf(stderr, "%zu bytes from %s: Time to live exceeded\n", data_size, host);

			if (options->options & OPT_VERBOSE) {
				// extended info
			}

			g_ping.data.lost++;
		}
	}

#pragma endregion
