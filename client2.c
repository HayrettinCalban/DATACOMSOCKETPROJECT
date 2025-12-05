#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h"

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Client 2 (Receiver) dinliyor (Port: %d)...\n", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, BUFFER_SIZE);
    
    char *data = strtok(buffer, "|");
    char *method = strtok(NULL, "|");
    char *received_ctrl_str = strtok(NULL, "|");

    if (!data || !method || !received_ctrl_str) {
        printf("Hatali paket formati!\n");
        return 0;
    }

    printf("\n--- SONUC ---\n");
    printf("Received Data: %s\n", data);
    printf("Method: %s\n", method);
    printf("Sent Check Bits: %s\n", received_ctrl_str);

    char calculated_ctrl_str[20];
    
    if (strcmp(method, "PARITY") == 0) {
        int parity = calculate_parity(data);
        sprintf(calculated_ctrl_str, "%d", parity);
    } else if (strcmp(method, "CRC16") == 0) {
        uint16_t crc = calculate_crc16(data);
        sprintf(calculated_ctrl_str, "%X", crc);
    }

    printf("Computed Check Bits: %s\n", calculated_ctrl_str);

    if (strcmp(received_ctrl_str, calculated_ctrl_str) == 0) {
        printf("Status: DATA CORRECT [cite: 73]\n");
    } else {
        printf("Status: DATA CORRUPTED [cite: 72]\n");
    }

    close(new_socket);
    close(server_fd);
    return 0;
}