#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 5

#define LISTEN_PORT 1024
#define LISTEN_ADDR "127.0.0.1"
#define BUFFER_SIZE_OUT 128
#define BUFFER_SIZE_IN 128


int main() {
    struct sockaddr_in server_address, client_address;
    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(LISTEN_PORT);
    server_address.sin_addr.s_addr = inet_addr(LISTEN_ADDR);

    // Create the socket
    int socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketfd < 1) {
        printf("Error %d creating socket\n");
        return -1;
    }

    // Bind the socket to the port
    if (bind(socketfd, &server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "Error %d binding socket %d\n", errno, socketfd);
        return -1;
    }

    // Begin listening for connections
    if (listen(socketfd, BACKLOG) < 0) {
        fprintf(stderr, "Error %d on listening socket %d\n", errno, socketfd);
        return -1;
    }
    char* server_ip = inet_ntoa(server_address.sin_addr);
    printf("Server listening on %s\n", server_ip); 

    // Accept a connection
    int client_addrlen = sizeof(client_address);
    int read_socket = accept(socketfd, &client_address, &client_addrlen);
    if (read_socket < 0) {
        fprintf(stderr, "Error %d accepting the client\n", errno);
        return -1;
    }
    char* client_ip = inet_ntoa(client_address.sin_addr);
    printf("Connection accepted from %s (length: %d) for socket %d\n",
        client_ip, client_addrlen, read_socket);

    // Read
    char input[BUFFER_SIZE_IN];
    int num_bytes_read = read(read_socket, input, sizeof(input));
    if (num_bytes_read < 0) {
        fprintf(stderr, "Error %d on read\n", errno);
        return -1;
    }
    printf("Received %d bytes (\"%s\")\n", num_bytes_read, input);

    // Write
    char output[BUFFER_SIZE_OUT] = "Hello, client!";
    int num_bytes_sent = write(read_socket, output, sizeof(output));
    if (num_bytes_sent < 0) {
        fprintf(stderr, "Error %d on write", errno);
        return -1;
    }
    printf("Sent \"%s\"\n");

    return 0;
}