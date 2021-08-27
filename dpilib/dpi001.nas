[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "dpi001.nas"]

		GLOBAL	_outb

[SECTION .text]

_outb:	; void api_putchar(int c);
		MOV		EDX,1
		MOV		EAX,[ESP+4]
		MOV		ECX,[ESP+8]; c
		INT		0x80
		RET
