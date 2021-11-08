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
#include <string>  
#include <iostream>  
#include <algorithm>  
#define max 100
 
int main() {
  int c, cod;
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

  cod = send(c, s, strlen(s) + 1, 0);
  if (cod != strlen(s) + 1) {
    fprintf(stderr, "Eroare la trimiterea datelor la server.\n");
    return 1;
  }

  int32_t size_of_file;
  cod = recv(c, &size_of_file, sizeof(int32_t), MSG_WAITALL);
  size_of_file = ntohl(size_of_file);
  if (cod != sizeof(int)) {
    fprintf(stderr, "Eroare la primirea datelor de la client.\n");
    return 1;
  }

  printf("Dimensiunea fisierului e %d bytes.\n", size_of_file);

  // compute the file name - everything after the last occurence of /
  int last_occurence = 0;
  for(int i = 0; i < strlen(s); i++)
    if(s[i] == '/')
      last_occurence = i+1;
  std::string file_name;
  for( int i = last_occurence; i < strlen(s); i++){
    file_name += s[i];
  }
  file_name[file_name.size()-1] = '-';
  file_name += "copy";
  
  std::cout << file_name<< std::endl;
  // open the required file
  FILE * fp;
  fp = fopen (file_name.c_str(), "w+");
  char b;
  do {
    cod = recv(c, &b, 1, 0);
    fprintf(fp,"%c", b);
  }
  while (b != 0); 

  fclose(fp);
  close(c);
}
 
 