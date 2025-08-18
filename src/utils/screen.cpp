#include "screen.h"
#include "io.h"

static int cursor_row = 0;
static int cursor_col = 0;

static volatile uint8_t* video = (uint8_t*)0xB8000;
static uint8_t cursor_attr = (LIGHT_GRAY | (BLACK << 4)); // (fg | (bg << 4))

static inline void hardware_cursor_update() {
    uint16_t pos = (uint16_t)(cursor_row * VGA_WIDTH + cursor_col);
    outb(0x3D4, 14); // Set high byte of cursor position
    outb(0x3D5, (uint8_t)(pos >> 8)); // High byte
    outb(0x3D4, 15); // Set low byte of cursor position
    outb(0x3D5, (uint8_t)(pos & 0xFF)); // Low byte
}

static inline void put_at(int row, int col, char ch, uint8_t attr) {
    const int index = (row * VGA_WIDTH + col) * 2;
    video[index] = (uint8_t)ch; // Character
    video[index + 1] = attr;    // Attribute
}

static void scroll_if_req() { // Scrolls everything by 1 line.
    if(cursor_row < VGA_HEIGHT) return;
    for(int row = 1; row < VGA_HEIGHT; ++row) {
        for(int col = 0; col < VGA_WIDTH; ++col) {
            const int from = (row * VGA_WIDTH + col) * 2;
            const int to = ((row - 1) * VGA_WIDTH + col) * 2;
            video[to] = video[from];
            video[to + 1] = video[from + 1];
        }
    }
    for(int col = 0; col < VGA_WIDTH; ++col) put_at(VGA_HEIGHT - 1, col, ' ', cursor_attr);
    cursor_row = VGA_HEIGHT - 1;
}

void screen_init() {
    hardware_cursor_update();
}

void clear_screen() {
    for(int row = 0; row < VGA_HEIGHT; ++row) {
        for(int col = 0; col < VGA_WIDTH; ++col) {
            put_at(row, col, ' ', cursor_attr); // Blank space character
        }
    }
    cursor_col = 0;
    cursor_row = 0;
    hardware_cursor_update();
}

void set_color(VGAColor fg, VGAColor bg) {
    cursor_attr = (uint8_t)(fg | (bg << 4));
}

void putchar(char ch) {
    if(ch == '\n') {
        cursor_col = 0;
        ++cursor_row;
    } else if(ch == '\r') {
        cursor_col = 0;
    } else if(ch == '\t') {
        int next = ((cursor_col / 4) + 1) * 4;
        while(cursor_col < next && cursor_col < VGA_WIDTH) {
            put_at(cursor_row, cursor_col, ' ', cursor_attr);
            ++cursor_col;
        }
    } else {
        put_at(cursor_row, cursor_col, ch, cursor_attr);
        ++cursor_col;
        if(cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            ++cursor_row;
        }
    }

    scroll_if_req();
    hardware_cursor_update();
}

void print(const char* str) {
    for(size_t i = 0; str[i] != '\0'; ++i) putchar(str[i]);
}

void println(const char* s) {
    print(s);
    putchar('\n');
}
