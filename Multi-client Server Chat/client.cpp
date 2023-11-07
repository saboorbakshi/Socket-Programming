#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <errno.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX 1000
#define SERVER_PORT 10000

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
        printf("TO SERVER: ");

        n = 0;
        while ((buffer[n++] = getchar()) != '\n');

        check(send(sd, buffer, MAX, 0), "send");

        bzero(buffer, MAX);

        check(valread = recv(sd, buffer, MAX, 0), "receive");
        printf("   FROM SERVER: %s", buffer);

        if (valread == 0) {
            printf("\nSERVER TERMINATED CONNECTION... \n");
            break;
        }
    }
}

int main(int argc, char **argv) 
{
    int client_sd;
    struct sockaddr_in server_addr;

    char buffer[1000];
    char server_reply[2000];
    ssize_t n;

    check((client_sd = socket(AF_INET, SOCK_STREAM, 0)), "socket");

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
   
    check(connect(client_sd, (struct sockaddr *) &server_addr, sizeof(server_addr)), "connect");
    
    chat(client_sd);

    close(client_sd);
    
    return 0;
}

