#include "string.h"

extern "C" void* memset(void* dest, int val, size_t len) {
    uint8_t* p = (uint8_t*)dest;
    for (size_t i = 0; i < len; i++) p[i] = (uint8_t)val;
    return dest;
}

extern "C" void* memcpy(void* dest, const void* src, size_t len) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < len; i++) d[i] = s[i];
    return dest;
}

extern "C" int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

extern "C" size_t strlen(const char* s) {
    const char* p = s;
    while (*p) p++;
    return p - s;
}

void int_to_str(uint32_t value, char* buf) {
    char temp[16];
    int i = 0, j = 0;
    if(value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while(value > 0) {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }

    while(i > 0) buf[j++] = temp[--i];
    buf[j] = '\0';
}

void hex_to_str(uint32_t value, char* buf) {
    const char* hex = "0123456789ABCDEF";
    buf[0] = '0';
    buf[1] = 'x';
    for(int i = 0; i < 8; i++) {
        buf[2 + i] = hex[(value >> (28 - i * 4)) & 0xF];
    }
    buf[10] = '\0';
}