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

int main(void)
{
    const int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(SOCKET_PORT)
    };
    if (connect(socket_fd, (struct sockaddr *) &addr, sizeof addr) < 0) {
        perror("Failed to connect");
        return EXIT_FAILURE;
    }
    char buf[BUF_SIZE];
    snprintf(buf, BUF_SIZE, "Client pid: %d", getpid());

    if (sendto(socket_fd, buf, strlen(buf) + 1, 0, (struct sockaddr *) &addr, sizeof addr) < 0)
    {
        perror("Failed to sendto");
        return EXIT_FAILURE;
    }
    printf("Client sent: %s\n", buf);
    if (recv(socket_fd, buf, BUF_SIZE, 0) < 0)
    {
        perror("Failed to recv");
        return EXIT_FAILURE;
    }
    printf("Client got: %s\n", buf);
    close(socket_fd);
    return 0;
}
