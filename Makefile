SRC_DIRS = kernel flibc/lib

# Collect all .c and .s files
C_SRCS = $(shell find $(SRC_DIRS) -type f -name '*.c')
S_SRCS = $(shell find $(SRC_DIRS) -type f -name '*.s')

# Convert them to .o objects
C_OBJS  = $(C_SRCS:.c=.o)
S_OBJS  = $(S_SRCS:.s=.o)

OBJECTS = $(C_OBJS) $(S_OBJS)

CC = gcc

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	 -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c \
	 -Iflibc/include \
	 -Iinclude/kernel

LDFLAGS = -T link.ld -melf_i386

AS = nasm

ASFLAGS = -f elf32

all: kernel.elf fikos.iso

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

fikos.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	grub-mkrescue -o fikos.iso iso

run: fikos.iso
	qemu-system-i386 -cdrom fikos.iso -vga std -serial file:serial.log

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	find $(SRC_DIRS) -type f -name '*.o' -exec rm -f {} +
	rm -f *.o kernel.elf fikos.iso
