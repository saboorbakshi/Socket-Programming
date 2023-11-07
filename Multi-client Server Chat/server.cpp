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

#define MAX_BUFFER 1000
#define MAX_CLIENTS 5
#define SERVER_PORT 10000
#define SERVER_BACKLOG 5

using namespace std;

void check(int val, const char *msg) {
    if (val < 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    } 
}

int main(int argc, char **argv) {

    fd_set readfds;  
    struct sockaddr_in server_addr, client_addr;
    int max_sd, server_sd, client_sd, client_sds[MAX_CLIENTS]; 

    for (int i = 0; i < MAX_CLIENTS; i++) client_sds[i] = 0;

    check((server_sd = socket(AF_INET, SOCK_STREAM, 0)), "socket");

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    
    check((bind(server_sd, (struct sockaddr *) &server_addr, sizeof(server_addr))), "bind");

    check(listen(server_sd, SERVER_BACKLOG), "listen");

    socklen_t addr_size = sizeof(sockaddr_in);

    while (true) {
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add server fd to set  
        FD_SET(server_sd, &readfds);   
        max_sd = server_sd;

        //add child sockets to set  
        for (int i = 0 ; i < MAX_CLIENTS; i++) {   

            int sd = client_sds[i];   
                 
            //if valid socket descriptor then add to read list  
            if (sd > 0) FD_SET(sd, &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if (sd > max_sd) max_sd = sd;   
        }  

        //wait for activity on a socket, timeout is NULL, so wait indefinitely  
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) printf("select"); //use check?

        //If something happens on the server sd, then its an incoming connection  
        if (FD_ISSET(server_sd, &readfds)) {   

            check(client_sd = accept(server_sd, (struct sockaddr *) &client_addr, &addr_size), "accept");
            
            //add new socket to array of sockets 
            for (int i = 0; i < MAX_CLIENTS; i++)   {   
                if (client_sds[i] == 0 ) {   
                    client_sds[i] = client_sd;  
                    //inform user of socket number - used to send and receive commands  
                    printf("NEW CONNECTION - CLIENT[%d] SOCKET FD: %d, IP: %s, PORT: %d\n", i, client_sd , inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));   
                    break;   
                }   
            }    
        }

        //else it is an IO operation on some other socket 
        for (int i = 0; i < MAX_CLIENTS; i++) {   
            int sd = client_sds[i];   

            //Chat with client     
            if (FD_ISSET(sd, &readfds)) { 

                int n, valread;
                char buffer[MAX_BUFFER];
                
                bzero(buffer, MAX_BUFFER);
                check(valread = recv(sd, buffer, MAX_BUFFER, 0), "receive");

                if (valread == 0) {
                    printf("CLIENT[%d] TERMINATED CONNECTION\n", i); 
                    client_sds[i] = 0;
                    close(sd);
                } else {
                    printf("FROM CLIENT[%d]: %s   TO CLIENT[%d]: ", i, buffer, i);

                    bzero(buffer, MAX_BUFFER);
                    n = 0;  
                    while ((buffer[n++] = getchar()) != '\n');
                    check(send(sd, buffer, MAX_BUFFER, 0), "send");
                }
            }   
        }
    }
    return 0;
}
