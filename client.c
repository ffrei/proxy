#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>



int proxy(int sockfd) {
  return(0);
/*
    int clientSocket;
    struct sockaddr_in serv_addr;

    if ((clientSocket = socket(PF_INET, SOCK_STREAM, 0)) <0 ) {
        perror ("erreur socket");
        exit(1);
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((ushort)atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(clientSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0) {
        perror("cliecho : erreur connect");
        exit(1);
    }

    return(0);
*/
}
