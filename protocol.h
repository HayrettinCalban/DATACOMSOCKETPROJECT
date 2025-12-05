#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int calculate_parity(const char *data) {
    int ones = 0;
    for (int i = 0; i < strlen(data); i++) {
        char c = data[i];
        for (int j = 0; j < 8; j++) {
            if ((c >> j) & 1) ones++;
        }
    }
    return (ones % 2 == 1) ? 1 : 0;
}

uint16_t calculate_crc16(const char *data) {
    uint16_t crc = 0xFFFF;
    size_t len = strlen(data);
    
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint8_t)data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = (crc >> 1);
            }
        }
    }
    return crc;
}

#endif