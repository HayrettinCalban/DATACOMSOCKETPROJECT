#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h"

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char text[500];
    int choice;
    char method[20];
    char control_info[20];
    char packet[BUFFER_SIZE];

    printf("Gonderilecek Metni Girin: ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = 0; 

    printf("Yontem Secin:\n1. Parity\n2. CRC-16\nSecim: ");
    scanf("%d", &choice);

    if (choice == 1) {
        strcpy(method, "PARITY");
        int parity = calculate_parity(text);
        sprintf(control_info, "%d", parity);
    } else {
        strcpy(method, "CRC16"); // [cite: 22]
        uint16_t crc = calculate_crc16(text);
        sprintf(control_info, "%X", crc);
    }

    sprintf(packet, "%s|%s|%s", text, method, control_info);
    printf("Olusturulan Paket: %s\n", packet);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed. Once Server'i baslatin.\n");
        return -1;
    }

    send(sock, packet, strlen(packet), 0);
    printf("Paket Server'a gonderildi.\n");

    close(sock);
    return 0;
}