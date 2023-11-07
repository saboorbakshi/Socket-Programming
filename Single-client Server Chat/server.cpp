#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <errno.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX 1000
#define SERVER_PORT 10000
#define SERVER_BACKLOG 5

using namespace std;

void check(int val, const char *msg) {
    if (val < 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    } 
}

void chat(int sd) {
    int n, valread;
    char buffer[MAX];
    
    while (1) {
        bzero(buffer, MAX);
        check(valread = recv(sd, buffer, MAX, 0), "receive");

        if (valread == 0) {
            printf("CLIENT TERMINATED CONNECTION... \n");
            break;
        }

        printf("FROM CLIENT: %s   TO CLIENT: ", buffer);

        bzero(buffer, MAX);

        n = 0;  
        while ((buffer[n++] = getchar()) != '\n');

        check(send(sd, buffer, MAX, 0), "receive");
    }
}

int main(int argc, char **argv) {

    int server_sd, client_sd;
    struct sockaddr_in server_addr, client_addr;

    check((server_sd = socket(AF_INET, SOCK_STREAM, 0)), "socket");

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    check((bind(server_sd, (struct sockaddr *) &server_addr, sizeof(server_addr))), "bind");

    check(listen(server_sd, SERVER_BACKLOG), "listen");

    socklen_t addr_size = sizeof(sockaddr_in);
    check(client_sd = accept(server_sd, (struct sockaddr *) &client_addr, &addr_size), "accept");

    chat(client_sd);

    close(client_sd);

    return 0;
}
