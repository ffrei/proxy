#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent


#define BUFSIZE 1024

int main(int argc,char *argv[]) {

 char *req = argv[1];
 struct addrinfo hints, *res;
 struct in_addr addr;
 int err;
 char ip[20];
 int nsnd;
 char msg[BUFSIZE];
 char* hostname,*tmp;

 strcpy(msg,req);
 hostname=strstr(msg,"http://")+7;  // decalage de 7 pour ne pas avoir le http://
 tmp =strchr(hostname,'/');
 tmp[0]='\0';

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

  int nrcv;
  printf("adresse : %s\n", hostname);

  int clientSocket;
  struct sockaddr_in serv_addr;

  if ((clientSocket = socket(PF_INET, SOCK_STREAM, 0)) <0 ) {
      perror ("erreur socket");
      exit(1);
  }



  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons((ushort)80);
  serv_addr.sin_addr.s_addr = inet_addr(ip);

  freeaddrinfo(res);

  if (connect(clientSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0) {
      perror("cliecho : erreur connect");
      exit(1);
  }


/*  strcpy(msg,"");
  strcat(msg,"GET ");
  strcat(msg,req);
  strcat(msg," HTTP/1.0\r\nHost: ");
  strcat(msg,"telecomnancy.univ-lorraine.fr");
  strcat(msg," \r\nUser-Agent: Chrome\r\n\r\n");
  strcat(msg,"\0");
*/

  strcpy(msg,"GET http://www.01net.com/ HTTP/1.1\r\nHost: www.01net.com\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\nAccept: text/html,application/xhtml+xml,application/xml\r\nAccept-Language: en-US,en\r\nCookie: _ga=GA1.2.2147028927.1492519864; TestIfCookieP=ok; xtvrn=$449128$; xtan449128=-; xtant449128=1; visited=true; mediaplayer_menu=on; axd=1001513191701500122; pbw=%24b%3d12520%3b%24o%3d99999; pid=3455235705789719565; pdomid=13; sasd2=q=%24qc%3d1307971523%3b%24ql%3dmedium%3b%24qpc%3d75001%3b%24qpp%3d0%3b%24qt%3d184_1903_42652t%3b%24dma%3d0&c=1&l=&lo=&lt=636282181077364484; sasd=%24qc%3d1307971523%3b%24ql%3dmedium%3b%24qpc%3d75001%3b%24qpp%3d0%3b%24qt%3d184_1903_42652t%3b%24dma%3d0; cookie_cnil_01net=cnil%2001net%20ok; csfq=1; dyncdn=2; cnfq=1; csync=0:6185056642648481819|76:CAESEPAzwchpvOKhH1RBEnyN-hc|25:1c945884-93ed-4700-a6b6-b76ec02ed3d9\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\n\r\n");

  printf("Message : \n%s\n", msg);

  if ( (nsnd = write (clientSocket, msg, strlen(msg)) ) <0 ) {
    printf ("servmulti : writen error on socket");
    exit (1);
  }
  printf("bla ?\n" );
  memset( (char*) msg, 0, sizeof(msg) );
  while ((nrcv= read ( clientSocket, msg, sizeof(msg)-1) ) >0) {
    msg[nrcv]='\0';
    printf("%s\n",msg );
    memset( (char*) msg, 0, sizeof(msg) );
  }

  printf("=======End=======\n" );
  close(clientSocket);
  return(0);
}
