[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_cli
	EXTERN	_io_sti
	EXTERN	_io_hlt
	EXTERN	_memman_alloc_4k
	EXTERN	_set_segmdesc
	EXTERN	_load_tr
	EXTERN	_timer_alloc
	EXTERN	_timer_settime
	EXTERN	_ofifo32_init
	EXTERN	_farjmp
	EXTERN	_ofifo32_put
	EXTERN	_clts
	EXTERN	_frstor
	EXTERN	_fnsave
[FILE "mtask.c"]
	GLOBAL	_taskruns
[SECTION .data]
	ALIGNB	4
_taskruns:
	DD	0
	GLOBAL	_taskfree
	ALIGNB	4
_taskfree:
	DD	0
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
	POP	EBP
	RET
	GLOBAL	_pid_alloc
_pid_alloc:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	CALL	_io_cli
	MOV	EAX,DWORD [_pid+79992]
	INC	EAX
	MOV	DWORD [_pid+79992],EAX
	MOV	DWORD [304+EBX],EAX
	MOV	DWORD [_pid+4+EAX*8],EBX
	CALL	_io_sti
	POP	EBX
	MOV	EAX,DWORD [_pid+79992]
	POP	EBP
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
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	MOV	EAX,DWORD [8+EBX]
	IMUL	EAX,EAX,408
	ADD	EAX,DWORD [_taskctl]
	LEA	EDX,DWORD [12+EAX]
	CMP	ECX,DWORD [12+EAX]
	JGE	L15
L19:
	CMP	DWORD [8+EDX+ECX*4],EBX
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
	MOV	DWORD [4+EBX],1
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
	POP	EBX
	POP	EBP
	RET
	GLOBAL	_task_delete
_task_delete:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	MOV	EDI,DWORD [8+EBP]
	PUSH	EDI
	CALL	_task_remove
	XOR	EAX,EAX
	CLD
	MOV	ECX,83
	REP
	STOSD
	MOV	EDI,DWORD [-4+EBP]
	LEAVE
	RET
	GLOBAL	_task_switchsub
_task_switchsub:
	PUSH	EBP
	XOR	ECX,ECX
	MOV	EBP,ESP
	MOV	EAX,DWORD [_taskctl]
	XOR	EDX,EDX
L37:
	CMP	DWORD [12+EAX+EDX*1],0
	JG	L33
	INC	ECX
	ADD	EDX,408
	CMP	ECX,9
	JLE	L37
L33:
	MOV	DWORD [EAX],ECX
	MOV	BYTE [8+EAX],0
	POP	EBP
	RET
[SECTION .data]
LC0:
	DB	"cpu idle",0x00
[SECTION .text]
	GLOBAL	_task_idle
_task_idle:
	PUSH	EBP
	MOV	EBP,ESP
L40:
	CALL	_task_now
	MOV	DWORD [272+EAX],LC0
	CALL	_io_hlt
	JMP	L40
[SECTION .data]
LC1:
	DB	"haribote kernel",0x00
[SECTION .text]
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
	PUSH	336092
	PUSH	DWORD [8+EBP]
	CALL	_memman_alloc_4k
	ADD	ESP,12
	MOV	DWORD [_taskctl],EAX
L48:
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
	ADD	EDI,332
	PUSH	EAX
	LEA	EAX,DWORD [2563928+ESI]
	PUSH	15
	ADD	ESI,8
	PUSH	EAX
	CALL	_set_segmdesc
	ADD	ESP,32
	DEC	EBX
	JNS	L48
	MOV	ECX,DWORD [_taskctl]
	XOR	EDX,EDX
	MOV	EBX,9
L53:
	LEA	EAX,DWORD [ECX+EDX*1]
	ADD	EDX,408
	DEC	EBX
	MOV	DWORD [12+EAX],0
	MOV	DWORD [16+EAX],0
	JNS	L53
	CALL	_task_alloc
	MOV	ESI,EAX
	MOV	DWORD [272+EAX],LC1
	MOV	DWORD [328+EAX],0
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
	MOV	DWORD [328+EBX],0
	PUSH	1
	PUSH	9
	PUSH	EBX
	CALL	_task_run
	ADD	ESP,36
	MOV	EAX,DWORD [_taskctl]
	MOV	DWORD [4+EAX],0
	PUSH	1000
	PUSH	DWORD [8+EBP]
	CALL	_memman_alloc_4k
	PUSH	0
	MOV	DWORD [_free_ofifo],EAX
	PUSH	EAX
	PUSH	1000
	PUSH	_free_ofifo
	CALL	_ofifo32_init
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	MOV	EAX,ESI
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
L69:
	MOV	EAX,EDX
	IMUL	EAX,EAX,332
	ADD	EAX,DWORD [_taskctl]
	CMP	DWORD [4096+EAX],0
	JE	L74
	INC	EDX
	CMP	EDX,999
	JLE	L69
	XOR	EAX,EAX
L58:
	MOV	EBX,DWORD [-4+EBP]
	LEAVE
	RET
L74:
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
L68:
	MOV	DWORD [148+EBX+EDX*4],0
	INC	EDX
	CMP	EDX,26
	JLE	L68
	PUSH	EBX
	CALL	_pid_alloc
	MOV	DWORD [304+EBX],EAX
	PUSH	20
	PUSH	3932160
	CALL	_memman_alloc_4k
	MOV	DWORD [328+EBX],0
	MOV	DWORD [316+EBX],EAX
	MOV	EAX,EBX
	JMP	L58
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
	JS	L80
L76:
	TEST	EAX,EAX
	JLE	L77
	MOV	DWORD [12+EBX],EAX
L77:
	CMP	DWORD [4+EBX],2
	JE	L81
L78:
	CMP	DWORD [4+EBX],2
	JE	L79
	MOV	DWORD [8+EBX],ESI
	PUSH	EBX
	CALL	_task_add
	POP	EAX
L79:
	MOV	EAX,DWORD [_taskctl]
	MOV	BYTE [8+EAX],1
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L81:
	CMP	DWORD [8+EBX],ESI
	JE	L78
	PUSH	EBX
	CALL	_task_remove
	POP	EDX
	JMP	L78
L80:
	MOV	ESI,DWORD [8+EBX]
	JMP	L76
	GLOBAL	_task_sleep
_task_sleep:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [8+EBP]
	CMP	DWORD [4+ESI],2
	JE	L86
L82:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L86:
	CALL	_task_now
	PUSH	ESI
	MOV	EBX,EAX
	CALL	_task_remove
	POP	EAX
	CMP	ESI,EBX
	JNE	L82
	CALL	_task_switchsub
	CALL	_task_now
	MOV	EBX,EAX
	CMP	DWORD [328+EAX],0
	JE	L85
	MOV	EAX,ESP
	MOV	DWORD [48+EBX],EAX
L85:
	PUSH	DWORD [EBX]
	PUSH	0
	CALL	_farjmp
	POP	ECX
	POP	EBX
	JMP	L82
	GLOBAL	_task_switch
_task_switch:
	PUSH	EBP
	MOV	EAX,DWORD [_taskctl]
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EDX,DWORD [EAX]
	IMUL	EDX,EDX,408
	LEA	EDX,DWORD [12+EDX+EAX*1]
	MOV	EAX,DWORD [4+EDX]
	MOV	ESI,DWORD [8+EDX+EAX*4]
L88:
	MOV	EAX,DWORD [4+EDX]
	INC	EAX
	MOV	DWORD [4+EDX],EAX
	CMP	EAX,DWORD [EDX]
	JL	L89
	MOV	DWORD [4+EDX],0
L89:
	MOV	EAX,DWORD [_taskctl]
	CMP	BYTE [8+EAX],0
	JNE	L96
L90:
	MOV	EAX,DWORD [4+EDX]
	MOV	EBX,DWORD [8+EDX+EAX*4]
	CMP	DWORD [4+EBX],0
	JE	L88
	CMP	DWORD [_taskruns],100
	JE	L97
L92:
	CMP	DWORD [304+EBX],10001
	JE	L98
L93:
	PUSH	DWORD [12+EBX]
	PUSH	DWORD [_task_timer]
	INC	DWORD [_taskruns]
	CALL	_timer_settime
	POP	ECX
	CMP	EBX,ESI
	POP	EAX
	JE	L87
	MOV	EAX,DWORD [ESI]
	CMP	DWORD [EBX],EAX
	JE	L87
	CMP	DWORD [328+EBX],0
	JE	L95
	MOV	EAX,ESP
	MOV	DWORD [48+EBX],EAX
L95:
	PUSH	DWORD [EBX]
	PUSH	0
	CALL	_farjmp
	POP	EAX
	POP	EDX
L87:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L98:
	INC	DWORD [_taskfree]
	JMP	L93
L97:
	PUSH	DWORD [_taskfree]
	PUSH	_free_ofifo
	MOV	DWORD [_taskruns],0
	CALL	_ofifo32_put
	POP	EAX
	MOV	DWORD [_taskfree],0
	POP	EDX
	JMP	L92
L96:
	CALL	_task_switchsub
	MOV	EDX,DWORD [_taskctl]
	MOV	EAX,DWORD [EDX]
	IMUL	EAX,EAX,408
	LEA	EDX,DWORD [12+EAX+EDX*1]
	JMP	L90
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
	JE	L100
	TEST	EAX,EAX
	JNE	L102
L101:
	LEA	EAX,DWORD [148+EBX]
	PUSH	EAX
	CALL	_frstor
	MOV	EAX,DWORD [_taskctl]
	MOV	DWORD [4+EAX],EBX
	POP	ECX
L100:
	CALL	_io_sti
	MOV	EBX,DWORD [-4+EBP]
	XOR	EAX,EAX
	LEAVE
	RET
L102:
	ADD	EAX,148
	PUSH	EAX
	CALL	_fnsave
	POP	EAX
	JMP	L101
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
	MOV	DWORD [4+EBX],3
	POP	ECX
	JE	L106
L103:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L106:
	CALL	_task_switchsub
	CALL	_task_now
	MOV	ESI,EAX
	CMP	DWORD [328+EAX],0
	JE	L105
	MOV	EAX,ESP
	MOV	DWORD [48+ESI],EAX
L105:
	PUSH	DWORD [ESI]
	PUSH	0
	CALL	_farjmp
	POP	EAX
	POP	EDX
	JMP	L103
	GLOBAL	_task_unblock
_task_unblock:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	PUSH	DWORD [12+EAX]
	PUSH	DWORD [8+EAX]
	PUSH	EAX
	CALL	_task_run
	LEAVE
	RET
	GLOBAL	_message_receive
_message_receive:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	EDI,DWORD [8+EBP]
	CALL	_task_now
	MOV	ESI,EAX
L109:
	CALL	_io_cli
	MOV	DWORD [308+ESI],1
	CMP	DWORD [312+ESI],1
	JE	L119
L117:
	CALL	_io_sti
	PUSH	ESI
	CALL	_task_block
	POP	EAX
	JMP	L109
L119:
	CMP	EDI,10000
	JE	L120
	MOV	EAX,DWORD [316+ESI]
	CMP	EDI,DWORD [12+EAX]
	JE	L121
	MOV	DWORD [312+ESI],0
	JMP	L117
L121:
	CLD
	MOV	DWORD [308+ESI],0
	MOV	DWORD [312+ESI],0
	MOV	ECX,5
	MOV	EDI,EAX
	MOV	ESI,DWORD [12+EBP]
L118:
	REP
	MOVSD
	CALL	_io_sti
	LEA	ESP,DWORD [-8+EBP]
	POP	ESI
	XOR	EAX,EAX
	POP	EDI
	POP	EBP
	RET
L120:
	CLD
	MOV	DWORD [308+ESI],0
	MOV	DWORD [312+ESI],0
	MOV	ECX,5
	MOV	EDI,DWORD [12+EBP]
	MOV	ESI,DWORD [316+ESI]
	JMP	L118
	GLOBAL	_message_send
_message_send:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_pid2task
	MOV	EBX,EAX
	POP	EAX
	OR	EAX,-1
	TEST	EBX,EBX
	JE	L122
	CALL	_io_cli
	CALL	_task_now
	MOV	ECX,5
	CLD
	MOV	EAX,DWORD [304+EAX]
	MOV	EDI,DWORD [316+EBX]
	MOV	DWORD [12+ESI],EAX
	REP
	MOVSD
	MOV	DWORD [312+EBX],1
	CALL	_io_sti
	CMP	DWORD [308+EBX],0
	JNE	L125
L124:
	XOR	EAX,EAX
L122:
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L125:
	PUSH	EBX
	CALL	_task_unblock
	POP	EAX
	JMP	L124
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
	GLOBAL	_free_ofifo
[SECTION .data]
	ALIGNB	16
_free_ofifo:
	RESB	24
