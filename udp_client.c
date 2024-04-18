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
#include <string.h>
#include <stdlib.h>

int main() {
    struct addrinfo set1;
    memset(&set1, 0, sizeof(set1));
    set1.ai_socktype=SOCK_DGRAM;

    struct addrinfo *listener;
    if (getaddrinfo("127.0.0.1", "8080", &set1, &listener)){
        printf("Todo salió bien...\n");
    }    

    SOCKET sock;
    sock=socket(listener->ai_family, listener-> ai_socktype, listener->ai_protocol);
    if (!ISVALIDSOCKET(sock)){
        printf("Algo salió mal...\n");
        exit(0);
    }

    if (connect(sock, listener->ai_addr, listener->ai_addrlen)){
        printf("No se conectó");
    }

    printf("Envía un nombre de dominio\n");

    while(1) {
        fd_set read;
        FD_ZERO(&read);
        FD_SET(sock, &read);
        FD_SET(0, &read);

        struct timeval timeout;
        timeout.tv_sec=0;
        timeout.tv_usec=100000;
        
        if (select(sock+1, &read, 0, 0, &timeout) < 0) {
            fprintf(stderr, "select falló (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        if (FD_ISSET(sock, &read)) {
            char read1[4000];
            int bytes_received = recv(sock, read1, 4000, 0);
            if (bytes_received < 1) {
                printf("Conexión cerrada\n");
                break;
            }
            printf("Dirección IP: %s\n", read1);
        }

        if(FD_ISSET(0, &read)) {
            char read2[4096];
            if (!fgets(read2, 4096, stdin)) break;
            printf("Dominio: %s\n", read2);
            int bytes_sent = send(sock, read2, strlen(read2), 0);
        }
    }

    CLOSESOCKET(sock);
    printf("Fin del programa\n");
    return 0;
}
