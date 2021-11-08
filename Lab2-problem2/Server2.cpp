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
#include <iostream>
#include <algorithm>
int c;

std::string execute_command(std::string command){
  char buffer[128];
   std::string result = "";
   FILE* pipe = popen(command.c_str(), "r");
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
   int exit_code =WEXITSTATUS(pclose(pipe));
   if(exit_code != 0)
    throw std::runtime_error("File does not exist");
   return result;
}

void tratare() {
  int cod;
  uint8_t b;
  struct sockaddr_in server;
 
  if (c < 0) {
    fprintf(stderr, "Eroare la stabilirea conexiunii cu clientul.\n");
    exit(1);
  }
  else
    printf("Un nou client s-a conectat cu succes.\n");
 
  std::string file_path="";
  // receive command
  do {
    cod = recv(c, &b, 1, 0);
    file_path+= b;
  
  }
  while (b != 0); 
  // I need to remove the annoying enter from my file_path
  std::replace( file_path.begin(), file_path.end(), '\n', ' ');


  std::string wc_result;
  int32_t file_size;
  std::string content;
  // I throw an error if the file was not found
  try{
    wc_result = execute_command("wc -c "+ file_path);
    // Rezultatul meu e de forma nr-bytes File-name. Profit de implementarea lui stoi, care cauta un numar de la pozitia 0 din string pana la primul caracter non-numeric  
    file_size = stoi(wc_result);
    content = execute_command("cat "+ file_path);
  }catch(std::runtime_error e){
      file_size = -1;
      content = "";
  }

   file_size = htonl(file_size);
  send(c, &file_size, sizeof(int32_t), 0);
  send(c, content.c_str(), strlen(content.c_str()) + 1, 0);
  
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
    
    if (fork() == 0) {
      tratare();
    }
    
  }
  
}