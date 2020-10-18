#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#define PORT 4000
#define MAXDATASIZE 200

void main(int argc, const char *argv[]){
    int fd, bytes_recibidos;
    char data_recibida[MAXDATASIZE];
    /* Estructura "hostent" que almacenara info host (nodo) remoto
        struct hostent {
               char  *h_name;            official name of host
               char **h_aliases;          alias list 
               int    h_addrtype;         host address type 
               int    h_length;           length of address 
               char **h_addr_list;        list of addresses 
        } 
    */
    struct hostent *he;
    // Informacion sobre la direccion del servidor
    struct sockaddr_in servidor;
    if (argc !=2 ) {
        /* esto es porque el programa sólo necesitará un argumento, (la IP) */
        printf("Uso: %s <Dirección IP>\n",argv[0]);
        exit(-1);
    }
    if((he=gethostbyname(argv[1]))==NULL){
        printf("Error en gethostbyname()\n");
        exit(-1);
    }
    if((fd=socket(AF_INET,SOCK_STREAM,0))==-1){
        printf("Error en socket()\n");
        exit(-1);
    }
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PORT);
    servidor.sin_addr = *((struct in_addr*)he->h_addr);
    bzero(&(servidor.sin_zero),8);
    if(connect(fd,(struct sockaddr*)&servidor,sizeof(servidor))==-1){
        printf("Error en connect()\n");
        exit(-1);
    }
    if((bytes_recibidos=recv(fd,&data_recibida,MAXDATASIZE,0))==-1){
        printf("Error en recv()\n");
        exit(-1);
    }
    data_recibida[bytes_recibidos]='\0';
    printf("Mensaje del servidor: %s\n",data_recibida);
    close(fd);
}