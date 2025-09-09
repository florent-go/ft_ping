#include "ft_ping.h"

static int init_sock_addr(struct ping *ping_info)
{
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = IPPROTO_ICMP
	};
	struct addrinfo *tmp;

	if (getaddrinfo(ping_info->host, NULL, &hints, &tmp) != 0) {
		printf("ft_ping: unknown host\n");
		return -1;
	}

	struct sockaddr_in *addr = (struct sockaddr_in *)tmp->ai_addr;
    ping_info->remote_addr.sin_family = AF_INET;
    ping_info->remote_addr.sin_port = 0;
    ping_info->remote_addr.sin_addr = addr->sin_addr;

	freeaddrinfo(tmp);

	if (inet_ntop(AF_INET, &ping_info->remote_addr.sin_addr, ping_info->str_sin_addr,
	    INET_ADDRSTRLEN) == NULL) {
		printf("inet_ntop: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

static int create_socket(uint8_t ttl)
{
	int sock_fd;

	if ((sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
		printf("socket: %s\n", strerror(errno));
		return -1;
	}
	if (setsockopt(sock_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
		printf("setsockopt: %s\n", strerror(errno));
		close(sock_fd);
		return -1;
	}
	return sock_fd;
}


int init_sock(int *sock_fd, struct ping *ping_info, int ttl)
{
	if (init_sock_addr(ping_info) == -1)
		return -1;
	if ((*sock_fd = create_socket(ttl)) == -1)
		return -1;
	return 0;
}