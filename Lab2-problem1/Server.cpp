#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>   
#include <string>
int c;


void tratare() {
  int cod;
  int32_t size;
  uint8_t b;
  struct sockaddr_in server;
 
  if (c < 0) {
    fprintf(stderr, "Eroare la stabilirea conexiunii cu clientul.\n");
    exit(1);
  }
  else
    printf("Un nou client s-a conectat cu succes.\n");
 
  recv(c, &size, sizeof(size), 0);
  size = ntohl(size);
  char* command = (char*) malloc(size*sizeof(char));
  // receive command
  // citire char by char
  int position = 0;
  do {
    cod = recv(c, &b, 1, 0);
    command[position++] = b;
  }
  while (b != 0); 

  // execute command
  // aici aş putea să folosesc forma clasică de C, fără string-uri. 
  // For the sake of it, am folosit string şi am trimis toate datele simultan. 
  // Teoretic nu mă opreşte nimica să trimit pe bucăţi bucăţile de output pe care el captez în buffer
  // e singurul motiv pentru care folosesc C++ pe fişier
   char buffer[128];
   std::string result = "";
   FILE* pipe = popen(command, "r");
   if (!pipe) exit(1);
   try {
      while (!feof(pipe)) {
         if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
      }
   } catch (...) {
      pclose(pipe);
      throw;
   }

   // send data
   int32_t result_code =  WEXITSTATUS(pclose(pipe)); // cu asta obţin exit code-ul de la funcţia executată mai sus
   send(c, &result_code, sizeof(int32_t), 0);
  send(c, result.c_str(), strlen(result.c_str()) + 1, 0);
  printf("Am trimis datele\n");
  close(c); 
  exit(0);
}
 
             
int main() {
  int s, cod;
  socklen_t l;
  struct sockaddr_in client, server;  
  
  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    fprintf(stderr, "Eroare la creare socket server.\n");
    return 1;
  }
  int enable = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR,(void*)&enable, sizeof(int));

  
  memset(&server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons(4321);
  server.sin_addr.s_addr = INADDR_ANY;
  
  cod = bind(s, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
  if (cod < 0) {
    fprintf(stderr, "Eroare la bind. Portul este deja folosit.\n");
    return 1;
  }
  
  listen(s, 5);
  
  while (1) { // deserveste oricati clienti
 
    memset(&client, 0, sizeof(client));
    l = sizeof(client);
 
    printf("Astept sa se conecteze un client.\n");
    c = accept(s, (struct sockaddr *) &client, &l);
    printf("S-a conectat clientul cu adresa %s si portul %d.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    // cu fork fac un proces nou. când fork == 0, sunt în copil
    if (fork() == 0) {
      tratare();
    }
    
  }
  
}