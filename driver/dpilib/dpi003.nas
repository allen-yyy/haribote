[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "dpi003.nas"]

		GLOBAL	_outw

[SECTION .text]

_outw:	; void api_putchar(int c);
		MOV		EDX,3
		MOV		EAX,[ESP+4]	
		MOV		EAX,[ESP+8]; c
		INT		0x80
		RET
