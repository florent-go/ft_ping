#include "ft_ping.h"


int main(int argc, char **argv)
{
    struct stats    s_icmp = {
        .nb_send = 0,
        .nb_ok = 0,
        .rtt_min = SIZE_MAX,
        .rtt_max = 0.0,
        .rtt_sum = 0.0,
        .rtt_sum_sq = 0.0,
    };
	int             sock_fd;
    struct ping     ping_info = {};

    if (getuid() != 0) {
		printf("ft_ping: usage error: need to be run as root\n");
		return EXIT_FAILURE;
	}
    int ret = check_args(argc, argv, &ping_info);
    if(ret != 0)
        return ret == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
    if (init_sock(&sock_fd, &ping_info, IP_TTL_VALUE) == -1)
		return EXIT_FAILURE;

    start_ping_loop(sock_fd, &ping_info, &s_icmp);
    return EXIT_SUCCESS;
}