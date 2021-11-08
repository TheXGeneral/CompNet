#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>    
#include <stdlib.h>         
#define max 100
 
int main() {
  int c, cod;
  int32_t result_code;
  // Observatie: Deoarece dimensiunea tipului int difera de la platforma la platforma,
  // (spre exemplu, in Borland C in DOS e reprezentat pe 2 octeti, iar in C sub Linux pe
  // 4 octeti) este necesara utilizarea unor tipuri intregi standard. A se vedea
  // stdint.h.
  struct sockaddr_in server;
  char s[max];
  
  c = socket(PF_INET, SOCK_STREAM, 0);
  if (c < 0) {
    fprintf(stderr, "Eroare la creare socket client.\n");
    return 1;
  }
  
  memset(&server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons(4321);
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  cod = connect(c, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
  if (cod < 0) {
    fprintf(stderr, "Eroare la conectarea la server.\n");
    return 1;
  }
  
  printf("Dati o comanda pentru trimis la server: ");
  fgets(s, max, stdin);
  int32_t size = strlen(s)+1;
  size = htonl(size);
  // aici trimit dimensiunea şi apoi string-ul
  send(c, &size, sizeof(size),0);
  cod = send(c, s, strlen(s) + 1, 0);
  if (cod != strlen(s) + 1) {
    fprintf(stderr, "Eroare la trimiterea datelor la server.\n");
    return 1;
  }
  
  cod = recv(c, &result_code, sizeof(int32_t), MSG_WAITALL);
  result_code = ntohl(result_code);
  if (cod != sizeof(int)) {
    fprintf(stderr, "Eroare la primirea datelor de la client.\n");
    return 1;
  }
  
  printf("Serverul a returnat codul de iesire %d .\n", result_code);
  printf("-----------------------\n");
  char b;
  do {
    cod = recv(c, &b, 1, 0);
    printf("%c", b);
  }
  while (b != 0); 
  

  close(c);
}
 
 