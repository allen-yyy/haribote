[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api028.nas"]

		GLOBAL	_api_gettime

[SECTION .text]

_api_gettime:		; int api_gettime(void);
		MOV		EDX,28
		INT		0x40
		RET