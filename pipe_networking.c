#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    int from_client = 0;
    
    //0. server creates a WKP
    mkfifo(WKP, 0644);
    from_client = open(WKP, O_RDONLY);
    
    //1. server handles client sp
    char *sp = calloc(BUFFER_SIZE, sizeof(char));
    read(from_client, sp, BUFFER_SIZE);
    printf("Secret pipe id: %s\n", sp);
    
    //4. server gets client message, removes wkp
    remove(WKP);
    
    //5. server sends client response
    *to_client = open(sp, O_WRONLY);
    write(*to_client, ACK, sizeof(ACK));

    //7. server receives back final response from client
    char * response = calloc(BUFFER_SIZE, sizeof(char));
    read(from_client, response, BUFFER_SIZE);
    printf("Got resposne: %s\n", response);
    return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    int from_server = 0;
    //1. client creates sp
    char * sp = calloc(BUFFER_SIZE, sizeof(char));
    //1.5 Use pid to randomize sp
    int pid = getpid();
    int *p = &pid;
    sscanf(sp, "%d", p);
    mkfifo(sp, 0644);
    //2. client sends sp name to server
    *to_server = open(WKP, O_WRONLY);
    write(*to_server, sp, sizeof(sp));
    //3. client waits for response
    from_server = open(sp, O_WRONLY);
    char * response = calloc(BUFFER_SIZE, sizeof(char));
    read(from_server,response,BUFFER_SIZE);
    printf("Got response:%s\n", response);
    //5.client gets response and removes sp
    remove(sp);
    //6.client sends final response
    if(!strcmp(response, ACK)){
        write(*to_server, "Message from client", strlen("Message from client"));
    }

    return from_server;
}