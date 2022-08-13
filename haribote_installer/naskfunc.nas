; naskfunc
; TAB=4

;[FORMAT "WCOFF"]				; オブジェクトファイルを作るモード	
;[INSTRSET "i486p"]				; 486の命令まで使いたいという記述
[BITS 32]						; 32ビットモード用の機械語を作らせる
;[FILE "naskfunc.nas"]			; ソースファイル名情報

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL	_asm_inthandler20, _asm_inthandler21
		GLOBAL	_asm_inthandler2c, _asm_inthandler0c
		GLOBAL	_asm_inthandler0d, _asm_end_app
		GLOBAL  _asm_inthandler2e
		GLOBAL	_memtest_sub
		GLOBAL	_farjmp, _farcall
		GLOBAL	_asm_hrb_api, _start_app,_start_sys, _asm_hrb_dpi
		EXTERN	_inthandler20, _inthandler21
		EXTERN	_inthandler2c, _inthandler0d
		EXTERN	_inthandler0c, _inthandler2e
		EXTERN	_hrb_api, _hrb_dpi
		GLOBAL  _clts, _fnsave, _frstor, _asm_inthandler07
		GLOBAL	_WriteByteToPort, _ReadByteStringFromPor
		GLOBAL	_WriteByteStringToPort, _ReadWordFromPort
		GLOBAL	_WriteWordToPort, _ReadWordStringFromPort
		GLOBAL	_WriteWordStringToPort, _inws, _outws
		GLOBAL	memcpy
		GLOBAL	memset
        EXTERN  _inthandler07,_inthandlers 

[SECTION .text]

%MACRO INTN 1
		GLOBAL _asm_inthandler%1
_asm_inthandler%1:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		PUSH	%1
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandlers
		ADD		ESP,4
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD
%ENDMACRO

INTN 0x20
INTN 0x21
INTN 0x22
INTN 0x23
INTN 0x24
INTN 0x25
INTN 0x26
INTN 0x27
INTN 0x28
INTN 0x29
INTN 0x2a
INTN 0x2b
INTN 0x2c
INTN 0x2d
INTN 0x2f

_clts:          ; void clts(void);
        CLTS
        RET

