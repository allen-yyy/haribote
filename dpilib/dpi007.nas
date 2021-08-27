[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "dpi007.nas"]

		GLOBAL	_message_send

[SECTION .text]

_message_send:	; void api_putchar(int c);
		MOV		EDX,7
		MOV		EAX,[ESP+4]		; c
		MOV		ECX,[ESP+8]
		INT		0x40
		RET
