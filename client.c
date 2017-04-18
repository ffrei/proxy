#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


#define BUFSIZE 1500

int proxy(int sockfd) {
  int nrcv, nsnd ,i;
  char msg[BUFSIZE];
  char adress[BUFSIZE];

  /*   Attendre  le message envoye par le client   */
  memset( (char*) msg, 0, sizeof(msg) );
  if ( (nrcv= read ( sockfd, msg, sizeof(msg)-1) ) < 0 )  {
    perror ("servmulti : readn error on socket");
    exit (1);
  }
  msg[nrcv]='\0';
  //printf ("servmulti :message recu:\n%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);

  for ( i = 0; i < nrcv; i++) {
    if( msg[i]=='G'  && i<nrcv-4 ){
      if(msg[i+1]=='E' && msg[i+2]=='T' && msg[i+3]==' '){

        int j =0 ;
        while (msg[i+4+j]!=' ' && j < nrcv ) {
            adress[j]=msg[i+4+j];
            j++;
        }
        msg[i+4+j]='\0';
      }
    }
  }

  printf("adresse : %s\n", adress);



  int clientSocket;
  struct sockaddr_in serv_addr;

  if ((clientSocket = socket(PF_INET, SOCK_STREAM, 0)) <0 ) {
      perror ("erreur socket");
      exit(1);
  }

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons((ushort)80);
  serv_addr.sin_addr.s_addr = inet_addr("172.217.6.35");

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


  memset( (char*) msg, 0, sizeof(msg) );
  do{
    nrcv= read ( clientSocket, msg, sizeof(msg)-1);
    if ( nrcv < 0 )  {
      perror ("servmulti : readn error on socket");
      exit (1);
    }else{
      msg[nrcv]='\0';
      //printf ("servmulti :message recu:\n%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);
      if ( (nsnd = write (sockfd, msg, nrcv) ) <0 ) {
        printf ("servmulti : writen error on socket");
        exit (1);
      }
      memset( (char*) msg, 0, sizeof(msg) );
    }

  }while ( nrcv == BUFSIZE-1);


  return(0);
}
