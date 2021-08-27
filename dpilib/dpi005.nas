[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "dpi005.nas"]

		GLOBAL	_outl

[SECTION .text]

_outl:	; void api_putchar(int c);
		MOV		EDX,5
		MOV		EAX,[ESP+4]
		MOV		ECX,[ESP+8]; c
		INT		0x80
		RET
