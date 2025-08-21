#pragma once

void keyboard_init();
extern "C" void irq1_handler();
extern "C" int kb_get_scancode_nonblocking();
extern const char scancode_to_ascii[128];
