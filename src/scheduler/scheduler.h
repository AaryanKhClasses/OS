#pragma once
#include <stdint.h>

typedef void (*task_fn_t)(void*);

void scheduler_init();
int task_create(task_fn_t entry, void* arg, const char* name);
void yield();
void task_sleep_ms(uint32_t ms);
void scheduler_start();
