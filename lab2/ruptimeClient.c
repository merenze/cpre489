#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define CONNECT_PORT 1024
#define CONNECT_ADDR "127.0.0.1"
#define BUFFER_SIZE_OUT 1
#define BUFFER_SIZE_IN 128

int main() {
    struct sockaddr_in server_address;
    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(CONNECT_PORT);
    server_address.sin_addr.s_addr = inet_addr(CONNECT_ADDR);

    // Create the socket
    int socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketfd < 1) {
        printf("Error %d creating socket\n");
        return -1;
    }

    // Connect
    if (connect(socketfd, &server_address, sizeof(server_address)) < 0) {
        printf("Error %d on connection\n", errno);
        return -1;
    }

    // Read
    const char input[BUFFER_SIZE_IN];
    int num_bytes_read = read(socketfd, input, sizeof(input));
    if (num_bytes_read < 0) {
        fprintf(stderr, "Error %d on read\n", errno);
        return -1;
    }
    printf("Received %d bytes (\"%s\")\n", num_bytes_read, input);

    return 0;
}