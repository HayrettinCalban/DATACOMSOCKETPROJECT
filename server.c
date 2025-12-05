#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define LISTEN_PORT 8080
#define DEST_PORT 8081
#define BUFFER_SIZE 1024

void inject_bit_flip(char *data) {
    int len = strlen(data);
    if (len > 0) {
        int char_idx = rand() % len;
        int bit_idx = rand() % 8;
        data[char_idx] ^= (1 << bit_idx);
        printf("[LOG] Bit Flip uygulandi: Index %d, Bit %d\n", char_idx, bit_idx);
    }
}

void inject_char_sub(char *data) {
    int len = strlen(data);
    if (len > 0) {
        int idx = rand() % len;
        data[idx] = 'X';
        printf("[LOG] Character Substitution uygulandi: Index %d -> X\n", idx);
    }
}

int main() {
    srand(time(NULL));
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { perror("Socket failed"); exit(EXIT_FAILURE); }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(LISTEN_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { perror("Bind failed"); exit(EXIT_FAILURE); }
    if (listen(server_fd, 3) < 0) { perror("Listen failed"); exit(EXIT_FAILURE); }

    printf("Server (Corruptor) Client 1'i bekliyor (Port: %d)...\n", LISTEN_PORT);

    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    read(client_socket, buffer, BUFFER_SIZE);
    printf("[LOG] Gelen Paket: %s\n", buffer);

    char temp_buf[BUFFER_SIZE];
    strcpy(temp_buf, buffer);
    
    char *data_part = strtok(temp_buf, "|");
    char *rest = strtok(NULL, "");

    if (rand() % 2 == 0) {
        if (rand() % 2 == 0) inject_bit_flip(data_part);
        else inject_char_sub(data_part);
    } else {
        printf("[LOG] Sans eseri veri bozulmadi.\n");
    }

    char final_packet[BUFFER_SIZE];
    sprintf(final_packet, "%s|%s", data_part, rest);
    printf("[LOG] Iletilen Paket: %s\n", final_packet);

    int dest_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);

    if (connect(dest_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        printf("Client 2'ye baglanilamadi! Lutfen once Client 2'yi baslatin.\n");
    } else {
        send(dest_sock, final_packet, strlen(final_packet), 0);
    }

    close(dest_sock);
    close(client_socket);
    close(server_fd);
    return 0;
}