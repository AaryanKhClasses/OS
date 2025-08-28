# Writing a Kernel
This is the "guide" or the "steps" I followed to write this "hobby project" kernel. This is not a tutorial, but rather a documentation of my journey. I will try to explain the steps I took and the resources I used.

## The Idea
This idea popped up to me while scrolling through YouTube and a video named "Writing my own OS" popped up. This was a few years ago, but I didn't have the time or the knowledge to start such a project. I recently made a compiler as a hobby project in C++ and I thought, why not try to write a kernel?

I already had a lot of experience with Typescript and a fair amount of experience with C++. I also had some experience with low-level programming, as I had a course on assembly language in my school. And hence, I decided to give it a try - writing an Operating System in Typescript.

But wait a minute, Typescript is a high-level language, how can I write a kernel in it? Well, the answer is simple - I can't. I did some research and found out that I can write a kernel in C++ and the userland applications in Typescript. This way, I can leverage the power of C++ for low-level programming and the ease of use of Typescript for userland applications.

This is a "journey log" of my experience writing a kernel in C++ and userland applications in Typescript. I will try to explain the steps I took and the resources I used.

## Pre-requisites
Before starting, I needed to set up my development environment. I already had my experience of C++ not working on Windows, so I decided to use WSL (Windows Subsystem for Linux) for this project. I installed Ubuntu 20.04 on WSL and set up my development environment. I required the following tools for C++:
- `gcc` - The GNU Compiler Collection
- `g++` - The GNU C++ Compiler
- `nasm` - The Netwide Assembler
- `ld` - The GNU Linker
- `make` - A build automation tool
- `qemu` - A generic and open source machine emulator and virtualizer
- `gdb` - The GNU Debugger

and some more tools along the way, but for now these are the main tools I needed.

I am using VSCode - Insiders as my code editor (yes, the thing that uninstalls itself every week). I also installed the following extensions:
- `C/C++` - by Microsoft
- `Makefile Tools` - by Microsoft

and some themes and icons, but these are the main extensions I needed.

> [!NOTE]
> In my honest opinion, you shouldn't just randomly start writing a kernel. You should have a good understanding of how operating systems work, how low-level programming works. You should also have a good understanding of C/C++ and assembly language.

## The First Step
### Bootloaders and GRUB
Before you start writing a kernel, you need to know how to run it. A kernel is just a piece of code that runs on the hardware. But how does the hardware know where to find the kernel? This is where bootloaders come into play.

A `bootloader` is a small program that runs when the computer is powered on. It is responsible for loading the operating system into memory and transferring control to it. There are many bootloaders available, but the most popular one is `GRUB`.

GRUB (GRand Unified Bootloader) is a bootloader package from the GNU Project. Instead of writing my own bootloader, I decided to use GRUB. It is a well-tested and widely used bootloader that supports many file systems and architectures.

### The Multiboot Specification
To make my kernel compatible with GRUB, I needed to follow the `Multiboot Specification`. The Multiboot Specification is a standard for bootloaders to load operating systems. It defines how the bootloader should pass information to the kernel and how the kernel should be structured. You can find the Multiboot Specification [here](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html).

### The `grub.cfg` file:
The `grub.cfg` file is the configuration file for GRUB. It is used to define the menu entries and the options for each entry. Here is a simple `grub.cfg` file that I used to boot my kernel:

```cfg
set timeout=0
set default=0

menuentry "OS" {
    multiboot /boot/kernel.bin
}
```

- The `set timeout=0` line sets the timeout for the menu to 0 seconds, so it will boot the default entry immediately.
- The `set default=0` line sets the default entry to the first entry (0-indexed).
- The `menuentry "OS"` line defines a menu entry with the name "OS".
- The `multiboot /boot/kernel.bin` line tells GRUB to load the kernel from the `/boot/kernel.bin` file.

We will talk about the kernel file later, but for now, this is how you can configure GRUB to load your kernel.

### The Multiboot Header
To make my kernel compatible with the Multiboot Specification, I needed to add a `multiboot_header.asm` file to my kernel. This file contains the Multiboot header, which is a special structure that tells the bootloader how to load the kernel.

```asm
section .multiboot
align 4
    dd 0x1BADB002
    dd 0x0
    dd -(0x1BADB002)

section .text
global _start
_start:
    extern kernel_main
    call kernel_main
    cli

.hang:
    hlt
    jmp .hang
```

- The `section .multiboot` line defines a new section in the binary called `.multiboot`.
- The `align 4` line aligns the section to a 4-byte boundary.
- The `dd 0x1BADB002` line is the magic number that identifies the Multiboot header.
- The `dd 0x0` line is the flags field, which is set to 0 for now.
- The `dd -(0x1BADB002)` line is the checksum field, which is calculated as the negative sum of the magic number and the flags field. This is because the sum of all three fields must be 0.
- The `section .text` line defines a new section in the binary called `.text`, which contains the code.
- The `global _start` line makes the `_start` label visible to the linker.
- The `_start:` label is the entry point of the kernel. This is where the bootloader will transfer control to the kernel.
- The `extern kernel_main` line declares the `kernel_main` function, which is defined in the C++ code.
- The `call kernel_main` line calls the `kernel_main` function.
- The `cli` instruction disables interrupts.
- The `.hang:` label is a label for the infinite loop.
- The `hlt` instruction halts the CPU until the next interrupt.
- The `jmp .hang` line jumps to the `.hang` label, creating an infinite loop.

We will talk about what interrupts are and why we need to disable them later, but for now, this is how you can create a Multiboot header for your kernel.

### Linkers
To create a kernel binary that can be loaded by GRUB, I needed to link the assembly and C++ code together. This is where the `linker.ld` file comes into play. The `linker.ld` file is a linker script that tells the linker how to link the object files together.

```ld
ENTRY(_start)

SECTIONS {
    . = 1M;

    .multiboot : {
        *(.multiboot)
    }

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}
```

- The `ENTRY(_start)` line tells the linker that the entry point of the kernel is the `_start` label.
- The `SECTIONS` block defines the sections of the binary.
- The `. = 1M;` line sets the starting address of the kernel to 1MB. This is because the Multiboot Specification requires the kernel to be loaded at or above 1MB.
- The `.multiboot : { *(.multiboot) }` block tells the linker to place the `.multiboot` section at the beginning of the binary
- The `.text : { *(.text) }` block tells the linker to place the `.text` section after the `.multiboot` section.
Similarly, the `.data` and `.bss` sections are placed after the `.text` section.

### Now what?
Until now, we only have the bootloader and the Multiboot header. We don't have any actual kernel code yet. The next step is to write the kernel code in C++. We will talk about that in the next section.

[Click Here](02_What_Is_VGA.md) to go to the next section about VGA and how to print text on the screen - The Actual First Step in Writing a Kernel.