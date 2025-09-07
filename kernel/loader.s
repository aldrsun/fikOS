extern kmain

section .multiboot_header
header_start:
	align 8
    dd 0xe85250d6                ; magic number (multiboot 2)
    dd 0                         ; architecture 0 (protected mode i386)
    dd header_end - header_start ; header length
    ; checksum
    dd -(0xe85250d6 + 0 + (header_end - header_start))

    ; ========================
    ; Flags tag (type = 4)
    ; ========================
    align 8
    dw 4          ; type = 4 (Flags)
    dw 0          ; flags
    dd 12         ; size of the tag in bytes (type+flags+size+console_flags)
    dd 0          ; console_flags (0 = no console info)
    dd 0          ; padding to align to 8 bytes (12 + 4 = 16)

    ; ========================
    ; Framebuffer tag (type = 5)
    ; ========================
    align 8
    dw 5          ; type = 5 (Framebuffer)
    dw 0          ; flags
    dd 20         ; size (type+flags+size+width+height+depth = 20)
    dd 1024       ; width
    dd 768        ; height
    dd 32         ; bits per pixel
    dd 0          ; padding to align to 8 bytes (20 + 4 = 24)

    ; ========================
    ; End tag
    ; ========================
    align 8
    dd 0          ; type = 0
    dd 8          ; size = 8
header_end:

; ==============================================================================
; Global Descriptor Table (GDT)
; This table is essential for protected mode operation.
; ==============================================================================

section .data
; Null Descriptor
gdt_null:
    dd 0x0
    dd 0x0

; Kernel Code Segment Descriptor
gdt_code:
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 0x9A         ; Access Byte: Present, Ring 0, Code, Readable
    db 0xCF         ; Flags + Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

; Kernel Data Segment Descriptor
gdt_data:
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 0x92         ; Access Byte: Present, Ring 0, Data, Writable
    db 0xCF         ; Flags + Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

; GDT Pointer (6 bytes)
gdt_ptr:
    dw $ - gdt_null - 1  ; Limit: size of GDT - 1
    dd gdt_null          ; Base: address of the GDT

; ==============================================================================
; Kernel Entry Point
; The entry point after the bootloader has loaded the kernel.
; ==============================================================================

section .text
bits 32

global start
start:
	mov esp, kernel_stack + KERNEL_STACK_SIZE
	push ebx    ; push the address of the multiboot info structure
    push eax    ; push the magic number

	; Load the GDT
    lgdt [gdt_ptr]

    ; Far jump to reload the code segment register (CS)
    ; 0x8 is the selector for gdt_code
    jmp 0x8:protected_mode_entry

protected_mode_entry:
    ; Now that CS is set, reload other segment registers
    mov ax, 0x10   ; 0x10 is the selector for gdt_data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

	call kmain

.loop
	hlt
	jmp .loop

section .bss
align 4

KERNEL_STACK_SIZE	equ 4096
kernel_stack:
	resb KERNEL_STACK_SIZE
