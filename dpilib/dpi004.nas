[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "dpi004.nas"]

		GLOBAL	_inw

[SECTION .text]

_inw:	; void api_putchar(int c);
		MOV		EDX,4
		MOV		EAX,[ESP+4]		; c
		INT		0x80
		RET
