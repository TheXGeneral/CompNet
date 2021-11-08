// exists on all platforms
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
 
#include <unistd.h>
#include <errno.h>
 
typedef int SOCKET;

// clientul trimite 2 numere la server, serverul calculeaza suma si o trimtie inapoi clientului


int main() {
      // s-ul o sa fie folosit ca un socket - 
      // A socket is one endpoint of a two-way communication link between two programs running on the network - seamana cu fifo descriptor
      // socketul-ule o variabila de tip int - varianta asta cu typedef pare mai unnecessaryly fancier, dar are sens cand scriem codul comun pentru windows- unix. libraria de windows are tipul SOCKET
       SOCKET s;
       // definesc o structura de tiput internet socket address - socketul targeteaza o adresa de server (un internet socket adress)
       // avem nevoie atat de adresa serverului cat si de a clientului
       struct sockaddr_in server, client;
       // definim o variabila de tip socket length. vom avea nevoie sa trimitem adresa ei ulterior
       socklen_t l;
       // c - socket client
       int c, err;
       // initializez socket-ul
      // primul parametru e domeniul
      //  is used to designate the type of addresses that your socket can communicate with 
      // AF_INET: address format is host and port number

      // al doilea parametru e tipul conexiunii
      //TCP almost always uses SOCK_STREAM and UDP uses SOCK_DGRAM.
      //TCP (SOCK_STREAM) is a connection-based protocol. The connection is established and the two parties have a conversation until the connection is terminated by one of the parties or by a network error.
      //UDP (SOCK_DGRAM) is a datagram-based protocol. You send one datagram and get one reply and then the connection terminates.

      // ultimul parametru e protocolul de transfer. 0 - inseamna ca lasam socketul sa aleaga ce protocol vrea el
      // in teorie functia asta e buna as it is, nu ar trebui modificata
       s = socket(AF_INET, SOCK_STREAM, 0);
       // functia returneaza -1 in caz de eroare sau un file descriptor daca totul merge bine
      // verificam exact ca atare daca socketul s-a creat
       if (s < 0) {
              printf("Eroare la crearea socketului server\n");
              return 1;
       }

        // initializez toata variabila de server cu 0 - sa n-avem trash in variabila
       memset(&server, 0, sizeof(server));

       // un server adress are 3 proprietati fundamentale. Familie, ip si port
       // portul si ip-ul trebuie sa fie aceleasi cu cele ale serverului, ca altfel nu se poate realiza conectarea
       server.sin_port = htons(1234);// aici setam portul la 1234 - variabila e trimisa in retea, ea trebuie convertita la protocolul de stocare a datelor pentru network, explicatie mai jos
       server.sin_family = AF_INET;// aici seatam familia la AF_INET 
       // grija sa nu existe discrepante intre domeniul definirii socketului si familia adresei - pot aparea erori 

       //INADDR_ANY is used when you don't need to bind a socket to a specific IP. 
       // When you use this value as the address when calling bind(), the socket accepts connections to all the IPs of the machine.
       server.sin_addr.s_addr = INADDR_ANY;
       
        // fata de client, in server nu ne conectam la server ci facem bind, principiul e similar tho
        // parametrii - socket, adresa variabilei server, dimenisunea variabilelei server 
      // functia retunreaza 0 la succes şi -1 la eroare, verificam ca atare 
       if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
              perror("Bind error:");
              return 1;
       }
      // incepem sa ascultam pentru conexiuni, 
      // parametrii - server socket (s), 
      // numarul de conexiuni ascultate de server (dupa ce avem N clienti care asteapta sa vorbeasca cu serverul, urmatoarele conexiuni vor fi refuzate pana cand nu se elibereaza)

       listen(s, 5);
      l = sizeof(client); 
        // initializez toata variabila de server cu 0 - sa n-avem trash in variabila
       memset(&client, 0, sizeof(client));

      // asteptam conexiuni in continuu
      // Ometita would not be proud of this specific line, but who cares :) 
       while (1) {
              // definim variabilele ca short-uri ( p1 16 biti)
              // care-i motivul pentru care variabilele astea is definite in while si nu ca globale, probabil nici Dadi nu se stie spune
              uint16_t a, b, suma;
              // doar pentru debug, sa vedem ca serverul a pornit si asteapta conexiuni
              printf("Listening for incomming connections\n");
              // in momentul in care un clientapeleaza functia de connect, serverul trebuie sa-i dea accept
              // parametri - socket, adresa variabilei de adresa client, adresa variabilei de dimensiunea clientului
              c = accept(s, (struct sockaddr *) &client, &l);
              // returneaza un client descriptor pe succes, altfel -1, testam pentru erori ca atare
              if (c < 0) {
                     printf("accept error: %d", err);
                     continue; // aici nu dam return, inchidem doar call actual al codului din while - mergem la urmatoarea executare a while-ului
                     // practic, daca da de continue, ignora tot codul de mai jos, se intoarce in while
              }
              // printam datele clientului conectat ip:port
              // inet_ntoa e inversa lui inet_addr, mai exact converteste dintr-un int intr-un char* adresa ip
              // client.sin_port e data venita din network, ea trebuie convertita
              printf("Incomming connected client from: %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
              // serving the connected client

                     // functiile de send si recv folosite pentru a trimite si a primi date de la server - as expected :)
              // parametrii - socket, adresa variabilei, dimensiunea variabilei, flag-uri ( de obicei 0 )
              // atat functia de send cat si cea de recv returneaza numarul de bytes trimis sau -1 in caz de eroare - 
              int res = recv(c, (char*)&a, sizeof(a), 0);

              //verificam erori la primirea datelor
              if (res != sizeof(a)) printf("Error receiving operand\n");
              res = recv(c, (char*)&b, sizeof(b), 0);
              if (res != sizeof(b)) printf("Error receiving operand\n");
 
              // pentru ca primim date din retea ce e posibil (chiar probabil) sa aiba alt tip de reprezentare, folosim ntohs (network to host short)
              // ea face conversia inversa fata de la htons
              a = ntohs(a);
              b = ntohs(b);
              // calculam suma
              suma = a + b;
              // folosim functia htons (host to network short) pentru a converti un numar 
              //din orice protocol se stocare ar avea ( little Endian/ big Endian - de obicei little Endian) la protocolul de stocare al retelei (de obicei big Endian)
              suma = htons(suma);
              res = send(c, (char*)&suma, sizeof(suma), 0);
              // verificam daca s-au trimis corect datele - protocolul TCP verifica validitatea datelor trimise
              if (res != sizeof(suma)) printf("Error sending result\n");
              // inchidem socketul de client
              close(c);
       }
}
 