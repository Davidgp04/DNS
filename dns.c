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

struct DNSRecord {
  char domain[256];
  char ip_address[16];
};
void agregarRegistroDNS(struct DNSRecord dns_database[], int *num_records, const char *domain, const char *ip_address);
const char *buscarIPPorDominio(struct DNSRecord dns_database[], int num_records, const char *nombreDominio);


void agregarRegistroDNS(struct DNSRecord dns_database[], int *num_records, const char *domain, const char *ip_address) {
  if (*num_records >= 10) {
    printf("Error: La base de datos de registros DNS está llena\n");
    return;
  }

   for (int i = *num_records; i > 0; i--) {
        strcpy(dns_database[i].domain, dns_database[i - 1].domain);
        strcpy(dns_database[i].ip_address, dns_database[i - 1].ip_address);
    }


  strcpy(dns_database[0].domain, domain);
  strcpy(dns_database[0].ip_address, ip_address);
  (*num_records)++;
}

const char *buscarIPPorDominio(struct DNSRecord dns_database[], int num_records, const char *nombreDominio) {
  const char *msg = NULL;

  // Ciclo para buscar dentro de todo el arreglo un registro DNS
  for (int i = 0; i < num_records; i++) {
    // Comparamos si el nombre del dominio a buscar es igual al nombre del
    // dominio de la posición en la que nos encontramos en el arreglo para que
    // si es igual nos devuelva la IP.
    if (strcmp(nombreDominio, dns_database[i].domain) == 0) {
      msg = dns_database[i].ip_address;
      break;
    }
  }

  return msg;
}



int main(){
   struct DNSRecord dns_database[10];
  int num_records = 0;
  agregarRegistroDNS(dns_database, &num_records, "www.eafit.edu.co","140.82.113.3");

    //We create the local server
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    //We place the local address
    struct addrinfo *bind_address;
    getaddrinfo("18.212.62.23", "57", &hints, &bind_address);


     //We create the listener socket
    // We create the socket
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        printf("El socket no se creó\n");
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

  num_records++;
    word[bytes_received-1]='\0';
    printf("%s",word);
    const char* value=buscarIPPorDominio(dns_database, num_records, word);

  if (value){
    sendto(socket_listen, value, strlen(value), 0, (struct sockaddr*)&client_address,
    client_len);
    printf("Sent %s autoritativo\n",value);
    
  }else{

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
    char address2[bytes_sent];
    for (int j=0;j<bytes_sent-1;++j) address2[j]=address_buffer[j];
    agregarRegistroDNS(dns_database, &num_records, word,address2);
    freeaddrinfo(ip_address);
    }
  
  }
    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

    printf("Finished.\n");


    return 0;
}
