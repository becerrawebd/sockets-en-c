/*
    Ejemplo de servidor de flujos (TCP) con sockets POSIX.
    Para compilar y ejecutar:
    1) abrir una terminal en el directorio donde se encuentre el servidor
    2) ejecutar el comando "make" (va a compilar el servidor.c y generar el binario del servidor)
    3) ejecutar el servidor con "./servidor"
    3) ejecutar un cliente que envie solicitudes a nuestro servidor
       por ejemplo podemos usar el navegador web ingresando a "localhost:4000"
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
// El puerto que sera abierto, puede ser en el rango 1023<PORT<65535 (ya que los menores a 1023 estan reservados)
// pero ademas que sea un puerto no utilizado por ningun otro programa
//#define PORT 4000   
#define BACKLOG 2   // Numero maximo de clientes en cola de espera (mientras atiendo a otro cliente)

int main(int argc, char *argv[]){
    int fd_server; // file descriptor del socket que devolvera la llamada a socket()
    int fd_cliente;  // file descriptor del cliente que devolvera la llamada a accept()
    struct sockaddr_in server; // estructura que guarda info del servidor
    struct sockaddr_in cliente; // estructura que guarda info del cliente
    // estructura de "sockaddr_in"
    //
    // struct sockaddr_in {
    //      sa_family_t sin_family;  -> "sa_family_t" es un una variable "unsigned short int" (entero corto sin signo)
    //      in_port_t sin_port;     -> "in_port_t" es una variable "unsigned short int"
    //      struct in_addr sin_addr
    //      unsigned char sin_zero[8];  -> Para tener el mismo tamaño que la estructura "sockaddr" (hacer cuenta de bytes)
    // };
    //
    // estructura de "in_addr" utilizado en la estructura "sockaddr_in"
    //
    // struct in_addr {
    //      uin32_t s_addr;     -> "uin32_t" es una variable "unsigned int"
    // };
    
    if (argc !=2 ) {
        /* esto es porque el programa sólo necesitará un argumento, (el puerto) */
        printf("Uso: %s <PUERTO>\n",argv[0]);
        exit(-1);
    }

    // 1) crear socket con socket()
    // AF_INET para aceptar clientes de otras maquinas, AF_UNIX solamente acepta clientes de la misma maquina del server
    // SOCK_STREAM = socket orientado a flujos (TCP), SOCK_DGRAM = socket orientado a datagramas (UDP)
    // el tercer parametro es el protocolo, se pone en 0
    if((fd_server=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("error en socket()\n");
        exit(-1);
    }
    // 2) llenar la info del server
    // Tipo de conexion (por red o interna), en este caso la misma que usa el socket (RED=AF_INET)
    server.sin_family = AF_INET;
    // Hay dos tipos de ordenamientos de bytes
    // "bytes menos significativos", llamado "Ordenamiento de Bytes para Redes"
    // "bytes mas significativos", llamado "Ordenamiento de Bytes para Nodos"
    // "sin_port" y "sin_addr" de la estructura "sockaddr_in" deben ser del tipo "Ordenamiento de Bytes para Redes"
    // htons() convierte una variable short a "Ordenamiento de bytes para Redes"
    server.sin_port = htons(atoi(argv[1]));
    // sin_addr.s_addr es la direccion del cliente que queremos atender
    // la constante INADDR_ANY sirve para atender a cualquier cliente
    server.sin_addr.s_addr = INADDR_ANY;
    // Llenamos con ceros ya que solo la usamos para tener el mismo size que "sockaddr"
    bzero(&(server.sin_zero),8);

    // 3) asociamos el socket con un puerto con bind()
    // bind recibe 3 parametros
    //
    // (int fd) - File descriptor del socket que abrimos con socket()
    //
    // (struct sockaddr*) - Puntero a una estructura del tipo "sockaddr", esta es una estructura general valida para cualquier tipo de socket.
    // Nosotros utilizamos una estructura "sockaddr_in" (porque utilizamos AF_INET, en red no?)
    // Utilizariamos la estructura "sockaddr_un" si utilizaramos AF_UNIX (para la misma maquina, no en red)
    // Finalmente casteamos a "struct sockaddr*", que es el tipo de puntero que nos pide bind()
    //
    // (socklen_t __len) - Basicamente es un "unsigned int" del tamaño en Bytes de la estructura apuntada en el 2do parametro
    if(bind(fd_server,(struct sockaddr*)&server,sizeof(server))==-1){
        printf("Error en bind()\n");
        exit(-1);
    }
    // 4) empezamos atender las llamadas con listen()
    // (int d) - File descriptor del socket abierto, se lo preparara para aceptar conexiones
    // (int __n) - Cantidad maxima de solicitudes que seran encoladas a espera de respuesta
    
    if(listen(fd_server,BACKLOG)==-1){
        printf("Error en listen()\n");
        exit(-1);
    }
    struct ifaddrs *id;
    getifaddrs(&id);
    struct ifaddrs * tmp = id;
    printf("Escuchando en ");
    while (tmp){
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET){
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            printf("%s:%s | ", inet_ntoa(pAddr->sin_addr), argv[1]);
        }
        tmp = tmp->ifa_next;
    }
    printf("\n");
    // 5) aceptamos las conexiones entrantes con accept() y enviamos respuesta al cliente
    while(1){
        
        // con la llamada a accept() aceptamos las solicitudes que esten en la cola de espera, si no hay clientes, la llamada quedara bloqueada hasta que lo haya
        // (int fd) - File descriptor del socket que recibe las solicitudes
        // (struct sockaddr *__restrict__ __addr) - Puntero a una estructura del tipo "struct sockaddr" que contendra la info del cliente
        // ya hablamos del casteo, ocurre lo mismo que en la llamada a bind().
        // (socklen_t *__restrict__ __addr_len) - Puntero a un entero en el que se nos devolvera la longitud util del parametro anterior 
        socklen_t size_cliente;
        if((fd_cliente=accept(fd_server,(struct sockaddr*)&cliente,&size_cliente))==-1){
            printf("Error en accept()\n");
            exit(-1);
        }
        // Una vez aceptado un cliente, mostramos su IP almacenada en cliente.sin_addr
        // inet_ntoa() recibe la direccion de un host dado en "Ordenamiento de bytes para Redes"
        // (anteriormente habiamos aclarado que "sin_addr" de la estructura "sockaddr_in" tiene este mismo ordenamiento)
        // y la convierte en un string (IPv4 con notacion punteada-decimal)
        printf("Solicitud recibida desde %s\n",inet_ntoa(cliente.sin_addr));
        const char msg_bienvenida[] = "Bienvenido al servidor";
        // Enviamos una respuesta al cliente aceptado con send()
        // send recibe 4 parametros
        // (int __fd) - File descriptor del socket cliente
        // (const void *__buf) - Un puntero al contenido de la respuesta (de cualquier tipo, por eso es void*)
        // (size_t __n) - El tamaño del contenido de la respuesta
        // (int __flags) - Los flags que se quieran (ver manual del programador de linux)
        send(fd_cliente,(const void*)&msg_bienvenida,sizeof(msg_bienvenida),0);
        // cierro el file descriptor del socket cliente
        close(fd_cliente);
    }
}