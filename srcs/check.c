#include "ft_ping.h"

static void print_usage(void)
{
    printf("Usage: ft_ping [OPTION...] HOST ...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
    printf(" Options:\n");
    printf("  -v                verbose output\n");
    printf("  -?                give this help list\n");
    printf("\nMandatory or optional arguments to long options are also mandatory or optional\n");
    printf("for any corresponding short options.\n");
}

int parse_args(int argc, char **argv, struct ping *ping)
{
    int i = 1;
    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-v") == 0)
            {
                ping->verbose = 1;
            }
            else if (strcmp(argv[i], "-?") == 0)
            {
                print_usage();
                return 1;
            }
            else
            {
                fprintf(stderr, "ft_ping: invalid option -- '%s'\n", &argv[i][1]);
                fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
                return -1;
            }
        } 
        else 
        {
            if (ping->host)
            {
                fprintf(stderr, "ft_ping: extra arguments found\n");
                fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
                return -1;
            }
            ping->host = argv[i];
        }
        i++;
    }

    if (!ping->host)
    {
        fprintf(stderr, "ft_ping: destination address required\n");
        fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
        return -1;
    }
    return 0;
}

int check_args(int argc, char **argv, struct ping *ping)
{
    if (argc < 2)
    {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n");
        return EXIT_FAILURE;
    }

    return parse_args(argc, argv, ping);
}