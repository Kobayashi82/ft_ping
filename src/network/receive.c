/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:36:35 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/20 20:20:06 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

#pragma endregion

#pragma region "Needs"

	static inline bool needs_raw(t_options *options) { return (options->type != ECHO || options->options & (OPT_FLOOD | OPT_IPTIMESTAMP | OPT_ROUTE) || options->preload); }

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

		if (g_ping.data.type == SOCK_DGRAM)
			icmp = (struct icmphdr *)buffer;
		else {
			ip = (struct iphdr *)buffer;
			icmp = (struct icmphdr *)(buffer + (ip->ihl << 2));
		}

		size_t expected_size = (g_ping.data.type == SOCK_DGRAM) ? sizeof(*icmp) : sizeof(*ip) + sizeof(*icmp);
		if (received < (ssize_t)expected_size) {
			if (options->options & OPT_VERBOSE) fprintf(stderr, "%s: packet too short (%zd bytes) from %s\n", g_ping.name, received, inet_ntoa(g_ping.options.sockaddr.sin_addr));
			return;
		}

		if (icmp->type == ICMP_ECHOREPLY && (needs_raw(options) ? ntohs(icmp->un.echo.id) == (getpid() & 0xFFFF) : true)) {
			if (checksum(icmp, received - ((g_ping.data.type == SOCK_DGRAM) ? 0 : (ip->ihl << 2)))) {
				g_ping.data.corrupted++;
				if (options->options & OPT_VERBOSE) fprintf(stderr, "%s: invalid checksum from %s\n", g_ping.name, inet_ntoa(from.sin_addr));
				return;
			}

			bool found = false, duplicated = false;
			for (int i = 0; i < g_ping.data.index; ++i) {
				if (g_ping.data.packets[i].id == ntohs(icmp->un.echo.sequence) && g_ping.data.packets[i].sent) {
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
					size_t data_len = (g_ping.options.size) ? g_ping.options.size : DEFAULT_SIZE;
					size_t data_size = data_len + 8;
					int ttl = (g_ping.data.type == SOCK_DGRAM) ? 64 : ip->ttl;

					if (duplicated) fprintf(stdout, "%zd bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms (duplicated)\n", data_size, from_str, ntohs(icmp->un.echo.sequence), ttl, rtt);
					else			fprintf(stdout, "%zd bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", data_size, from_str, ntohs(icmp->un.echo.sequence), ttl, rtt);
				}
				if (!duplicated) g_ping.data.received++;
			}
		} else if (icmp->type == ICMP_TIME_EXCEEDED) {
			struct iphdr *orig_ip = (struct iphdr *)((char *)icmp + sizeof(struct icmphdr));
			struct icmphdr *orig_icmp = (struct icmphdr *)((char *)orig_ip + (orig_ip->ihl << 2));

			if (checksum(icmp, received - ((g_ping.data.type == SOCK_DGRAM) ? 0 : (ip->ihl << 2)))) {
				g_ping.data.corrupted++;
				if (options->options & OPT_VERBOSE) fprintf(stderr, "%s: invalid checksum from %s\n", g_ping.name, inet_ntoa(from.sin_addr));
				return;
			}

			bool found = false, duplicated = false;
			for (int i = 0; i < g_ping.data.index; ++i) {
				if (g_ping.data.packets[i].id == ntohs(orig_icmp->un.echo.sequence) && g_ping.data.packets[i].sent) {
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
				size_t data_len = (g_ping.options.size) ? g_ping.options.size : DEFAULT_SIZE;
				size_t data_size = data_len + 8;

				char host[NI_MAXHOST + 32];
				strlcpy(host, from_str, sizeof(host));
				if (!(options->options & OPT_NUMERIC)) resolve_host(from_str, host);

				if (duplicated)	fprintf(stderr, "%zu bytes from %s: Time to live exceeded (duplicated)\n", data_size, host);
				else			fprintf(stderr, "%zu bytes from %s: Time to live exceeded\n", data_size, host);

				if (options->options & OPT_VERBOSE) {
					// extended info
				}

				if (!duplicated) g_ping.data.lost++;
			}
		}
	}

#pragma endregion

// void print_ip_data (icmphdr_t * icmp, void *data _GL_UNUSED_PARAMETER) {
//   int hlen;
//   unsigned char *cp;
//   struct ip *ip = &icmp->icmp_ip;

//   print_ip_header (ip);

//   hlen = ip->ip_hl << 2;
//   cp = (unsigned char *) ip + hlen;

//   if (ip->ip_p == IPPROTO_TCP)
//     printf ("TCP: from port %u, to port %u (decimal)\n",
// 	    (*cp * 256 + *(cp + 1)), (*(cp + 2) * 256 + *(cp + 3)));
//   else if (ip->ip_p == IPPROTO_UDP)
//     printf ("UDP: from port %u, to port %u (decimal)\n",
// 	    (*cp * 256 + *(cp + 1)), (*(cp + 2) * 256 + *(cp + 3)));
//   else if (ip->ip_p == IPPROTO_ICMP)
//     {
//       int type = *cp;
//       int code = *(cp + 1);

//       printf ("ICMP: type %u, code %u, size %u", type, code,
// 	      ntohs (ip->ip_len) - hlen);
//       if (type == ICMP_ECHOREPLY || type == ICMP_ECHO)
// 	printf (", id 0x%04x, seq 0x%04x", *(cp + 4) * 256 + *(cp + 5),
// 		*(cp + 6) * 256 + *(cp + 7));
//       printf ("\n");
//     }
// }

// static void print_ip_header (struct ip *ip) {
//   size_t hlen;
//   unsigned char *cp;

//   hlen = ip->ip_hl << 2;
//   cp = (unsigned char *) ip + sizeof (*ip);	/* point to options */

//   if (options & OPT_VERBOSE)
//     {
//       size_t j;

//       printf ("IP Hdr Dump:\n ");
//       for (j = 0; j < sizeof (*ip); ++j)
// 	printf ("%02x%s", *((unsigned char *) ip + j),
// 		(j % 2) ? " " : "");	/* Group bytes two by two.  */
//       printf ("\n");
//     }

//   printf
//     ("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
//   printf (" %1x  %1x  %02x", ip->ip_v, ip->ip_hl, ip->ip_tos);
//   /*
//    * The member `ip_len' is not portably reported in any byte order.
//    * Use a simple heuristic to print a reasonable value.
//    */
//   printf (" %04x %04x",
// 	  (ip->ip_len > 0x2000) ? ntohs (ip->ip_len) : ip->ip_len,
// 	  ntohs (ip->ip_id));
//   printf ("   %1x %04x", (ntohs (ip->ip_off) & 0xe000) >> 13,
// 	  ntohs (ip->ip_off) & 0x1fff);
//   printf ("  %02x  %02x %04x", ip->ip_ttl, ip->ip_p, ntohs (ip->ip_sum));
//   printf (" %s ", inet_ntoa (*((struct in_addr *) &ip->ip_src)));
//   printf (" %s ", inet_ntoa (*((struct in_addr *) &ip->ip_dst)));
//   while (hlen-- > sizeof (*ip))
//     printf ("%02x", *cp++);

//   printf ("\n");
// }