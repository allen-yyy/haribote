[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	__alloca
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
	EXTERN	_boxfill8
	EXTERN	_sprintf
	EXTERN	_putfonts8_asc
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
	MOV	EAX,8016
	MOV	EBP,ESP
	CALL	__alloca
	XOR	EAX,EAX
	MOV	DWORD [_pid+79992],-1
	MOV	DWORD [-16+EBP],0
L7:
	MOV	DWORD [_pid+EAX*8],EAX
	INC	EAX
	CMP	EAX,9999
	JLE	L7
	XOR	EAX,EAX
L12:
	MOV	DWORD [-8016+EBP+EAX*8],EAX
	INC	EAX
	CMP	EAX,1000
	JLE	L12
	LEA	EAX,DWORD [-8016+EBP]
	MOV	DWORD [3855],EAX
	LEAVE
	RET
	GLOBAL	_pid_alloc
_pid_alloc:
	PUSH	EBP
	MOV	EAX,DWORD [_pid+79992]
	MOV	EBP,ESP
	MOV	EDX,DWORD [8+EBP]
	POP	EBP
	MOV	DWORD [_pid+4+EAX*8],EDX
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
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [8+EBP]
	MOV	EAX,DWORD [8+ESI]
	IMUL	EAX,EAX,408
	ADD	EAX,DWORD [_taskctl]
	LEA	EDX,DWORD [12+EAX]
	CMP	ECX,DWORD [12+EAX]
	JGE	L22
L26:
	CMP	DWORD [8+EDX+ECX*4],ESI
	JE	L22
	INC	ECX
	CMP	ECX,DWORD [EDX]
	JL	L26
L22:
	MOV	EAX,DWORD [4+EDX]
	DEC	DWORD [EDX]
	CMP	ECX,EAX
	JGE	L27
	DEC	EAX
	MOV	DWORD [4+EDX],EAX
L27:
	MOV	EAX,DWORD [EDX]
	CMP	DWORD [4+EDX],EAX
	JL	L28
	MOV	DWORD [4+EDX],0
L28:
	MOV	DWORD [4+ESI],1
	MOV	EBX,DWORD [EDX]
	CMP	ECX,EBX
	JGE	L36
L33:
	MOV	EAX,DWORD [12+EDX+ECX*4]
	MOV	DWORD [8+EDX+ECX*4],EAX
	INC	ECX
	CMP	ECX,EBX
	JL	L33
L36:
	MOV	EAX,DWORD [300+ESI]
	MOV	EDX,DWORD [_pid+4+EAX*8]
	MOV	EAX,DWORD [4+ESI]
	MOV	DWORD [4+EDX],EAX
	POP	EBX
	POP	ESI
	POP	EBP
	RET
	GLOBAL	_task_switchsub
_task_switchsub:
	PUSH	EBP
	XOR	ECX,ECX
	MOV	EBP,ESP
	MOV	EAX,DWORD [_taskctl]
	XOR	EDX,EDX
L43:
	CMP	DWORD [12+EAX+EDX*1],0
	JG	L39
	INC	ECX
	ADD	EDX,408
	CMP	ECX,9
	JLE	L43
L39:
	MOV	DWORD [EAX],ECX
	MOV	BYTE [8+EAX],0
	POP	EBP
	RET
	GLOBAL	_task_idle
_task_idle:
	PUSH	EBP
	MOV	EBP,ESP
L46:
	CALL	_io_hlt
	JMP	L46
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
	PUSH	324092
	PUSH	DWORD [8+EBP]
	CALL	_memman_alloc_4k
	ADD	ESP,12
	MOV	DWORD [_taskctl],EAX
L54:
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
	ADD	EDI,320
	PUSH	EAX
	LEA	EAX,DWORD [2563928+ESI]
	PUSH	15
	ADD	ESI,8
	PUSH	EAX
	CALL	_set_segmdesc
	ADD	ESP,32
	DEC	EBX
	JNS	L54
	MOV	ECX,DWORD [_taskctl]
	XOR	EDX,EDX
	MOV	EBX,9
L59:
	LEA	EAX,DWORD [ECX+EDX*1]
	ADD	EDX,408
	DEC	EBX
	MOV	DWORD [12+EAX],0
	MOV	DWORD [16+EAX],0
	JNS	L59
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
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [3824]
L75:
	MOV	EAX,EDX
	IMUL	EAX,EAX,320
	ADD	EAX,DWORD [_taskctl]
	CMP	DWORD [4096+EAX],0
	JE	L80
	INC	EDX
	CMP	EDX,999
	JLE	L75
	XOR	EAX,EAX
L64:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L80:
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
L74:
	MOV	DWORD [148+EBX+EDX*4],0
	INC	EDX
	CMP	EDX,26
	JLE	L74
	PUSH	EBX
	CALL	_pid_alloc
	MOV	DWORD [308+EBX],0
	MOV	DWORD [300+EBX],EAX
	PUSH	16
	PUSH	ESI
	CALL	_memman_alloc_4k
	MOV	DWORD [312+EBX],EAX
	MOV	EAX,EBX
	JMP	L64
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
	JS	L86
L82:
	TEST	EAX,EAX
	JLE	L83
	MOV	DWORD [12+EBX],EAX
L83:
	CMP	DWORD [4+EBX],2
	JE	L87
L84:
	CMP	DWORD [4+EBX],2
	JE	L85
	MOV	DWORD [8+EBX],ESI
	PUSH	EBX
	CALL	_task_add
	POP	EAX
L85:
	MOV	EAX,DWORD [_taskctl]
	MOV	BYTE [8+EAX],1
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L87:
	CMP	DWORD [8+EBX],ESI
	JE	L84
	PUSH	EBX
	CALL	_task_remove
	POP	EDX
	JMP	L84
L86:
	MOV	ESI,DWORD [8+EBX]
	JMP	L82
	GLOBAL	_task_sleep
_task_sleep:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [8+EBP]
	CMP	DWORD [4+ESI],2
	JE	L91
L88:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L91:
	CALL	_task_now
	PUSH	ESI
	MOV	EBX,EAX
	CALL	_task_remove
	POP	EAX
	CMP	ESI,EBX
	JNE	L88
	CALL	_task_switchsub
	CALL	_task_now
	PUSH	DWORD [EAX]
	PUSH	0
	CALL	_farjmp
	POP	ECX
	POP	EBX
	JMP	L88
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
	JE	L96
L93:
	CMP	BYTE [8+EBX],0
	JNE	L97
L94:
	MOV	EAX,DWORD [4+ECX]
	MOV	EBX,DWORD [8+ECX+EAX*4]
	PUSH	DWORD [12+EBX]
	PUSH	DWORD [_task_timer]
	CALL	_timer_settime
	CMP	EBX,ESI
	POP	ECX
	POP	EAX
	JE	L92
	PUSH	DWORD [EBX]
	PUSH	0
	CALL	_farjmp
	POP	EAX
	POP	EDX
L92:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L97:
	CALL	_task_switchsub
	MOV	EDX,DWORD [_taskctl]
	MOV	EAX,DWORD [EDX]
	IMUL	EAX,EAX,408
	LEA	ECX,DWORD [12+EAX+EDX*1]
	JMP	L94
L96:
	MOV	DWORD [4+ECX],0
	JMP	L93
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
	JE	L99
	TEST	EAX,EAX
	JNE	L101
L100:
	LEA	EAX,DWORD [148+EBX]
	PUSH	EAX
	CALL	_frstor
	MOV	EAX,DWORD [_taskctl]
	MOV	DWORD [4+EAX],EBX
	POP	EAX
L99:
	CALL	_io_sti
	MOV	EBX,DWORD [-4+EBP]
	XOR	EAX,EAX
	LEAVE
	RET
L101:
	ADD	EAX,148
	PUSH	EAX
	CALL	_fnsave
	POP	EAX
	JMP	L100
	GLOBAL	_task_block
_task_block:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	EDI,DWORD [8+EBP]
	PUSH	EBX
	MOV	EBX,DWORD [3855]
	CMP	DWORD [4+EDI],2
	JE	L105
L102:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L105:
	CALL	_task_now
	PUSH	EDI
	MOV	ESI,EAX
	CALL	_task_remove
	MOV	EAX,DWORD [300+EDI]
	MOV	DWORD [4+EDI],3
	MOV	EDX,DWORD [_pid+4+EAX*8]
	MOV	DWORD [4+EDX],3
	MOV	EDX,DWORD [8000+EBX]
	MOV	DWORD [EBX+EDX*8],EAX
	MOV	DWORD [4+EBX+EDX*8],EDI
	INC	DWORD [8000+EBX]
	CMP	EDI,ESI
	POP	ECX
	JNE	L102
	CALL	_task_switchsub
	CALL	_task_now
	PUSH	DWORD [EAX]
	PUSH	0
	CALL	_farjmp
	POP	EAX
	POP	EDX
	JMP	L102
	GLOBAL	_task_unblock
_task_unblock:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	MOV	ESI,DWORD [3855]
	PUSH	DWORD [12+EBX]
	PUSH	DWORD [8+EBX]
	PUSH	EBX
	CALL	_task_run
	ADD	ESP,12
	MOV	EAX,DWORD [300+EBX]
	XOR	ECX,ECX
	MOV	EDX,DWORD [_pid+4+EAX*8]
	MOV	EAX,DWORD [4+EBX]
	MOV	DWORD [4+EDX],EAX
	MOV	EAX,DWORD [8000+ESI]
	CMP	ECX,EAX
	JGE	L108
L112:
	CMP	DWORD [4+ESI+ECX*8],EBX
	JE	L121
	INC	ECX
	CMP	ECX,EAX
	JL	L112
L108:
	MOV	EBX,DWORD [8000+ESI]
	CMP	ECX,EBX
	JGE	L120
L117:
	MOV	EAX,DWORD [8+ESI+ECX*8]
	MOV	EDX,DWORD [12+ESI+ECX*8]
	MOV	DWORD [ESI+ECX*8],EAX
	MOV	DWORD [4+ESI+ECX*8],EDX
	INC	ECX
	CMP	ECX,EBX
	JL	L117
L120:
	LEA	EAX,DWORD [-1+EBX]
	MOV	DWORD [8000+ESI],EAX
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L121:
	MOV	DWORD [4+ESI+ECX*8],0
	MOV	DWORD [ESI+ECX*8],0
	JMP	L108
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
	MOV	DWORD [304+EAX],1
L123:
	CMP	DWORD [308+EBX],0
	JNE	L131
L130:
	PUSH	EBX
	CALL	_task_block
	POP	EAX
	JMP	L123
L131:
	CMP	EDI,10000
	JE	L132
	MOV	EAX,DWORD [312+EBX]
	CMP	DWORD [12+EAX],EDI
	JNE	L130
	MOV	DWORD [304+EBX],0
L122:
	LEA	ESP,DWORD [-12+EBP]
	XOR	EAX,EAX
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L132:
	MOV	EDX,DWORD [312+EBX]
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
	JMP	L122
[SECTION .data]
LC0:
	DB	"taskrun %d",0x00
[SECTION .text]
	GLOBAL	_message_send
_message_send:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	LEA	EBX,DWORD [-44+EBP]
	SUB	ESP,32
	PUSH	DWORD [8+EBP]
	MOV	EDI,DWORD [12+EBP]
	CALL	_pid2task
	PUSH	15
	MOV	ESI,EAX
	PUSH	255
	PUSH	0
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_boxfill8
	ADD	ESP,32
	PUSH	ESI
	PUSH	LC0
	PUSH	EBX
	CALL	_sprintf
	PUSH	EBX
	PUSH	7
	PUSH	0
	PUSH	0
	MOVSX	EAX,WORD [4084]
	PUSH	EAX
	PUSH	DWORD [4088]
	CALL	_putfonts8_asc
	OR	EAX,-1
	ADD	ESP,36
	TEST	ESI,ESI
	JE	L133
	CALL	_io_cli
	CALL	_task_now
	MOV	EDX,DWORD [312+ESI]
	MOV	EAX,DWORD [300+EAX]
	MOV	DWORD [12+EDI],EAX
	MOV	EAX,DWORD [EDI]
	MOV	DWORD [EDX],EAX
	MOV	EAX,DWORD [4+EDI]
	MOV	DWORD [4+EDX],EAX
	MOV	EAX,DWORD [8+EDI]
	MOV	DWORD [8+EDX],EAX
	MOV	EAX,DWORD [12+EDI]
	MOV	DWORD [12+EDX],EAX
	CMP	DWORD [304+ESI],1
	JE	L136
L135:
	MOV	DWORD [308+ESI],1
	CALL	_io_sti
	XOR	EAX,EAX
L133:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L136:
	PUSH	ESI
	CALL	_task_unblock
	POP	EAX
	JMP	L135
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