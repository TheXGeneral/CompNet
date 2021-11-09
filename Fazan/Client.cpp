#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
// simulate a fazan game

int max = 200;
void send_client_word(int c, char s[], int32_t &size){
  int32_t accepted = 0;
    while(accepted == 0){
      printf("Your word> \n");
      fgets(s, max, stdin);
      size = strlen(s);
      // replace the \n with \0
      s[size-1] = '\0';
      size = htonl(size);
      send(c, &size, sizeof(size), 0);
      size = ntohl(size);
      send(c, s, size, 0);

      recv(c, &accepted, sizeof(accepted), 0);
      accepted = ntohl(accepted); 
    }
    printf("The other player will have to give a word starting with %c%c\n", s[size-3],s[size-2]);
}
void get_string(int c, char s[], int32_t& size){
  printf("Wait for the other player\n");
  recv(c, &size, sizeof(size), 0);
  size = ntohl(size);
  recv(c, s, size, 0);
  printf("The opponent said: %s. You have to give a word starting with %c%c\n",s,s[size-3],s[size-2]);
}
int main()
{
  int c;
  struct sockaddr_in server;
  c = socket(AF_INET, SOCK_STREAM, 0);
  if (c < 0)
  {
    printf("Eroare la crearea socketului client\n");
    return 1;
  }
  memset(&server, 0, sizeof(server));
  server.sin_port = htons(1234);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  if (connect(c, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    printf("Eroare la conectarea la server\n");
    return 1;
  }
  int32_t order;
  // awaiting for the start game signal. Will send 1 or 2 depending on the player's turn
  printf("Wait for the other player to connect\n");
  recv(c, &order, sizeof(order), 0);
  order = ntohl(order);
  printf("The other player is connected. You are player %d\n",order);
  int32_t continue_the_game = 1;
  char s[201];
  int32_t max = 200;
  int32_t size;
  // if the other player is the first to say a word

  if(order == 2){
    while(continue_the_game != 0){
      get_string(c,s,size);
      send_client_word(c,s,size);
    }
  }else{
     while(continue_the_game != 0){
      send_client_word(c,s,size);
      get_string(c,s,size);
    }
  }
   
  //close(c);
}
