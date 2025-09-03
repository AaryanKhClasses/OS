CXX = g++
LD = ld
AS = nasm

CXXFLAGS = -ffreestanding -fno-exceptions -fno-rtti -m32 -Wall -Wextra -I src/utils -I src/interrupts -I src/drivers
LDFLAGS = -m elf_i386 -T linker.ld

OUT_DIR = out
SRC_DIR = src
UTILS_DIR = $(SRC_DIR)/utils
INTR_DIR = $(SRC_DIR)/interrupts
DRV_DIR  = $(SRC_DIR)/drivers
SCH_DIR  = $(SRC_DIR)/scheduler
MEM_DIR  = $(SRC_DIR)/mem

# Objects
$(OUT_DIR)/multiboot_header.o: $(SRC_DIR)/multiboot_header.asm
	$(AS) -f elf32 $< -o $@

$(OUT_DIR)/kernel.o: $(SRC_DIR)/kernel.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/screen.o: $(UTILS_DIR)/screen.cpp $(UTILS_DIR)/screen.h $(UTILS_DIR)/io.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/idt.o: $(INTR_DIR)/idt.cpp $(INTR_DIR)/idt.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/isr_asm.o: $(INTR_DIR)/isr.asm
	$(AS) -f elf32 $< -o $@

$(OUT_DIR)/isr.o: $(INTR_DIR)/isr.cpp $(INTR_DIR)/isr.h $(UTILS_DIR)/screen.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/irq_asm.o: $(INTR_DIR)/irq.asm
	$(AS) -f elf32 $< -o $@

$(OUT_DIR)/irq.o: $(INTR_DIR)/irq.cpp $(INTR_DIR)/irq.h $(INTR_DIR)/isr.h $(INTR_DIR)/idt.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/pic.o: $(INTR_DIR)/pic.cpp $(INTR_DIR)/pic.h $(UTILS_DIR)/io.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/keyboard.o: $(DRV_DIR)/keyboard.cpp $(DRV_DIR)/keyboard.h $(INTR_DIR)/irq.h $(UTILS_DIR)/io.h $(UTILS_DIR)/screen.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/pit.o: $(DRV_DIR)/pit.cpp $(DRV_DIR)/pit.h $(INTR_DIR)/irq.h $(UTILS_DIR)/io.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/scheduler.o: $(SCH_DIR)/scheduler.cpp $(SCH_DIR)/scheduler.h $(DRV_DIR)/pit.h $(DRV_DIR)/keyboard.h $(UTILS_DIR)/screen.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/ctx_switch.o: $(SCH_DIR)/ctx_switch.asm
	$(AS) -f elf32 $< -o $@

$(OUT_DIR)/string.o: $(UTILS_DIR)/string.cpp $(UTILS_DIR)/string.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/paging.o: $(MEM_DIR)/paging.cpp $(MEM_DIR)/paging.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/pmm.o: $(MEM_DIR)/pmm.cpp $(MEM_DIR)/pmm.h $(UTILS_DIR)/string.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT_DIR)/heap.o: $(MEM_DIR)/heap.cpp $(MEM_DIR)/heap.h $(UTILS_DIR)/string.h $(MEM_DIR)/pmm.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link Kernel
$(OUT_DIR)/kernel.bin: $(OUT_DIR)/multiboot_header.o $(OUT_DIR)/kernel.o $(OUT_DIR)/string.o \
	$(OUT_DIR)/screen.o $(OUT_DIR)/idt.o $(OUT_DIR)/isr_asm.o $(OUT_DIR)/isr.o \
	$(OUT_DIR)/irq_asm.o $(OUT_DIR)/irq.o $(OUT_DIR)/pic.o $(OUT_DIR)/keyboard.o $(OUT_DIR)/pit.o \
	$(OUT_DIR)/ctx_switch.o $(OUT_DIR)/scheduler.o \
	$(OUT_DIR)/paging.o $(OUT_DIR)/pmm.o $(OUT_DIR)/heap.o
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

# Debug
debug: os.iso
	qemu-system-i386 -cdrom $(OUT_DIR)/os.iso -d int -no-reboot -no-shutdown -D qemu.log

# Clean
clean:
	rm -rf $(OUT_DIR) iso os.iso

# Ensure out dir exists
$(shell mkdir -p $(OUT_DIR))
