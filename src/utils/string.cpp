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
