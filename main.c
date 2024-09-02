#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

void generar_usuario(char *usuario, int largo) {
    const char *vocales = "aeiou";
    const char *consonantes = "bcdfghjklmnpqrstvwxyz";
    int esVocal = rand() % 2;

    for (int i = 0; i < largo; i++) {
        if (esVocal) {
            usuario[i] = vocales[rand() % strlen(vocales)];
        } else {
            usuario[i] = consonantes[rand() % strlen(consonantes)];
        }
        esVocal = !esVocal;
    }
    usuario[largo] = '\0';
}

void generar_password(char *password, int largo) {
    const char *characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (int i = 0; i < largo; i++) {
        password[i] = characters[rand() % strlen(characters)];
    }
    password[largo] = '\0';
}

int main() {
    WSADATA wsa;
    SOCKET server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int c;
    char mensajeCliente[1024];
    char respuesta[1024];

    srand(time(NULL));

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket : %d", WSAGetLastError());
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        return 1;
    }

    listen(server_sock, 3);

    printf("Esperando conexiones entrantes...\n");
    c = sizeof(struct sockaddr_in);

    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &c);
    if (client_sock == INVALID_SOCKET) {
        printf("accept failed with error code : %d", WSAGetLastError());
        return 1;
    }
    printf("Conexion aceptada.\n");


    int read_size;
    while ((read_size = recv(client_sock, mensajeCliente, sizeof(mensajeCliente), 0)) > 0) {
        mensajeCliente[read_size] = '\0';

        char type = mensajeCliente[0];
        int largo = atoi(mensajeCliente + 1);

        if (type == 'U') {
            if (largo >= 5 && largo <= 15) {
                generar_usuario(respuesta, largo);
            } else {
                snprintf(respuesta, sizeof(respuesta), "Largo invalido para nombre de usuario");
            }
        } else if (type == 'P') {
            if (largo >= 8 && largo < 50) {
                generar_password(respuesta, largo);
            } else {
                snprintf(respuesta, sizeof(respuesta), "Largo invalido para contrasenia");
            }
        } else {
            snprintf(respuesta, sizeof(respuesta), "Solicitud no reconocida");
        }

        send(client_sock, respuesta, strlen(respuesta), 0);
    }

    if (read_size == 0) {
        printf("Cliente desconectado.\n");
    } else if (read_size == -1) {
        printf("recv failed with error code : %d", WSAGetLastError());
    }

    closesocket(client_sock);
    closesocket(server_sock);
    WSACleanup();

    return 0;
}
