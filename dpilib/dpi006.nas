[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "dpi006.nas"]

		GLOBAL	_inl

[SECTION .text]

_inl:	; void api_putchar(int c);
		MOV		EDX,6
		MOV		EAX,[ESP+4]		; c
		INT		0x80
		RET
