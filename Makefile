CXX = g++
LD = ld
AS = nasm

CXXFLAGS = -ffreestanding -fno-exceptions -fno-rtti -m32 -Wall -Wextra -I src/utils
LDFLAGS = -m elf_i386 -T linker.ld

OUT_DIR = out
SRC_DIR = src
UTILS_DIR = $(SRC_DIR)/utils
INTR_DIR = $(SRC_DIR)/interrupts
DRV_DIR  = $(SRC_DIR)/drivers

# Objects
$(OUT_DIR)/multiboot_header.o: $(SRC_DIR)/multiboot_header.asm
	$(AS) -f elf32 $< -o $@

$(OUT_DIR)/kernel.o: $(SRC_DIR)/kernel.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/screen.o: $(UTILS_DIR)/screen.cpp $(UTILS_DIR)/screen.h $(UTILS_DIR)/io.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/idt.o: $(INTR_DIR)/idt.cpp $(INTR_DIR)/idt.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/pic.o: $(INTR_DIR)/pic.cpp $(INTR_DIR)/pic.h $(UTILS_DIR)/io.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/keyboard.o: $(DRV_DIR)/keyboard.cpp $(DRV_DIR)/keyboard.h $(UTILS_DIR)/io.h $(UTILS_DIR)/screen.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/isr.o: $(INTR_DIR)/isr.asm
	$(AS) -f elf32 $< -o $@

# Link kernel
$(OUT_DIR)/kernel.bin: $(OUT_DIR)/multiboot_header.o $(OUT_DIR)/kernel.o \
	$(OUT_DIR)/screen.o $(OUT_DIR)/idt.o $(OUT_DIR)/pic.o \
	$(OUT_DIR)/keyboard.o $(OUT_DIR)/isr.o
	$(LD) $(LDFLAGS) -o $@ $^

# ISO build
os.iso: $(OUT_DIR)/kernel.bin grub.cfg
	mkdir -p iso/boot/grub
	cp $(OUT_DIR)/kernel.bin iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $(OUT_DIR)/os.iso iso

# All
all: os.iso

# Run
run: os.iso
	qemu-system-i386 -cdrom $(OUT_DIR)/os.iso

# Clean
clean:
	rm -rf $(OUT_DIR) iso os.iso

# Ensure out dir exists
$(shell mkdir -p $(OUT_DIR))
