#include "ft_ping.h"
#include <math.h>

void print_start_info(const struct ping *ping)
{
	int pid;

	printf("PING %s (%s): %d data bytes", ping->host, ping->str_sin_addr,
	       ICMP_PAYLOAD_SIZE);
	if (ping->verbose) {
		pid = getpid();
		printf(", id 0x%04x = %d", pid, pid);
	}
	printf("\n");
}

void display_ping_stats(uint8_t *buf, ssize_t nb_bytes, struct stats *stats)
{
    struct iphdr *ip_hdr_recv = (struct iphdr *)buf;
    struct icmphdr *icmp_hdr_recv = (struct icmphdr *)(buf + (ip_hdr_recv->ihl * 4));
    struct timeval *time_sent = (struct timeval *)(buf + (ip_hdr_recv->ihl * 4) + sizeof(struct icmphdr));
    
    struct timeval time_recv;
    gettimeofday(&time_recv, NULL);
    
    long rtt_us = (time_recv.tv_sec - time_sent->tv_sec) * 1000000L + (time_recv.tv_usec - time_sent->tv_usec);
    double rtt_ms = rtt_us / 1000.0;
    
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_hdr_recv->saddr, ip_str, INET_ADDRSTRLEN);
    
    printf("%zd bytes from %s: icmp_seq=%u ttl=%u time=%.3f ms\n", 
           nb_bytes - (ip_hdr_recv->ihl * 4), 
           ip_str, 
           ntohs(icmp_hdr_recv->un.echo.sequence), 
           ip_hdr_recv->ttl, 
           rtt_ms);

    stats->nb_ok++;
    stats->rtt_sum += rtt_ms;
    stats->rtt_sum_sq += rtt_ms * rtt_ms;

    if (rtt_ms < stats->rtt_min) {
        stats->rtt_min = rtt_ms;
    }
    if (rtt_ms > stats->rtt_max) {
        stats->rtt_max = rtt_ms;
    }
}

void	display_icmp_error(struct icmphdr *icmph, uint8_t *buf)
{
	struct iphdr *ip_hdr_recv = (struct iphdr *)buf;

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_hdr_recv->saddr, ip_str, INET_ADDRSTRLEN);

    switch (icmph->type)
	{
        case ICMP_DEST_UNREACH:
            printf("From %s: Destination Host Unreachable\n", ip_str);
            break;
        case ICMP_TIME_EXCEEDED:
            printf("From %s: Time to live exceeded\n", ip_str);
            break;
        case ICMP_REDIRECT:
            printf("From %s: Redirect Host\n", ip_str);
            break;
        default:
            printf("From %s: Received unknown ICMP type %d, code %d\n",
                   ip_str, icmph->type, icmph->code);
            break;
    }
}

void print_final_stats(const struct ping *ping, struct stats *stats)
{
    struct timeval end_time;
    double total_time_ms;
    double packet_loss_rate = 0.0;
    
    gettimeofday(&end_time, NULL);
    total_time_ms = (double)(end_time.tv_sec - stats->start_time.tv_sec) * 1000.0;
    total_time_ms += (double)(end_time.tv_usec - stats->start_time.tv_usec) / 1000.0;

    printf("\n--- %s ping statistics ---\n", ping->host);
    
    if (stats->nb_send > 0)
        packet_loss_rate = 100.0 * (stats->nb_send - stats->nb_ok) / stats->nb_send;

    printf("%d packets transmitted, %d received, %.1f%% packet loss\n",
           stats->nb_send, stats->nb_ok, packet_loss_rate);
    
    if (stats->nb_ok > 0)
    {
        double rtt_avg = stats->rtt_sum / stats->nb_ok;
        
        double stddev = ft_sqrt((stats->rtt_sum_sq / stats->nb_ok) - (rtt_avg * rtt_avg));

        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
               stats->rtt_min, rtt_avg, stats->rtt_max, stddev);
    }
}