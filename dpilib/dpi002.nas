[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "dpi002.nas"]

		GLOBAL	_inb

[SECTION .text]

_inb:	; void api_putchar(int c);
		MOV		EDX,2
		MOV		EAX,[ESP+4]		; c
		INT		0x80
		RET