_fnsave:        ; void fnsave(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FNSAVE  [EAX]
        RET

_frstor:        ; void frstor(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FRSTOR  [EAX]
        RET

_asm_inthandler07:
        STI
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV     EAX,ESP
        PUSH    EAX
        MOV     AX,SS
        MOV     DS,AX
        MOV     ES,AX
        CALL    _inthandler07
        CMP     EAX,0
        JNE     _asm_end_app
        POP     EAX
        POPAD
        POP     DS
        POP     ES
        IRETD                   ; INT07では ESP += 4; はいらない



_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS という意味
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS という意味
		RET

_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_load_tr:		; void load_tr(int tr);
		LTR		[ESP+4]			; tr
		RET

_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD
		
_asm_inthandler2e:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2e
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD
		
_asm_inthandler0c:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0c
		CMP		EAX,0
		JNE		_asm_end_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4			; INT 0x0c でも、これが必要
		IRETD

_asm_inthandler0d:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0d
		CMP		EAX,0			; ここだけ違う
		JNE		_asm_end_app	; ここだけ違う
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4			; INT 0x0d では、これが必要
		IRETD


_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; （EBX, ESI, EDI も使いたいので）
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_farjmp:		; void farjmp(int eip, int cs);
		JMP		FAR	[ESP+4]		; eip, cs
		RET

_farcall:		; void farcall(int eip, int cs);
		CALL	FAR	[ESP+4]				; eip, cs
		RET

_asm_hrb_api:
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD		; 保存のためのPUSH
		PUSHAD		; hrb_apiにわたすためのPUSH
		MOV		AX,SS
		MOV		DS,AX		; OS用のセグメントをDSとESにも入れる
		MOV		ES,AX
		CALL	_hrb_api
		CMP		EAX,0		; EAXが0でなければアプリ終了処理
		JNE		_asm_end_app
		ADD		ESP,32
		POPAD
		POP		ES
		POP		DS
		IRETD
_asm_end_app:
;	EAXはtss.esp0の番地
		MOV		ESP,[EAX]
		MOV		DWORD [EAX+4],0
		POPAD
		RET					; cmd_appへ帰る

_asm_hrb_dpi:
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD		; 保存のためのPUSH
		PUSHAD		; hrb_apiにわたすためのPUSH
		MOV		AX,SS
		MOV		DS,AX		; OS用のセグメントをDSとESにも入れる
		MOV		ES,AX
		CALL	_hrb_dpi
		;CMP		EAX,0		; EAXが0でなければアプリ終了処理
		;JNE		_asm_end_app
		ADD		ESP,32
		POPAD
		POP		ES
		POP		DS
		IRETD

_start_app:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD		; 32ビットレジスタを全部保存しておく
		MOV		EAX,[ESP+36]	; アプリ用のEIP
		MOV		ECX,[ESP+40]	; アプリ用のCS
		MOV		EDX,[ESP+44]	; アプリ用のESP
		MOV		EBX,[ESP+48]	; アプリ用のDS/SS
		MOV		EBP,[ESP+52]	; tss.esp0の番地
		MOV		[EBP  ],ESP		; OS用のESPを保存
		MOV		[EBP+4],SS		; OS用のSSを保存
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	以下はRETFでアプリに行かせるためのスタック調整
		OR		ECX,3			; アプリ用のセグメント番号に3をORする
		OR		EBX,3			; アプリ用のセグメント番号に3をORする
		PUSH	EBX				; アプリのSS
		PUSH	EDX				; アプリのESP
		PUSH	ECX				; アプリのCS
		PUSH	EAX				; アプリのEIP
		RETF
_start_sys:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD		; 32ビットレジスタを全部保存しておく
		MOV		EAX,[ESP+36]	; アプリ用のEIP
		MOV		ECX,[ESP+40]	; アプリ用のCS
		MOV		EDX,[ESP+44]	; アプリ用のESP
		MOV		EBX,[ESP+48]	; アプリ用のDS/SS
		MOV		EBP,[ESP+52]	; tss.esp0の番地
		MOV		[EBP  ],ESP		; OS用のESPを保存
		MOV		[EBP+4],SS		; OS用のSSを保存
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	以下はRETFでアプリに行かせるためのスタック調整
		OR		ECX,1			; アプリ用のセグメント番号に3をORする
		OR		EBX,1			; アプリ用のセグメント番号に3をORする
		PUSH	EBX				; アプリのSS
		PUSH	EDX				; アプリのESP
		PUSH	ECX				; アプリのCS
		PUSH	EAX				; アプリのEIP
		RETF
;	アプリが終了してもここには来ない

;_shutdown:
;		MOV AX,5301H
;		XOR BX,BX
;		INT 0x15 

_WriteByteToPort:
		push ebp
		mov ebp,esp
		push edx
		mov al,BYTE [ebp + 8]
		mov dx,WORD [ebp + 12]
		out dx,al
		pop edx
		leave
		retn

_ReadByteStringFromPort:
		push ebp
		mov ebp,esp
		push ecx
		push edx
		push edi
		mov edi,dword [ebp + 8]
		mov ecx,dword [ebp + 12]
		mov dx,word [ebp + 16]
		rep insb
		pop edi
		pop edx
		pop ecx
		leave
		retn



_WriteByteStringToPort:
		push ebp
		mov ebp,esp
		push ecx
		push edx
		push esi
		mov esi,dword [ebp + 8]
		mov ecx,dword [ebp + 12]
		mov dx,word [ebp + 16]
		rep outsb
		pop esi
		pop edx
		pop ecx
		leave
		retn

_ReadWordFromPort:
		push ebp
		mov ebp,esp
		push ebx
		push edx
		mov dx,word [ebp + 0x0c]
		mov ebx,dword [ebp + 0x08]
		in ax,dx
		mov word [ebx],ax
		pop edx
		pop ebx
		leave
		retn


_WriteWordToPort:
		push ebp
		mov ebp,esp
		push dx
		mov dx,word [ebp + 0x0c]
		mov ax,word [ebp + 0x08]
		out dx,ax
		pop dx
		leave
		retn

_ReadWordStringFromPort:
		push ebp
		mov ebp,esp
		push ecx
		push edx
		push edi
		mov edi,dword [ebp + 0x08]
		mov ecx,dword [ebp + 0x0c]
		shr ecx,0x01
		mov dx,  word [ebp + 0x10]
		cld
		rep insw
		pop edi
		pop edx
		pop ecx
		leave
		retn




_WriteWordStringToPort:
		push ebp
		mov ebp,esp
		push ecx
		push edx
		push esi
		mov esi,dword [ebp + 0x08]
		mov ecx,dword  [ebp + 0x0c]
		shr ecx,0x02
		mov dx,  word [ebp + 0x10]
		rep outsw
		pop esi
		pop edx
		pop ecx
		leave
		retn

_inws:
	mov	esi, [esp + 4]	; buf
	mov	edx, [esp + 4 + 4 + 4]	; port
	mov	edx, [esp + 4 + 4]   ; n
	shr	ecx, 1
	cld
	rep	insw
	ret
	
_outws:
	mov	esi, [esp + 4]	; buf
	mov	edx, [esp + 4 + 4 + 4]	; port
	mov	ecx, [esp + 4 + 4]   ; n
	shr	ecx, 1
	cld
	rep	outsw
	ret
	
; ------------------------------------------------------------------------
; void* memcpy(void* es:p_dst, void* ds:p_src, int size);
; ------------------------------------------------------------------------
_memcpy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	esi, [ebp + 12]	; Source
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	al, [ds:esi]		; ┓
	inc	esi			; ┃
					; ┣ 逐字节移动
	mov	byte [es:edi], al	; ┃
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
; memcpy 结束-------------------------------------------------------------


; ------------------------------------------------------------------------
; void memset(void* p_dst, char ch, int size);
; ------------------------------------------------------------------------
_memset:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	edx, [ebp + 12]	; Char to be putted
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	byte [edi], dl		; ┓
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
	
;[SECTION .initfunc.init]
;__initfunc_start:
;__initfunc_end: