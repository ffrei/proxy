#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include "server.h"
#include "client.h"


/*********************************************************************
 *                                                                   *
 * FICHIER: SERVER_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application serveur*
 *              application client                                   *
 *                                                                   *
 * principaux appels (du point de vue serveur) pour un protocole     *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   *
 *     bind()                                                        *
 *                                                                   *
 *     listen()                                                      *
 *                                                                   *
 *     accept()                                                      *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *********************************************************************/


int serv(int numPort)

{
  int sockfd, newsockfd;
  struct sockaddr_in  serv_addr, cli_addr;
  socklen_t clilen;

/*
 * Ouvrir une socket (a TCP socket)
 */
if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) <0) {
   perror("servmulti : Probleme socket\n");
   exit (2);
 }


/*
 * Lier l'adresse  locale � la socket
 */
 memset( (char*) &serv_addr,0, sizeof(serv_addr) );
 serv_addr.sin_family = PF_INET;
 serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
 serv_addr.sin_port = htons(numPort);


 if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
   perror ("servmulti : erreur bind\n");
   exit (1);
 }

/* Param�trer le nombre de connexion "pending" */
 if (listen(sockfd, SOMAXCONN) <0) {
   perror ("servmulti : erreur listen\n");
   exit (1);
 }


 for (;;) {
   clilen = sizeof(cli_addr);
   //Une demande de connexion a �t� �mise par un client
   newsockfd = accept(sockfd,(struct sockaddr*) &cli_addr,&clilen);
   if (newsockfd < 0) {
    perror("servmulti : erreur accept\n");
    exit (1);
   }

   int pid = fork();

   if(pid ) {
     close(newsockfd);
   }else{
     proxy(newsockfd);
     exit(0);
   }
 }
}
