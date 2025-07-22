/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 20:54:09 by vzurera-          #+#    #+#             */
/*   Updated: 2025/07/22 23:16:51 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "ping.h"

	#include <math.h>

#pragma endregion

#pragma region "Header"

	void show_header() {
		size_t data_size = (g_ping.options.size != SIZE_MAX) ? g_ping.options.size : DEFAULT_SIZE;
		printf("PING %s (%s): %zu data bytes", g_ping.options.hostname, inet_ntoa(g_ping.options.sockaddr.sin_addr), data_size);
		if (g_ping.options.options & OPT_VERBOSE) printf (", id 0x%04x = %u", htons(getpid() & 0xFFFF), htons(getpid() & 0xFFFF));
		printf("\n");
	}

#pragma endregion

#pragma region "Stats"

	#pragma region "Show"

		void show_stats() {
			char addr_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &g_ping.options.sockaddr.sin_addr, addr_str, INET_ADDRSTRLEN);

			int total_sent = g_ping.data.sent;
			int total_received = g_ping.data.received;
			int total_lost = total_sent - total_received;
			int loss_percent = (total_sent > 0) ? (total_lost * 100) / total_sent : 0;

			printf("--- %s ping statistics ---\n", g_ping.options.hostname);
			printf("%d packets transmitted, %d packets received", total_sent, total_received);
			printf(", %d%% packet loss\n", loss_percent);

			if (g_ping.data.rtt_count > 0) {
				double avg_rtt = g_ping.data.sum_rtt / g_ping.data.rtt_count;
				double variance = 0.0;
				
				if (g_ping.data.rtt_count > 1) {
					double mean_sq = g_ping.data.sum_rtt_sq / g_ping.data.rtt_count;
					variance = mean_sq - (avg_rtt * avg_rtt);
					if (variance < 0) variance = 0.0;
				}
				
				double stddev = sqrt(variance);
				
				printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", 
					g_ping.data.min_rtt, avg_rtt, g_ping.data.max_rtt, stddev);
			}
		}

	#pragma endregion

	#pragma region "Update"

		void update_stats(double rtt) {
			if (rtt < 0) return;
			
			if (g_ping.data.min_rtt < 0 || rtt < g_ping.data.min_rtt) g_ping.data.min_rtt = rtt;
			if (g_ping.data.max_rtt < 0 || rtt > g_ping.data.max_rtt) g_ping.data.max_rtt = rtt;
			
			g_ping.data.sum_rtt += rtt;
			g_ping.data.sum_rtt_sq += rtt * rtt;
			g_ping.data.rtt_count++;
		}

	#pragma endregion

#pragma endregion

void show_icmp_info(void *icmp, size_t size) {   
	if (!icmp || size < sizeof(struct iphdr) + sizeof(struct icmphdr)) return;

	struct iphdr	*embedded_ip = (struct iphdr *)((char*)icmp + sizeof(struct icmphdr));
	struct icmphdr	*embedded_icmp = (struct icmphdr *)((char*)embedded_ip + (embedded_ip->ihl << 2));
	
	if (!(g_ping.options.options & OPT_VERBOSE) || embedded_ip->daddr != g_ping.options.sockaddr.sin_addr.s_addr) return;

	size_t total_icmp_size = ntohs(embedded_ip->tot_len) - (embedded_ip->ihl << 2);

	printf("ICMP: type %d, code %d, size %zu, id 0x%04x, seq 0x%04x\n", embedded_icmp->type, embedded_icmp->code, total_icmp_size, ntohs(embedded_icmp->un.echo.id), ntohs(embedded_icmp->un.echo.sequence));
}

void show_ip_header(struct iphdr *ip, struct icmphdr *icmp, size_t size) {
	struct iphdr *target_ip = ip;

	if (icmp) {
		if (icmp->type == ICMP_TIME_EXCEEDED && size >= sizeof(struct iphdr))
			target_ip = (struct iphdr *)((char*)icmp + sizeof(struct icmphdr));
	}
	
	printf("IP Hdr Dump:\n ");
	unsigned char *p = (unsigned char *)target_ip;
	for (size_t j = 0; j < sizeof(*target_ip); j++) printf("%02x%s", p[j], (j % 2) ? " " : "");

	char	src_str[INET_ADDRSTRLEN];
	char	dst_str[INET_ADDRSTRLEN];
	struct	in_addr src_addr, dst_addr;
	src_addr.s_addr = target_ip->saddr;
	dst_addr.s_addr = target_ip->daddr;
	inet_ntop(AF_INET, &src_addr, src_str, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &dst_addr, dst_str, INET_ADDRSTRLEN);
	
	printf("\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
	printf(" %1x  %1x  %02x %04x %04x   %1x %04x  %02x  %02x %04x %s %s\n", target_ip->version, target_ip->ihl, target_ip->tos, ntohs(target_ip->tot_len), ntohs(target_ip->id), (ntohs(target_ip->frag_off) & 0xe000) >> 13, ntohs(target_ip->frag_off) & 0x1fff, target_ip->ttl, target_ip->protocol, ntohs(target_ip->check), src_str, dst_str);
}

