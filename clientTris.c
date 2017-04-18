#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //for exit(0);
#include<sys/socket.h>
#include<errno.h> //For errno - the error number
#include<netdb.h> //hostent
#include<arpa/inet.h>
#include <unistd.h>

int main(int argc,char *argv[]) {
 char *hostname = argv[1];
 struct addrinfo hints, *res;
 struct in_addr addr;
 int err;
 char ip[100];

 memset(&hints, 0, sizeof(hints));
 hints.ai_socktype = SOCK_STREAM;
 hints.ai_family = AF_UNSPEC;

 if ((err = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
   fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
   exit(EXIT_FAILURE);
 }

 addr.s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
 strcpy(ip, inet_ntoa(addr));
 printf("%s resolved to %s\n" , hostname , ip);

 freeaddrinfo(res);

 return 0;
}
