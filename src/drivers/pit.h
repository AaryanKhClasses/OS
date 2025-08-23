#pragma once
#include <stdint.h>

void pit_init(uint32_t freq_hz); // Inits IRQ0 with given Hz Frequency
uint32_t pit_get_ticks(); // Returns tick count since boot
void sleep_ms(uint32_t ms); // Sleeps for given milliseconds
