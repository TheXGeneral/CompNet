// exists on all platforms
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define closesocket close
typedef int SOCKET;
int check_if_word_is_ok(char current_string[], int current_string_size, char user_string[], int user_string_size){
  if(current_string_size == 0)
    return 1;
  if(user_string_size <= 3)
    return 0;
  if(current_string[current_string_size - 2] == user_string[1] && current_string[current_string_size - 3] == user_string[0])
    return 1;
  return 0;
}
void handle_client_word(int client, char* string, int32_t &string_size){
    char s[201] = "";
    int32_t max = 200;
    int32_t size;
    int32_t accepted;
    accepted = 0;
  while(accepted == 0){
        int c = recv(client, &size, sizeof(size), 0);
        size = ntohl(size);
        c = recv(client, s, size, 0);
        accepted = check_if_word_is_ok(string, string_size, s, size);
        if (accepted == 1){
          strcpy(string, s);
          string_size = size;
        }
        accepted = htonl(accepted);
        send(client, &accepted, sizeof(accepted), 0);
        accepted = ntohl(accepted); 
    }
}

void send_string(int client, char* string, int32_t string_size){
  string_size = htonl(string_size);
  int c = send(client, &string_size, sizeof(string_size), 0);
  string_size = ntohl(string_size);
  c = send(client, string, string_size, 0);
}

void tratare(int client, int client2){
  int32_t order1, order2;
  order1 = rand()%2 + 1;
  order2 = 3 - order1;
  order1 = htonl(order1);
  order2 = htonl(order2);
  send(client, &order1, sizeof(order1), 0);
  send(client2, &order2, sizeof(order2), 0);
  order1 = ntohl(order1);
  order2 = ntohl(order2);

  int32_t continue_the_game = 10;

  int32_t string_size = 0;
  char string[200] = "";
  if(order1 == 2)
      while(continue_the_game != 0){
    handle_client_word(client2, string, string_size);
    send_string(client, string, string_size);
    handle_client_word(client, string, string_size);
    send_string(client2, string, string_size);
  }
  else
    while(continue_the_game != 0){
        handle_client_word(client, string, string_size);
        send_string(client2, string, string_size);
        handle_client_word(client2, string, string_size);
        send_string(client, string, string_size);
    }
}

int main()
{
  SOCKET s;
  struct sockaddr_in server,client, client2;
  int c, c2, err;

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
  {
    printf("Eroare la crearea socketului server\n");
    return 1;
  }
  int enable = 1;

  setsockopt(s, SOL_SOCKET, SO_REUSEADDR,(void*)&enable, sizeof(int));

  memset(&server, 0, sizeof(server));
  server.sin_port = htons(1234);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    perror("Bind error:");
    return 1;
  }

  listen(s, 5);
  socklen_t l,l2;
  l = sizeof(client);
  l2 = sizeof(client2);
    while (1) { 
 
    memset(&client, 0, sizeof(client));
    l = sizeof(client);
    printf("Astept sa se conecteze un client.\n");
    c = accept(s, (struct sockaddr *) &client, &l);
    printf("S-a conectat clientul cu adresa %s si portul %d.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    memset(&client2, 0, sizeof(client2));
    l2 = sizeof(client2);

    printf("Astept sa se conecteze al doilea client.\n");
    c2 = accept(s, (struct sockaddr *) &client2, &l2);
    printf("S-a conectat clientul cu adresa %s si portul %d.\n", inet_ntoa(client2.sin_addr), ntohs(client2.sin_port));

    if (fork() == 0) { 
      tratare(c, c2);
      close(c);
      close(c2);
      return 0;
    }
  }
}
