[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "dpi007.nas"]

		GLOBAL	_putchr

[SECTION .text]

_putchr:	; void api_putchar(int c);
		MOV		EDX,9
		MOV		EAX,[ESP+4]		; c
		MOV		ECX,[ESP+8]
		MOV		BL,[ESP+12]
		INT		0x80
		RET
