#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent


#define BUFSIZE 1024
#define LOG 1 // 0 -> pas tout les logs ; 1 log activé

int proxy(int clientfd) {
  int nrcvServ,nrcvClient, nsndServ, nsndClient , msgSize;
  char msg[BUFSIZE];
  char msgCpy[BUFSIZE];
  char* hostname,*tmp,*msgAll;

  do{
    printf("############Waiting For Client  ##############\n" );
    memset( (char*) msg, 0, BUFSIZE );
    nrcvClient= read ( clientfd, msg, BUFSIZE-1) ;

    if(nrcvClient < 0){
      perror ("servmulti : readn error on socket");
      return (1);
    }else if(nrcvClient == 0){
      close(clientfd);
      return 0;
    }

    if(LOG){
      msg[nrcvClient]='\0';
      printf("==============REQUETE RECU=======================\n%s\n====================%d=================\n",msg,nrcvClient );
    }

    strcpy(msgCpy,msg);
    hostname=strstr(msgCpy,"http://")+7;  // decalage de 7 pour ne pas avoir le http://
    if(! hostname ){
      fprintf(stderr, "servmulti: ce n'est pas une requete http\n");
      return 1;
    }
    tmp =strchr(hostname,'/');
    tmp[0]='\0';

    printf("adresse : %s\n", hostname);

    int serverfd;
    struct sockaddr_in serv_addr;

    if ((serverfd = socket(PF_INET, SOCK_STREAM, 0)) <0 ) {
        perror ("servmulti");
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
      fprintf(stderr, "servmulti: %s\n", gai_strerror(err));
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

    if (connect(serverfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0) {
        perror("servmulti : erreur connect");
        exit(1);
    }

    //envoie de la requete au serveur web
    if ( (nsndServ = write (serverfd, msg, nrcvClient) ) <0 ) {
      printf ("servmulti : writen error on socket");
      exit (1);
    }

    msgAll=(char*)malloc(1 * sizeof(char));
    msgAll[0]='\0';
    msgSize=1;

    fd_set rfds;
    struct timeval tv;
    int retval;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(serverfd, &rfds);
    /* Wait up to 5 seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    do{
       retval = select(serverfd+1, &rfds, NULL, NULL, &tv);
       /* Don’t rely on the value of tv now! */
       if (retval == -1){
         perror("select()");
       }else if (!retval){
         printf("No data within 5 seconds.\n");
         nrcvServ=0;
       }else{
         /* FD_ISSET(0, &rfds) will be true. */

         printf("############ Waiting For Server  ##############\n" );
         memset( (char*) msg, 0, BUFSIZE   );
         nrcvServ= read ( serverfd, msg, BUFSIZE-1) ;
         if ( nrcvServ < 0 )  {
           perror ("servmulti : readn error on socket");
           return (1);
         }else{
           msgSize+=nrcvServ;
           printf("New size  : %d \n",msgSize );
           char* tmp_msgAll= (char*)realloc(msgAll,msgSize * sizeof(char));
           if(tmp_msgAll==NULL){
             printf("The re-allocation of array a has failed");
           }
           msgAll=tmp_msgAll;

           //printf("############   ##############\n" );
           strcat(msgAll,msg);

         }
       }
     }while(nrcvServ>0);

     if ( (nsndClient = write (clientfd, msgAll, msgSize) ) <0 ) {
       printf ("servmulti : writen error on socket");
     }

    if(LOG){
      msgAll[msgSize]='\0';
      printf("=====================================\n%s\n====================%d=================\n",msgAll,msgSize );
    }

    printf("=======End=======\n" );
    close(serverfd);

  }while (nrcvClient > 0 );
  close(clientfd);
  return 0;








































































































/*
  //recuperation header + debut du message
  printf("=========recuperation de la 1ere trame==========\n" );
  memset( (char*) msg, 0, BUFSIZE );
  nrcv= read ( serverfd, msg, BUFSIZE-1);
  if ( nrcv < 0 )  {
    perror ("servmulti : readn error on socket");
    exit (1);
  }else{
    //msg[nrcv]='\0';
  }

  //printf ("servmulti :message recu:\n%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);

 char *ret;

  ret = strstr(msg,"Content-Length");
  if(!ret){
    perror ("servmulti : no Content-Length");
    exit (1);
  }
  ret+=16;

  int msgSize = atoi(ret);


  int headerSize=0;
  char *body;
  body = strstr(msg, "\r\n\r\n");

  if(body){
      headerSize= body-msg;
  }

  printf("headerSize:%d\n",headerSize );
  msgSize += headerSize;

  if ( (nsnd = write (clientfd, msg, nrcv) ) <0 ) {
    printf ("servmulti : writen error on socket");
    exit (1);
  }

  int finished=0;
  if(strchr(msg,'\0')){
    finished=1;
  }


  //msg[nrcv]='\0';
  //printf ("servmulti :message recu:\n%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);
  int amountReadt=nrcv;
  while ( !finished ){
    printf("%d\n",nrcv );
    memset( (char*) msg, 0, BUFSIZE );
    printf("============attente d'un nieme trame ===========\n" );
    nrcv= read ( serverfd, msg, BUFSIZE-1);

    if(strchr(msg,'\0')){
      finished=1;
    }

    amountReadt+=nrcv;
    if ( nrcv < 0 )  {
      perror ("servmulti : readn error on socket");
      exit (1);
    }else{
      if ( (nsnd = write (clientfd, msg, nrcv) ) <0 ) {
        printf ("servmulti : writen error on socket");
      }

//      msg[nrcv]='\0';
//      printf ("servmulti :message recu:\n%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);

    }

  }
  printf("==============fin de la com=================\n" );
  close(serverfd);
}

if(nrcv < 0){
  perror ("servmulti : readn error on socket");
  exit (1);
}
*/
  //write (clientfd, '\0', 1);

}
