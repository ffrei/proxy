
#include "server.h"

usage(){
  printf("usage : servmulti numero_port_serveur\n");
}


int main (int argc,char *argv[]){

  /* Verifier le nombre de param�tre en entr�e */
   if (argc != 2){
     usage();
     exit(1);
   }

  serv(atoi(argv[1]));
}
