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


#define BUFSIZE 1001

int proxy(int sockfd) {
  int nrcv, nsnd ,i;
  char msg[BUFSIZE];
  char msgCpy[BUFSIZE];
  char* hostname,*tmp;

  /*   Attendre  le message envoye par le client   */
  memset( (char*) msg, 0, sizeof(msg) );
  while ( (nrcv= read ( sockfd, msg, sizeof(msg)-1) ) > 0 )  {

  //msg[nrcv]='\0';
  //printf ("servmulti :message recu:\n%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);

  strcpy(msgCpy,msg);
  hostname=strstr(msgCpy,"http://")+7;  // decalage de 7 pour ne pas avoir le http://
  tmp =strchr(hostname,'/');
  tmp[0]='\0';

  printf("adresse : %s\n", hostname);



  int clientSocket;
  struct sockaddr_in serv_addr;

  if ((clientSocket = socket(PF_INET, SOCK_STREAM, 0)) <0 ) {
      perror ("erreur socket");
      exit(1);
  }

  struct addrinfo hints, *res;
  struct in_addr addr;
  int err;
  char ip[20];

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

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons((ushort)80);
  serv_addr.sin_addr.s_addr = inet_addr(ip);

  freeaddrinfo(res);

/*
  serv_addr.sin_port = htons((ushort)atoi(argv[2]));
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);*/

  if (connect(clientSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0) {
      perror("cliecho : erreur connect");
      exit(1);
  }

  if ( (nsnd = write (clientSocket, msg, nrcv) ) <0 ) {
    printf ("servmulti : writen error on socket");
    exit (1);
  }

  //recuperation header + debut du message
  nrcv= read ( clientSocket, msg, sizeof(msg)-1);
  if ( nrcv < 0 )  {
    perror ("servmulti : readn error on socket");
    exit (1);
  }else{
    //msg[nrcv]='\0';
  }

  //printf ("servmulti :message recu:\n%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);

/*  char *ret;

  ret = strstr(msg,"Content-Length");
  ret+=16;

  int msgSize = atoi(ret);
printf("======================================\n" );

  int headerSize=0;
  char *body;
  body = strstr(msg, "\r\n\r\n");
printf("======================================\n" );
  if(body){
      headerSize= body-msg;
  }

  printf("headerSize:%d\n",headerSize );
  msgSize += headerSize;
*/
  if ( (nsnd = write (sockfd, msg, nrcv) ) <0 ) {
    printf ("servmulti : writen error on socket");
    exit (1);
  }
  int amountReadt = nrcv;
  do{
    memset( (char*) msg, 0, sizeof(msg) );
    printf("======================================bla\n" );
    nrcv= read ( clientSocket, msg, sizeof(msg)-1);
    if ( nrcv < 0 )  {
      perror ("servmulti : readn error on socket");
      exit (1);
    }else{
      //printf ("servmulti :message recu:\n%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);
      if ( (nsnd = write (sockfd, msg, nrcv) ) <0 ) {
        printf ("servmulti : writen error on socket");
      }
    }
    printf("======================================\n" );
  }while ( nrcv>0 );
  //write (sockfd, '\0', 1);
  close(clientSocket);

}

if(nrcv < 0){
  perror ("servmulti : readn error on socket");
  exit (1);
}

  //write (sockfd, '\0', 1);
  return(0);
}
