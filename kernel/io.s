;; arg: addr, data
global outb
outb:
    mov dx, [esp + 4]
	mov eax, [esp + 8]
	mov al, al
	out dx, al
	ret

;; arg: addr
global inb
inb:
	mov dx, [esp + 4]
	in al, dx
	movzx eax, al
	ret

	; void outw(uint16_t port, uint16_t value)
global outw
outw:
    mov dx, [esp + 4]  ; port (16-bit)
    mov ax, [esp + 8]  ; value (16-bit)
    out dx, ax
    ret
