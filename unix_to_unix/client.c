#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
typedef int SOCKET;
// clientul trimite 2 numere la server, serverul calculeaza suma si o trimtie inapoi clientului
int main() {
      // c-ul o sa fie folosit ca un socket - 
      // A socket is one endpoint of a two-way communication link between two programs running on the network - seamana cu fifo descriptor
      // socketul-ule o variabila de tip int - varianta asta cu typedef pare mai unnecessaryly fancier, dar are sens cand scriem codul comun pentru windows- unix. libraria de windows are tipul SOCKET
      SOCKET c;
       // definesc o structura de tiput internet socket address - socketul targeteaza o adresa de server (un internet socket adress)
       struct sockaddr_in server;
       // definesc variabilele de tip short (16 biti)
       uint16_t a, b, suma;
      
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
       c = socket(AF_INET, SOCK_STREAM, 0);
      // functia returneaza -1 in caz de eroare sau un file descriptor daca totul merge bine
      // verificam exact ca atare daca socketul s-a creat
       if (c < 0) {
              printf("Eroare la crearea socketului client\n");
              return 1;
       }
      
      // initializez toata variabila de server cu 0 - sa n-avem trash in variabila
       memset(&server, 0, sizeof(server));
       // un server adress are 3 proprietati fundamentale. Familie, ip si port

       server.sin_port = htons(1234); // aici setam portul la 1234 - variabila e trimisa in retea, ea trebuie convertita la protocolul de stocare a datelor pentru network, explicatie mai jos
       // portul si ip-ul trebuie sa fie aceleasi cu cele ale serverului, ca altfel nu se poate realiza conectarea
       server.sin_family = AF_INET; // aici seatam familia la AF_INET 
       // grija sa nu existe discrepante intre domeniul definirii socketului si familia adresei - pot aparea erori 
       // aici definim ip-ul serverului. functia inet_addr converteste din char* in int
       // 127.0.0.1 - home ip - aka sistemul se targeteaza singur, indiferent de ce ip public ar avea el
       server.sin_addr.s_addr = inet_addr("127.0.0.1");
      
      // facem conenexiunea socketului la server
      // parametrii - socket, adresa variabilei server, dimenisunea variabilelei server 
      // functia retunreaza 0 la succes şi -1 la eroare, verificam ca atare 
       if (connect(c, (struct sockaddr *) &server, sizeof(server)) < 0) {
              printf("Eroare la conectarea la server\n");
              return 1;
       }
 
      // citirea basic din C - %hu - pentru short-uri
       printf("a = ");
       scanf("%hu", &a);
       printf("b = ");
       scanf("%hu", &b);

      // folosim functia htons (host to network short) pentru a converti un numar 
      //din orice protocol se stocare ar avea ( little Endian/ big Endian - de obicei little Endian) la protocolul de stocare al retelei (de obicei big Endian)
       a = htons(a);
       b = htons(b);

       // functiile de send si recv folosite pentru a trimite si a primi date de la server - as expected :)
       // parametrii - socket, adresa variabilei, dimensiunea variabilei, flag-uri ( de obicei 0 )
       // atat functia de send cat si cea de recv returneaza numarul de bytes trimis sau -1 in caz de eroare - 
       // Aici n-o fost testata validitatea datelor, se poate face fix ca atare <0 -> eroare

       send(c, &a, sizeof(a), 0);
       send(c, &b, sizeof(b), 0);
       recv(c, &suma, sizeof(suma), 0);
       // pentru ca primim date din retea ce e posibil (chiar probabil) sa aiba alt tip de reprezentare, folosim ntohs (network to host short)
       // ea face conversia inversa fata de la htons
       suma = ntohs(suma);
       // printam rezultatul
       printf("Suma este %hu\n", suma);
       // inchidem socketul
       close(c);
}