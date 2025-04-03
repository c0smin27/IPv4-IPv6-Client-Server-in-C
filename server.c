#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void client_12(int client_sock) {
    struct sockaddr_in6 server_addr;
    int web_sock = socket(PF_INET6, SOCK_STREAM, 0);
    if (web_sock == -1) {
        printf("Eroare la crearea socket-ului conexiune server extern.\n");
        exit(1);
    } else {
        printf("Socket conexiune server extern creat cu succes.\n");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = PF_INET6;
    inet_pton(PF_INET6, "::1", &server_addr.sin6_addr);
    server_addr.sin6_port = htons(80);

    int con = connect(web_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (con == -1) {
        printf("Eroare la stabilirea conexiunii cu server-ul.\n");
        close(web_sock);
        exit(1);
    } else {
        printf("Conexiune la server-ul extern realizata cu succes.\n");
    }

    char *message = "GET / HTTP/1.0\r\n\r\n";
    int trimite = send(web_sock, message, strlen(message), 0);
    if (trimite == -1) {
        printf("Eroare la trimiterea mesajului.\n");
        close(web_sock);
        exit(1);
    } else {
        printf("Request-ul a fost trimis catre server cu succes.\n");
    }

    FILE *file = fopen("index.html", "w");
    if (file == NULL) {
        printf("Eroare la crearea fisierului html.\n");
        close(web_sock);
        exit(1);
    }

    char buffer[1025];
    int primit;
    while ((primit = recv(web_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[primit] = '\0';
        if (fwrite(buffer, 1, primit, file) != primit) {
            printf("Eroare la scrierea in fisier.\n");
            break;
        }
        if (send(client_sock, buffer, primit, 0) == -1) {
            printf("Eroare la trimiterea datelor catre client.\n");
            break;
        }
    }

    if (primit == -1) {
        printf("Eroare la primirea datelor.\n");
        exit(1);
    } else {
        printf("Conexiune server extern inchisa cu succes.\n");
    }

    fclose(file);
    close(web_sock);
}

int main() {
    struct sockaddr_in server_addr;
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Eroare la crearea socket-ului conexiune clienti.\n");
        exit(1);
    } else {
        printf("Socket conexiune client creat cu succes.\n");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(22214);

    int legatura = bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (legatura == -1) {
        printf("Eroare la crearea functiei bind().\n");
        exit(1);
    } else {
        printf("Bind creat cu succes. Se asteapta IP si port.\n");
    }

    int asculta = listen(sock, 5);
    if (asculta == -1) {
        printf("Eroare la creare functiei listen().\n");
        exit(1);
    } else {
        printf("Listen creat cu succes. Se asteapta cererea clientilor.\n");
    }

    char buffer[1025];
    int primit;
    int admis = -1;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        admis = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
        if (admis == -1) {
            printf("Eroare la crearea functiei accept().\n");
            exit(1);
        } else {
            printf("Conexiune client acceptata cu succes.\n");

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            int client_port = ntohs(client_addr.sin_port);

            FILE *file = fopen("log.txt", "a");
            if (file == NULL) {
                printf("Eroare la deschiderea fisierului txt.\n");
            } else {
                fprintf(file, "Client conectat: IP %s, Port %d\n", client_ip, client_port);
                fclose(file);
            }
        }

        while ((primit = recv(admis, buffer, sizeof(buffer), 0)) > 0) {
            if (primit == -1) {
                printf("Eroare la primirea datelor.\n");
                exit(1);
            } else {
                printf("Date primite de la client cu succes.\n");
            }

            buffer[primit] = '\0';
            if (strcmp(buffer, "12#") == 0) {
                printf("Comanda 12 functioneaza.\n");
                client_12(admis);
            } else {
                char *message = "Comanda neimplementata.";
                int trimite = send(admis, message, strlen(message) + 1, 0);
                if (trimite == -1) {
                    printf("Eroare la trimiterea mesajului.\n");
                    exit(1);
                } else {
                    printf("Mesaj trimis catre client cu succes.\n");
                }
            }
        }

        int inchis = close(admis);
        if (inchis == -1) {
            printf("Eroare la inchiderea conexiunii cu clientul.\n");
        } else {
            printf("Clientul a inchis conexiunea.\n");
        }
    }

    return 0;
}
