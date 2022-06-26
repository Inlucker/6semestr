#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE 256
#define SOCKET_ADDR "localhost"
#define SOCKET_PORT 9999

#define OK 0
#define MAX_CLIENTS_COUNT 10

static int socket_fd;
static int clients[MAX_CLIENTS_COUNT];

int cleanup()
{
    close(socket_fd);
    exit(EXIT_FAILURE);
}

void sigint_handler(int signum)
{
    cleanup();
    exit(OK);
}

void handle_connection(void)
{
    const int sd = accept(socket_fd, NULL, NULL);
    if (sd == -1) {
        cleanup();
    }

    for (int i = 0; i < MAX_CLIENTS_COUNT; ++i)
    {
        if (!clients[i])
        {
            clients[i] = sd;
            fprintf(stdout, "New connection.\n");
            return;
        }
    }

    fprintf(stderr, "Reached MAX_CLIENTS_COUNT (%d)\n", MAX_CLIENTS_COUNT);
    cleanup();
}

void handle_client(int i)
{
    char buf[BUF_SIZE];

    if (recv(clients[i], &buf, BUF_SIZE, 0) < 0)
    {
        cleanup();
        return;
    }


    fprintf(stdout, "Server got: %s\n", buf);

    snprintf(buf, BUF_SIZE, "Server pid: %d", getpid());

    if (send(clients[i], buf, strlen(buf), 0) < 0)
    {
        cleanup();
        return;
    }

    fprintf(stdout, "Server sent: %s\n", buf);
    close(clients[i]);
    clients[i] = 0;
}

int main(void)
{
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(SOCKET_PORT)
    };

    if (bind(socket_fd, (struct sockaddr *) &addr, sizeof addr) < 0)
    {
        cleanup();
    }

    if (listen(socket_fd, MAX_CLIENTS_COUNT) < 0)
    {
        cleanup();
    }

    signal(SIGINT, sigint_handler);
    fprintf(stdout, "Server is listening\n");

    while (1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);

        int max_sd = socket_fd;

        for (int i = 0; i < MAX_CLIENTS_COUNT; ++i)
        {
            if (clients[i] > 0)
            {
                FD_SET(clients[i], &readfds);
            }

            if (clients[i] > max_sd)
            {
                max_sd = clients[i];
            }
        }

        if (pselect(max_sd + 1, &readfds, NULL, NULL, NULL, NULL) < 0)
        {
            cleanup();
            perror("Failed to select");
        }

        if (FD_ISSET(socket_fd, &readfds))
        {
            handle_connection();
        }

        for (int i = 0; i < MAX_CLIENTS_COUNT; ++i)
        {
            if (clients[i] && FD_ISSET(clients[i], &readfds))
            {
                handle_client(i);
            }
        }
    }
}
