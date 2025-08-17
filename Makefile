CXX = g++
LD = ld
AS = nasm

CXXFLAGS = -ffreestanding -fno-exceptions -fno-rtti -m32 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

OUTDIR = out

all: $(OUTDIR)/os.iso

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/multiboot_header.o: multiboot_header.asm | $(OUTDIR)
	$(AS) -f elf32 multiboot_header.asm -o $@

$(OUTDIR)/kernel.o: kernel.cpp | $(OUTDIR)
	$(CXX) $(CXXFLAGS) -c kernel.cpp -o $@

$(OUTDIR)/kernel.bin: $(OUTDIR)/multiboot_header.o $(OUTDIR)/kernel.o
	$(LD) $(LDFLAGS) $^ -o $@

$(OUTDIR)/os.iso: $(OUTDIR)/kernel.bin grub.cfg
	mkdir -p iso/boot/grub
	cp $(OUTDIR)/kernel.bin iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso

run: $(OUTDIR)/os.iso
	qemu-system-i386 -cdrom $<

clean:
	rm -rf $(OUTDIR) iso
