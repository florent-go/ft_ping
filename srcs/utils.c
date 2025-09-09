#include "ft_ping.h"

#define EPSILON 0.000001

int packet_filter(uint8_t *buf)
{
	struct icmphdr *hdr_sent;
	struct icmphdr *hdr_rep = (struct icmphdr *)buf;

	if (hdr_rep->type == ICMP_ECHO)
		return 0;

	if (hdr_rep->type != ICMP_ECHOREPLY)
		buf += sizeof(struct icmphdr) + sizeof(struct iphdr);
	hdr_sent = (struct icmphdr *)buf;

	return hdr_sent->un.echo.id == htons((uint16_t)getpid());;
}


unsigned short calculate_checksum(unsigned short *b, int len)
{
    unsigned long sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *b++;
    if (len == 1)
        sum += *(unsigned char*)b;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    result = (unsigned short) ~sum;
    return result;
}

double ft_sqrt(double n)
{
    if (n < 0) 
        return 0;
    if (n == 0)
        return 0;

    double x = n;
    double y = 1.0;

    while (x - y > EPSILON)
    {
        x = (x + y) / 2.0;
        y = n / x;
    }

    return x;
}