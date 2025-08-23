#include "pit.h"
#include "../utils/io.h"
#include "../interrupts/irq.h"
#include <stdint.h>

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43

static volatile uint32_t ticks = 0;
static uint32_t pit_freq = 100;

static inline void pit_set_divisor(uint16_t divisor) {
    outb(PIT_COMMAND, 0x34); // Channel 0, lobyte/hibyte, mode 3, binary
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF)); // Low byte
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF)); // High byte
}

static void pit_irq(Registers*) {
    ++ticks;
}

void pit_init(uint32_t freq_hz) {
    if(freq_hz == 0) freq_hz = 100;
    pit_freq = freq_hz;
    uint32_t divisor = 1193182 / pit_freq; // 1193182 is the PIT input clock frequency
    if(divisor == 0) divisor = 1;
    pit_set_divisor((uint16_t)divisor);
    irq_register_handler(0, pit_irq);
}

uint32_t pit_get_ticks() {
    return ticks;
}

void sleep_ms(uint32_t ms) {
    uint32_t start = pit_get_ticks();
    uint32_t wait_ticks = ((uint32_t)ms * pit_freq + 999) / 1000;
    uint32_t target = start + (wait_ticks ? wait_ticks : 1);

    while(pit_get_ticks() < target) {
        asm volatile("hlt");
    }
}
