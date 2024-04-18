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
int main(){
    //We create the local server
      printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    //We place the local address
    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);


     //We create the listener socket
    // We create the socket
      printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    //We bind the socket to the local address
      printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);

  while(1){
    //We store our client
         struct sockaddr_storage client_address; //Store Client address
    socklen_t client_len = sizeof(client_address); 
    char read[1024]; // Buffer to store client data
    int bytes_received = recvfrom(socket_listen,
            read, 1024,
            0,
            (struct sockaddr*) &client_address, &client_len);


     struct addrinfo hints2;
    memset(&hints2, 0, sizeof(hints2));
    hints2.ai_family = AF_INET;
    hints2.ai_socktype = SOCK_DGRAM;
    hints2.ai_flags = AI_PASSIVE;
    struct addrinfo *ip_address;
    char word[bytes_received+1];
    for (int i=0;i<bytes_received-1;++i) word[i]=read[i];
    word[bytes_received-1]='\0';
    printf("%s",word);
    int status = getaddrinfo(word, NULL, &hints2, &ip_address);
if (status != 0) {
    fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(status));
    return 1;
}

    struct addrinfo *address = ip_address;
    char address_buffer[101]; // Store the text address.
        getnameinfo(address->ai_addr, address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                0, 0,
                NI_NUMERICHOST);
        printf("\t%s\n", address_buffer);
    address_buffer[100]='\n';
    int bytes_sent=sendto(socket_listen, address_buffer, sizeof(address_buffer), 0, (struct sockaddr*)&client_address,
    client_len);
    printf("Sent: %s, (%ld)\n",address_buffer,  sizeof(address_buffer));
    freeaddrinfo(ip_address);

  }
    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

    printf("Finished.\n");


    return 0;
}
