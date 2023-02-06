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
time_t parse_date(char*);
char* format_time(char*, double);


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

    // Parse the uptime
    char uptime[BUFFER_SIZE_OUT];
    parse_uptime(uptime);

    // Write uptime to the client
    printf("%s\n", uptime);
    int num_bytes_sent = write(read_socket, uptime, BUFFER_SIZE_OUT);
    if (num_bytes_sent < 0) {
        fprintf(stderr, "Error %d on write", errno);
        return -1;
    }

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
    time_t time_start = parse_date(cmd_out);
    time_t time_now;
    time(&time_now);
    double time_diff = difftime(time_now, time_start);
    format_time(buffer, time_diff);
    // TODO
    return buffer;
}

time_t parse_date(char* datestring) {
    struct tm *time = malloc(sizeof(struct tm));
    char* format = "%d-%d-%d %d:%d:%d";
    int scan = sscanf(datestring, format,
        &(time->tm_year),
        &(time->tm_mon), 
        &(time->tm_mday), 
        &(time->tm_hour), 
        &(time->tm_min), 
        &(time->tm_sec)
    );
    if (scan == EOF) {
        printf("Error %d parsing date\n", errno);
        return -1;
    }
    // tm represents year as years since 1900
    time->tm_year -= 1900;
    // tm represents mon as 0-11
    time->tm_mon -= 1;
    // Convert the structure to the final format
    time_t result = mktime(time);
    return result;
}

char* format_time(char* buffer, double time) {
    int seconds = (int) time;
    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    seconds %= 60;

    sprintf(buffer, "Uptime: %d hour(s) %d minute(s) %d second(s)", hours, minutes, seconds);
    return buffer;
}