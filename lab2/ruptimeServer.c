#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 5

#define LISTEN_PORT 1024
#define LISTEN_ADDR "127.0.0.1"
#define BUFFER_SIZE_OUT 128
#define BUFFER_SIZE_IN 128

char* parse_uptime(char*);
struct time_t* parse_date(char*);


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

    // Accept a connection
    int client_addrlen = sizeof(client_address);
    int read_socket = accept(socketfd, &client_address, &client_addrlen);
    if (read_socket < 0) {
        fprintf(stderr, "Error %d accepting the client\n", errno);
        return -1;
    }

    // Read
    char input[BUFFER_SIZE_IN];
    int num_bytes_read = read(read_socket, input, sizeof(input));
    if (num_bytes_read < 0) {
        fprintf(stderr, "Error %d on read\n", errno);
        return -1;
    }
    printf("Received %d bytes (\"%s\")\n", num_bytes_read, input);
    
    fprintf("%s\n", parse_uptime(NULL));

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

char* parse_uptime(char* buffer) {
    // Uptime does not show seconds, so we're doing some math here.
    // First, get the machine's start time.
    // Execute uptime process
    FILE* pipe = popen("uptime -s", "r");
    if (!popen) {
        fprintf(stderr, "Error %d executing `uptime`\n", errno);
        return NULL;
    }
    // Capture the process output
    char cmd_out[BUFFER_SIZE_OUT];
    fgets(cmd_out, BUFFER_SIZE_OUT, pipe);
    pclose(pipe);
    // Parse the process output into a time_t structure
    struct time_t* start_time = parse_date(cmd_out);
    printf("bar\n");
    char* now_pretty = asctime(parse_date);
    printf("Time: %d-%d-%d");
    // TODO
    return buffer;
}

struct time_t* parse_date(char* datestring) {
    struct tm *time = malloc(sizeof(struct tm));
    char* format = "%d-%d-%d %d:%d:%d";
    // TODO Fix segfault
    printf("Segfault about to occur on sscanf\n");
    int scan = sscanf(datestring, format,
        time->tm_year,
        time->tm_mon, 
        time->tm_mday, 
        time->tm_hour, 
        time->tm_min, 
        time->tm_sec
    );
    if (scan == EOF) {
        printf("Error %d parsing date\n", errno);
        return NULL;
    }
    printf("Scan successful\n");
    // tm represents year as years since 1900
    time->tm_year -= 1900;
    // tm represents mon as 0-11
    time->tm_mon -= 1;
    // Convert the structure to the final format
    printf("Converting tm to time_t... ");
    struct time_t* result = mktime(time);
    printf("Done\n");
    return result;
}