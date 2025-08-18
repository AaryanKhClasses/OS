#pragma once
#include <stdint.h>

void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_set_masks(uint8_t master_mask, uint8_t slave_mask);
