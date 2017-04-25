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
#include "adBlock.h"
#include "utils.h"

#define BUFSIZE 64
#define LOG 1 // 0 -> pas tout les logs ; 1 log activé
#define TIMEOUT 15


char* myRead(int fd,int *size,int length){

  char msg[1];
  int end = 0,nrcv,allocated;
  allocated=BUFSIZE;
  char* msgAll=(char*)malloc(allocated);

  msgAll[0]='\0';
  *size=1;


  if(length==0){
    char* msgAll=(char*)malloc(1);
    *size=1;
    msgAll[0]='\0';
    return msgAll;
  }

  if(length>0){
    char* msgAll=(char*)malloc(length+1);
    memset( (char*) msgAll, 0, length+1   );
    *size= read ( fd, msgAll,length);

    msgAll[*size]='\0';

    while (*size<length) {
      memset( (char*) msg, 0, 1   );
      nrcv= read ( fd, msg,1)  ;

      *size+=nrcv;
      strcat(msgAll,msg);
    }
    msgAll[*size]='\0';
    return msgAll;
  }


  while (!end) {
    memset( (char*) msg, 0, 1   );
    nrcv= read ( fd, msg,1)  ;

    *size+=nrcv;
    if(*size==allocated-1){
      allocated+=BUFSIZE;

      char* tmp_msgAll= (char*)realloc(msgAll,allocated * sizeof(char));
      if(tmp_msgAll==NULL){
        printf("The re-allocation of array a has failed");
      }
      msgAll=tmp_msgAll;
    }

    strcat(msgAll,msg);

    char * ptr=strstr( msgAll,"\r\n\r\n");
    if(ptr){
      end=1;
      strcpy(ptr+strlen("\r\n\r\n"),"\0");
    }
  }
  /*
  printf("=================================================\n" );
  printAll(msgAll);
  printf("=================================================\n" );*/
  return msgAll;
}


int proxy(int clientfd) {
  int nrcvClient, nsndServ, nsndClient ;
  char* msg;
  char* msgCpy;
  char *hostname,*tmp,*msgAll ,*url;
  printf("##### pid:%d && fd:%d #####\n",getpid(),clientfd );
  do{
    if(LOG){
        //printf("#####%d####### Waiting For Client  ##############\n",getpid() );
    }

    msg=myRead(clientfd,&nrcvClient,-1);

    if(nrcvClient < 0){
      perror ("servmulti : readn error on socket");
      return (1);
    }else if(nrcvClient == 0){
      close(clientfd);
      return 0;
    }

    if(LOG){
      printf("==============REQUETE RECU=======================\n" );
      printAll(msg);
      printf("=================================================\n" );
    }

    msgCpy=(char*)malloc(nrcvClient);
    strcpy(msgCpy,msg);
    url=strstr(msgCpy,"http://");
    tmp =strstr(url,"HTTP/1");
    tmp[0]='\0';
    hostname=url+7;  // decalage de 7 pour ne pas avoir le http://
    if(! hostname ){
      fprintf(stderr, "servmulti: ce n'est pas une requete http\n");
      return 1;
    }
    tmp =strchr(hostname,'/');
    tmp[0]='\0';

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
      fprintf(stderr, "servmulti %s: %s\n",hostname, gai_strerror(err));
      exit(EXIT_FAILURE);
    }

    addr.s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;
    strcpy(ip, inet_ntoa(addr));
    if(LOG){
        printf("%d:%s resolved to %s\n" ,getpid(), hostname , ip);
    }


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


    int headerSize=0;
    int contentSize=0;
    char* header;
    char* content;

    header=myRead(serverfd,&headerSize,-1);
    if(LOG){
      printf("==============HEADER===================\n");
      printAll(header);
      printf("=======================================\n");
    }

    char * contentLength=strstr(header,"Content-Length");

    int length=-1;
    if(contentLength){
          int offset=strlen("Content-Length :");
          printf("Length : %d\n", atoi(contentLength+offset));
          length=atoi(contentLength+offset);
    }

    content=myRead(serverfd,&contentSize,length);
    if(LOG){
      printf("==============Content==================\n");
      printAll(content);
      printf("=======================================\n");
    }

    char* buffer=malloc(headerSize+strlen(content)+2*strlen("\r\n\r\n"));

    if( strstr(header,"Content-Type: text/html") ){

       char* content_tmp=cleanAd(content,contentSize)+9; // le +9 permet de retirer le "<-undef>"

       if(content_tmp){

         //init buffer
         int bufferSize=headerSize+strlen(content_tmp)+strlen("\r\n\r\n")+1;
         buffer=malloc(bufferSize);

         //copie du header dans le buffer
         strcpy(buffer,header);

         //conversion len(content_tmp) en str
         int buff2size=10;
         char* buff2=malloc(buff2size);
         itoa(strlen(content_tmp),buff2,buff2size);

         //recup d'un pointeur pour changer la taille
         char * contentLength=strstr(buffer,"Content-Length");
         if(contentLength){
           int offset=strlen("Content-Length :");
           strcpy(contentLength+offset,buff2);
           strcat(buffer,"\r\n\r\n");
         }
         //ajout du contenu au buffer
         strcat(buffer,content_tmp);
       }

     }else{
       if(buffer){
         strcpy(buffer,header);
         strcat(buffer,content);
       }
     }



     msgAll=buffer;

     if ( (nsndClient = write (clientfd, msgAll, strlen(msgAll)) ) <0 ) {
       printf ("servmulti : writen error on socket");
     }

    if(LOG){
      printf("===========HEADER et Content apres adblock====================\n");
      printAll(msgAll);
      printf("=======================================\n");
    }

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
