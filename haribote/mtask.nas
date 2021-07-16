[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_hlt
	EXTERN	_memman_alloc_4k
	EXTERN	_set_segmdesc
	EXTERN	_load_tr
	EXTERN	_timer_alloc
	EXTERN	_timer_settime
	EXTERN	_farjmp
	EXTERN	_io_cli
	EXTERN	_clts
	EXTERN	_frstor
	EXTERN	_io_sti
	EXTERN	_fnsave
[FILE "mtask.c"]
[SECTION .text]
	GLOBAL	_pid2task
_pid2task:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	POP	EBP
	MOV	EAX,DWORD [_pid+4+EAX*8]
	RET
	GLOBAL	_init_pid
_init_pid:
	PUSH	EBP
	XOR	EAX,EAX
	MOV	EBP,ESP
	MOV	DWORD [_pid+79992],-1
L7:
	MOV	DWORD [_pid+EAX*8],EAX
	INC	EAX
	CMP	EAX,9999
	JLE	L7
	MOV	DWORD [3850],_pid
	POP	EBP
	RET
	GLOBAL	_pid_alloc
_pid_alloc:
	PUSH	EBP
	MOV	EAX,DWORD [_pid+79992]
	MOV	EBP,ESP
	MOV	EDX,DWORD [8+EBP]
	POP	EBP
	MOV	DWORD [_pid+12+EAX*8],EDX
	INC	EAX
	MOV	DWORD [_pid+79992],EAX
	RET
	GLOBAL	_task_now
_task_now:
	MOV	EAX,DWORD [_taskctl]
	PUSH	EBP
	MOV	EBP,ESP
	POP	EBP
	MOV	EDX,DWORD [EAX]
	IMUL	EDX,EDX,408
	LEA	EAX,DWORD [12+EDX+EAX*1]
	MOV	EDX,DWORD [4+EAX]
	MOV	EAX,DWORD [8+EAX+EDX*4]
	RET
	GLOBAL	_task_add
_task_add:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	ECX,DWORD [8+EBP]
	MOV	EDX,DWORD [8+ECX]
	IMUL	EDX,EDX,408
	ADD	EDX,DWORD [_taskctl]
	MOV	EAX,DWORD [12+EDX]
	MOV	DWORD [20+EDX+EAX*4],ECX
	INC	EAX
	MOV	DWORD [12+EDX],EAX
	MOV	DWORD [4+ECX],2
	POP	EBP
	RET
	GLOBAL	_task_remove
_task_remove:
	PUSH	EBP
	XOR	ECX,ECX
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	ESI,DWORD [8+EBP]
	PUSH	EBX
	MOV	EDI,DWORD [3850]
	MOV	EAX,DWORD [8+ESI]
	IMUL	EAX,EAX,408
	ADD	EAX,DWORD [_taskctl]
	LEA	EDX,DWORD [12+EAX]
	CMP	ECX,DWORD [12+EAX]
	JGE	L15
L19:
	CMP	DWORD [8+EDX+ECX*4],ESI
	JE	L15
	INC	ECX
	CMP	ECX,DWORD [EDX]
	JL	L19
L15:
	MOV	EAX,DWORD [4+EDX]
	DEC	DWORD [EDX]
	CMP	ECX,EAX
	JGE	L20
	DEC	EAX
	MOV	DWORD [4+EDX],EAX
L20:
	MOV	EAX,DWORD [EDX]
	CMP	DWORD [4+EDX],EAX
	JL	L21
	MOV	DWORD [4+EDX],0
L21:
	MOV	DWORD [4+ESI],1
	MOV	EBX,DWORD [EDX]
	CMP	ECX,EBX
	JGE	L29
L26:
	MOV	EAX,DWORD [12+EDX+ECX*4]
	MOV	DWORD [8+EDX+ECX*4],EAX
	INC	ECX
	CMP	ECX,EBX
	JL	L26
L29:
	MOV	EAX,DWORD [300+ESI]
	MOV	EDX,DWORD [4+EDI+EAX*8]
	MOV	EAX,DWORD [4+ESI]
	MOV	DWORD [4+EDX],EAX
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_task_switchsub
_task_switchsub:
	PUSH	EBP
	XOR	ECX,ECX
	MOV	EBP,ESP
	MOV	EAX,DWORD [_taskctl]
	XOR	EDX,EDX
L36:
	CMP	DWORD [12+EAX+EDX*1],0
	JG	L32
	INC	ECX
	ADD	EDX,408
	CMP	ECX,9
	JLE	L36
L32:
	MOV	DWORD [EAX],ECX
	MOV	BYTE [8+EAX],0
	POP	EBP
	RET
	GLOBAL	_task_idle
_task_idle:
	PUSH	EBP
	MOV	EBP,ESP
L39:
	CALL	_io_hlt
	JMP	L39
	GLOBAL	_task_init
_task_init:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	XOR	EDI,EDI
	PUSH	EBX
	XOR	ESI,ESI
	PUSH	DWORD [8+EBP]
	MOV	EBX,999
	CALL	_init_pid
	PUSH	328092
	PUSH	DWORD [8+EBP]
	CALL	_memman_alloc_4k
	ADD	ESP,12
	MOV	DWORD [_taskctl],EAX
L47:
	MOV	EDX,EDI
	LEA	EAX,DWORD [24+ESI]
	ADD	EDX,DWORD [_taskctl]
	MOV	DWORD [4092+EDX],EAX
	LEA	EAX,DWORD [8024+ESI]
	MOV	DWORD [4232+EDX],EAX
	LEA	EAX,DWORD [2555928+ESI]
	MOV	DWORD [4096+EDX],0
	PUSH	137
	ADD	EDX,4136
	PUSH	EDX
	PUSH	103
	PUSH	EAX
	CALL	_set_segmdesc
	MOV	EAX,DWORD [_taskctl]
	ADD	EAX,EDI
	PUSH	130
	ADD	EAX,4348
	ADD	EDI,324
	PUSH	EAX
	LEA	EAX,DWORD [2563928+ESI]
	PUSH	15
	ADD	ESI,8
	PUSH	EAX
	CALL	_set_segmdesc
	ADD	ESP,32
	DEC	EBX
	JNS	L47
	MOV	ECX,DWORD [_taskctl]
	XOR	EDX,EDX
	MOV	EBX,9
L52:
	LEA	EAX,DWORD [ECX+EDX*1]
	ADD	EDX,408
	DEC	EBX
	MOV	DWORD [12+EAX],0
	MOV	DWORD [16+EAX],0
	JNS	L52
	CALL	_task_alloc
	MOV	ESI,EAX
	MOV	DWORD [4+EAX],2
	MOV	DWORD [12+EAX],2
	MOV	DWORD [8+EAX],0
	PUSH	EAX
	CALL	_task_add
	CALL	_task_switchsub
	PUSH	DWORD [ESI]
	CALL	_load_tr
	CALL	_timer_alloc
	PUSH	DWORD [12+ESI]
	PUSH	EAX
	MOV	DWORD [_task_timer],EAX
	CALL	_timer_settime
	CALL	_task_alloc
	PUSH	65536
	PUSH	DWORD [8+EBP]
	MOV	EBX,EAX
	CALL	_memman_alloc_4k
	ADD	EAX,65536
	MOV	DWORD [100+EBX],EAX
	MOV	DWORD [76+EBX],_task_idle
	MOV	DWORD [116+EBX],8
	MOV	DWORD [120+EBX],16
	MOV	DWORD [124+EBX],8
	MOV	DWORD [128+EBX],8
	MOV	DWORD [132+EBX],8
	MOV	DWORD [136+EBX],8
	PUSH	1
	PUSH	9
	PUSH	EBX
	CALL	_task_run
	MOV	EAX,DWORD [_taskctl]
	MOV	DWORD [4+EAX],0
	LEA	ESP,DWORD [-12+EBP]
	MOV	EAX,ESI
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
	GLOBAL	_task_alloc
_task_alloc:
	PUSH	EBP
	XOR	EDX,EDX
	MOV	EBP,ESP
	PUSH	EBX
L68:
	MOV	EAX,EDX
	IMUL	EAX,EAX,324
	ADD	EAX,DWORD [_taskctl]
	CMP	DWORD [4096+EAX],0
	JE	L73
	INC	EDX
	CMP	EDX,999
	JLE	L68
	XOR	EAX,EAX
L57:
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
L73:
	LEA	EBX,DWORD [4092+EAX]
	MOV	EDX,3
	MOV	DWORD [4+EBX],1
	MOV	DWORD [80+EBX],514
	MOV	DWORD [84+EBX],0
	MOV	DWORD [88+EBX],0
	MOV	DWORD [92+EBX],0
	MOV	DWORD [96+EBX],0
	MOV	DWORD [104+EBX],0
	MOV	DWORD [108+EBX],0
	MOV	DWORD [112+EBX],0
	MOV	DWORD [116+EBX],0
	MOV	DWORD [128+EBX],0
	MOV	DWORD [132+EBX],0
	MOV	DWORD [136+EBX],0
	MOV	DWORD [144+EBX],1073741824
	MOV	DWORD [52+EBX],0
	MOV	DWORD [148+EBX],895
	MOV	DWORD [152+EBX],0
	MOV	DWORD [156+EBX],65535
L67:
	MOV	DWORD [148+EBX+EDX*4],0
	INC	EDX
	CMP	EDX,26
	JLE	L67
	PUSH	EBX
	CALL	_pid_alloc
	MOV	DWORD [300+EBX],EAX
	PUSH	16
	PUSH	3932160
	CALL	_memman_alloc_4k
	MOV	DWORD [312+EBX],EAX
	MOV	EAX,EBX
	JMP	L57
	GLOBAL	_task_run
_task_run:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [12+EBP]
	MOV	EAX,DWORD [16+EBP]
	MOV	EBX,DWORD [8+EBP]
	TEST	ESI,ESI
	JS	L79
L75:
	TEST	EAX,EAX
	JLE	L76
	MOV	DWORD [12+EBX],EAX
L76:
	CMP	DWORD [4+EBX],2
	JE	L80
L77:
	CMP	DWORD [4+EBX],2
	JE	L78
	MOV	DWORD [8+EBX],ESI
	PUSH	EBX
	CALL	_task_add
	POP	EAX
L78:
	MOV	EAX,DWORD [_taskctl]
	MOV	BYTE [8+EAX],1
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L80:
	CMP	DWORD [8+EBX],ESI
	JE	L77
	PUSH	EBX
	CALL	_task_remove
	POP	EDX
	JMP	L77
L79:
	MOV	ESI,DWORD [8+EBX]
	JMP	L75
	GLOBAL	_task_sleep
_task_sleep:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [8+EBP]
	CMP	DWORD [4+ESI],2
	JE	L84
L81:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L84:
	CALL	_task_now
	PUSH	ESI
	MOV	EBX,EAX
	CALL	_task_remove
	POP	EAX
	CMP	ESI,EBX
	JNE	L81
	CALL	_task_switchsub
	CALL	_task_now
	PUSH	DWORD [EAX]
	PUSH	0
	CALL	_farjmp
	POP	ECX
	POP	EBX
	JMP	L81
	GLOBAL	_task_switch
_task_switch:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,DWORD [_taskctl]
	MOV	EDX,DWORD [EBX]
	IMUL	EDX,EDX,408
	LEA	EDX,DWORD [EDX+EBX*1]
	LEA	ECX,DWORD [12+EDX]
	MOV	EAX,DWORD [4+ECX]
	MOV	ESI,DWORD [8+ECX+EAX*4]
	INC	EAX
	MOV	DWORD [4+ECX],EAX
	CMP	EAX,DWORD [12+EDX]
	JE	L89
L86:
	CMP	BYTE [8+EBX],0
	JNE	L90
L87:
	MOV	EAX,DWORD [4+ECX]
	MOV	EBX,DWORD [8+ECX+EAX*4]
	PUSH	DWORD [12+EBX]
	PUSH	DWORD [_task_timer]
	CALL	_timer_settime
	CMP	EBX,ESI
	POP	ECX
	POP	EAX
	JE	L85
	PUSH	DWORD [EBX]
	PUSH	0
	CALL	_farjmp
	POP	EAX
	POP	EDX
L85:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L90:
	CALL	_task_switchsub
	MOV	EDX,DWORD [_taskctl]
	MOV	EAX,DWORD [EDX]
	IMUL	EAX,EAX,408
	LEA	ECX,DWORD [12+EAX+EDX*1]
	JMP	L87
L89:
	MOV	DWORD [4+ECX],0
	JMP	L86
	GLOBAL	_inthandler07
_inthandler07:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	CALL	_task_now
	MOV	EBX,EAX
	CALL	_io_cli
	CALL	_clts
	MOV	EAX,DWORD [_taskctl]
	MOV	EAX,DWORD [4+EAX]
	CMP	EAX,EBX
	JE	L92
	TEST	EAX,EAX
	JNE	L94
L93:
	LEA	EAX,DWORD [148+EBX]
	PUSH	EAX
	CALL	_frstor
	MOV	EAX,DWORD [_taskctl]
	MOV	DWORD [4+EAX],EBX
	POP	EAX
L92:
	CALL	_io_sti
	MOV	EBX,DWORD [-4+EBP]
	XOR	EAX,EAX
	LEAVE
	RET
L94:
	ADD	EAX,148
	PUSH	EAX
	CALL	_fnsave
	POP	EAX
	JMP	L93
	GLOBAL	_task_block
_task_block:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	CALL	_task_now
	PUSH	EBX
	MOV	ESI,EAX
	CALL	_task_remove
	CMP	EBX,ESI
	MOV	EAX,DWORD [300+EBX]
	MOV	DWORD [4+EBX],3
	MOV	EAX,DWORD [_pid+4+EAX*8]
	MOV	DWORD [4+EAX],3
	POP	ECX
	JE	L97
L95:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L97:
	CALL	_task_switchsub
	CALL	_task_now
	PUSH	DWORD [EAX]
	PUSH	0
	CALL	_farjmp
	POP	EAX
	POP	EDX
	JMP	L95
	GLOBAL	_task_unblock
_task_unblock:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	MOV	ESI,DWORD [3850]
	PUSH	DWORD [12+EBX]
	PUSH	DWORD [8+EBX]
	PUSH	EBX
	CALL	_task_run
	MOV	EAX,DWORD [300+EBX]
	MOV	EDX,DWORD [4+ESI+EAX*8]
	MOV	EAX,DWORD [4+EBX]
	MOV	DWORD [4+EDX],EAX
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
	GLOBAL	_message_receive
_message_receive:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	SUB	ESP,32
	MOV	EDI,DWORD [8+EBP]
	MOV	ESI,DWORD [12+EBP]
	CALL	_task_now
	MOV	EBX,EAX
	CALL	_io_cli
	MOV	DWORD [304+EBX],1
L100:
	CALL	_io_cli
	MOV	EDX,DWORD [312+EBX]
	TEST	EDX,EDX
	JNE	L108
L101:
	CALL	_io_sti
	PUSH	EBX
	CALL	_task_block
	POP	EAX
	JMP	L100
L108:
	CMP	EDI,10000
	JE	L109
	CMP	DWORD [12+EDX],EDI
	JNE	L101
	MOV	DWORD [304+EBX],0
L107:
	CALL	_io_sti
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	XOR	EAX,EAX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L109:
	MOV	DWORD [304+EBX],0
	MOV	EAX,DWORD [EDX]
	MOV	DWORD [ESI],EAX
	MOV	EAX,DWORD [4+EDX]
	MOV	DWORD [4+ESI],EAX
	MOV	EAX,DWORD [8+EDX]
	MOV	DWORD [8+ESI],EAX
	MOV	EAX,DWORD [12+EDX]
	MOV	DWORD [12+ESI],EAX
	MOV	DWORD [312+EBX],0
	JMP	L107
	GLOBAL	_message_send
_message_send:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	SUB	ESP,32
	PUSH	DWORD [8+EBP]
	MOV	ESI,DWORD [12+EBP]
	CALL	_pid2task
	MOV	EBX,EAX
	POP	EAX
	OR	EAX,-1
	TEST	EBX,EBX
	JE	L110
	CALL	_io_cli
	CALL	_task_now
	MOV	EDX,DWORD [312+EBX]
	MOV	EAX,DWORD [300+EAX]
	MOV	DWORD [12+ESI],EAX
	MOV	EAX,DWORD [ESI]
	MOV	DWORD [EDX],EAX
	MOV	EAX,DWORD [4+ESI]
	MOV	DWORD [4+EDX],EAX
	MOV	EAX,DWORD [8+ESI]
	MOV	DWORD [8+EDX],EAX
	MOV	EAX,DWORD [12+ESI]
	MOV	DWORD [12+EDX],EAX
	CMP	DWORD [304+EBX],0
	JNE	L113
L112:
	CALL	_io_sti
	XOR	EAX,EAX
L110:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L113:
	PUSH	EBX
	CALL	_task_unblock
	POP	EAX
	JMP	L112
	GLOBAL	_taskctl
[SECTION .data]
	ALIGNB	4
_taskctl:
	RESB	4
	GLOBAL	_task_timer
[SECTION .data]
	ALIGNB	4
_task_timer:
	RESB	4
	GLOBAL	_pid
[SECTION .data]
	ALIGNB	16
_pid:
	RESB	79996
