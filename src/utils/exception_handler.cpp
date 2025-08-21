#include "../utils/screen.h"
#include <stdint.h>

static void print_dec(unsigned v){
    char b[12]; int i=0; if(!v){ print("0"); return; }
    while(v){ b[i++]='0'+(v%10); v/=10; }
    while(i--) { char s[2]={b[i],0}; print(s); }
}

extern "C" void isr_handler_c(uint32_t vec){
    set_color(LIGHT_RED, BLACK);
    print("Exception vector: "); print_dec(vec); print("\n");
    print("Halting.\n");
    for(;;) asm volatile("hlt");
}
