#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>


#define SERV_SOCKET_NAME "./server.soc"
#define RECV_BUF_LEN 64

static int sock;

void clean(int sock_fd) {
    close(sock_fd);
    unlink(SERV_SOCKET_NAME);
}


void sighandler(int signum) {
    clean(sock);
    exit(0);
}

int main() {
    struct sockaddr_un server_address = {};
    socklen_t client_length = sizeof (struct sockaddr_un);

    struct sockaddr_un client_address = {};
    socklen_t address_length = sizeof(struct sockaddr_un);

    ssize_t bytes_recv;

    char buffer[RECV_BUF_LEN];
	
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Failed to create socket");
        return 1;
    }

    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SERV_SOCKET_NAME, sizeof(SERV_SOCKET_NAME));
	
    if (bind(
        sock,
        (struct sockaddr *)&server_address,
        address_length) == -1)
    {
        perror("Failed to bind");
        return 1;
    }

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    printf("Server is listening\n");

    socklen_t cli_len = client_length;
    for (;;) {
        bytes_recv = recvfrom(
                sock,
                buffer,
                sizeof(buffer),
                0,
                (struct sockaddr *)&client_address,
                &cli_len
        );

        if (bytes_recv < 0) {
            perror("recvfrom() call failed");
            clean(sock);
            return 1;
        }

        buffer[bytes_recv] = '\0';

        printf("Client sent: %s; sun_family: %d, sun_path: %s\n", buffer, client_address.sun_family, client_address.sun_path);

        if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_address, client_length) < 0) {
            perror("Failed to send datagram message");
        }

        cli_len = client_length;
    }
}
