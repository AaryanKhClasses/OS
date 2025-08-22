#pragma once
#include <stdint.h>
#include "../interrupts/isr.h"

void keyboard_init();
int kb_get_scancode_nonblocking();

extern const char scancode_to_ascii[128];
