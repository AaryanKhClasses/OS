#ifndef SCREEN_H
#define SCREEN_H

#include <stddef.h>
#include <stdint.h>

const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;

enum VGAColor: uint8_t {
    BLACK = 0, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY,
    DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE
};

void screen_init();
void clear_screen();
void set_color(VGAColor fg, VGAColor bg);
void putchar(char ch);
void print(const char* str);
void println(const char* str);

#endif
