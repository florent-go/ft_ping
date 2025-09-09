#ifndef FT_PING_H
#define FT_PING_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <sys/time.h>
#include <linux/errqueue.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>

#define IP_TTL_VALUE 64

#define ICMP_PAYLOAD_SIZE 56
#define PACKET_SIZE (sizeof(struct icmphdr) + ICMP_PAYLOAD_SIZE)
#define RECV_PACK_SIZE ((sizeof(struct iphdr) + sizeof(struct icmphdr)) * 2 + ICMP_PAYLOAD_SIZE + 1)

struct stats {
    int nb_send;
    int nb_ok;
    double rtt_min; 
    double rtt_max;
    double rtt_sum;
    double rtt_sum_sq; 
    struct timeval start_time;
};

struct ping
{
    int                 verbose;
	char                *host;
	struct sockaddr_in  remote_addr;
	char                str_sin_addr[INET_ADDRSTRLEN];
};

int                 check_args(int argc, char **argv, struct ping *ping);
int                 init_sock(int *sock_fd, struct ping *ping_info, int ttl);
int                 start_ping_loop(int socket_fd, struct ping *ping, struct stats *stats);

int 				packet_filter(uint8_t *buf);
unsigned short 		calculate_checksum(unsigned short *b, int len);
double 				ft_sqrt(double n);

void 				display_ping_stats(uint8_t *buf, ssize_t nb_bytes, struct stats *stats);
void 				print_start_info(const struct ping *ping);
void				display_icmp_error(struct icmphdr *icmph, uint8_t *buf);
void 				print_final_stats(const struct ping *ping, struct stats *stats);
#endif
