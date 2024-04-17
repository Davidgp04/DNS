#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef AI_ALL
#define AI_ALL 0x0100
#endif

int main(int argc, char *argv[]){
    if (argc<2){
        printf("Por favor ingresa un hostname de la forma\n example.com o www.example.com");
        exit(0);
    }
    printf("Hallando la dirección '%s'\n", argv[1]);
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE;
    struct addrinfo *peer_address;
    if (getaddrinfo(argv[1], 0, &hints,&peer_address)){
         fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

     printf("La dirección IP es:\n");
    struct addrinfo *address = peer_address;
    char address_buffer[100]; // Store the text address.
        getnameinfo(address->ai_addr, address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                0, 0,
                NI_NUMERICHOST);
        printf("\t%s\n", address_buffer);
     freeaddrinfo(peer_address);
    return 0;
}
