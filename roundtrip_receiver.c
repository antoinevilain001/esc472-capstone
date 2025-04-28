// roundtrip_receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5000

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[1024];
    int small_buf_size = 256;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set small send/recv buffers
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &small_buf_size, sizeof(small_buf_size));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &small_buf_size, sizeof(small_buf_size));

    // Bind to any IP address, PORT
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Receiver ready on port %d\n", PORT);

    len = sizeof(cliaddr);
    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);

        // Immediate reply
        sendto(sockfd, "ACK", strlen("ACK"), 0, (struct sockaddr *)&cliaddr, len);
        printf("Sent ACK\n");
    }

    close(sockfd);
    return 0;
}
