#include "ft_ping.h"

bool stop_ping_loop = 1;
bool can_send_packet = 1;

void handler(int signum)
{
	if (signum == SIGINT)
		stop_ping_loop = 0;
	else if (signum == SIGALRM)
		can_send_packet = 1;
}

static int build_icmp_request(uint8_t *buf, int packet_len)
{
	static int seq = 0;
	struct icmphdr *hdr = (struct icmphdr *)buf;
	struct timeval *timestamp =  (void *)((uint8_t *)buf + (sizeof(struct icmphdr)));

	if (gettimeofday(timestamp, NULL) == -1) {
		printf("gettimeofday err: %s\n", strerror(errno));
		return -1;
	}
	hdr->type = ICMP_ECHO;
    hdr->un.echo.id = htons((uint16_t)getpid());
    hdr->un.echo.sequence = htons((uint16_t)seq++);
    hdr->checksum = 0;
	hdr->checksum = calculate_checksum((unsigned short *)buf, packet_len);
	return 0;
}


static int icmp_send_ping(int sock_fd, const struct ping *ping, struct stats *stats)
{
	ssize_t nb_bytes;
	uint8_t buf[PACKET_SIZE] = {};

	if (build_icmp_request(buf, sizeof(buf)) == -1)
		return -1;

	nb_bytes = sendto(sock_fd, buf, sizeof(buf), 0,
			  (const struct sockaddr *)&ping->remote_addr,
			  sizeof(ping->remote_addr));
	if (nb_bytes == -1)
    {
        if (errno == EACCES)
		printf("ft_ping: socket access error. Are you trying to ping broadcast ?\n");
        else
            printf("sendto err: %s\n", strerror(errno));
        return -1;
    }
	stats->nb_send++;
	return 0;
}

int icmp_recv_ping(int sock_fd, struct stats *stats)
{
	uint8_t buf[RECV_PACK_SIZE] = {};
	ssize_t nb_bytes;
	struct icmphdr *icmph;
	struct iovec iov[1] = {
		[0] = { .iov_base = buf, .iov_len = sizeof(buf)}
	};
	struct msghdr msg = { .msg_iov = iov, .msg_iovlen = 1 };

	nb_bytes = recvmsg(sock_fd, &msg, MSG_DONTWAIT);
	if (errno != EAGAIN && errno != EWOULDBLOCK && nb_bytes == -1)
    {
		printf("recvmsg err: %s\n", strerror(errno));
		return -1;
	} 
    else if (nb_bytes == -1)
		return 0;
	icmph = (void *)((uint8_t *)buf + sizeof(struct iphdr));
	if (!packet_filter((uint8_t *)icmph))
		return 0;

	if (icmph->type == ICMP_ECHOREPLY)
		display_ping_stats(buf, nb_bytes, stats);
	else
		display_icmp_error(icmph, buf);
	return 1;
}

int start_ping_loop(int socket_fd, struct ping *ping, struct stats *stats)
{
    signal(SIGINT, &handler);
	signal(SIGALRM, &handler);

	print_start_info(ping);
 	while (stop_ping_loop) 
    {
		if (can_send_packet)
		{
			can_send_packet = 0;
			if (icmp_send_ping(socket_fd, ping, stats) == -1)
				return EXIT_FAILURE;
			alarm(1);
		}
		if (icmp_recv_ping(socket_fd, stats) == -1)
            return EXIT_FAILURE;
	}
    print_final_stats(ping, stats);
    return EXIT_SUCCESS;
}