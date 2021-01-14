	GLOBAL __START
    align 4
    bits 32 ;32 bits code since in protected mode present.
__START:
    jmp __32CODE_BEGIN
    ;Following is data area used to save registers temporary.
    align 4
__P_IDTR:
    dw 00     ;Limitation of IDTR
    dd 00     ;Base of IDTR
__CR3 dd 0x00 ;Used to save CR3
__32CODE_BEGIN:
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    cli  ;Disable interrupt
    sidt [__P_IDTR + 4096] ;Save IDTR
    push eax
    mov eax,cr3
    mov dword [__CR3 + 4096],eax ;Save CR3.
    mov eax,cr0
    and eax,0x7FFFFFFF ;Clear PG bit
    mov cr0,eax  ;Disable paging.
    xor eax,eax
    mov cr3,eax  ;Flush TLB.
    pop eax
    jmp 0x38 : 4096 + __16BIT_ENTRY ;Jump to 16 bits code.
 
    align 4
    bits 16  ;16 bits code.
__16BIT_ENTRY:
    jmp __16CODE_BEGIN
    align 4
__R_IDTR:
    dw 1024
    dd 0x00
__16CODE_BEGIN:
    mov bx,0x30
    mov ds,bx
    mov ss,bx
    mov es,bx
    mov fs,bx
    mov gs,bx
    lidt [__R_IDTR + 4096] ;Load interrupt vector table,the lidt use physical address.
    mov ebx,cr0
    and bl,0xFE ;Clear PE bit
    mov cr0,ebx
    jmp 0x100 : __REAL_MODE_ENTRY  ;Jump to real mode.
 
    align 4
    bits 16
__REINIT_8259:           ;Re-initialize the 8259 chip to comply real mode,
                         ;since it has been configured into different mode
                         ;for protected mode.
    push ax
    mov al,0x13
    out 0x20,al
    mov al,0x08
    out 0x21,al
    mov al,0x09
    out 0x21,al
    pop ax
    ret
 
__REINIT_8259_EX:
    push ax
    mov al,0x11
    out 0x20,al
    out 0xa0,al
    mov al,0x08
    out 0x21,al
    mov al,0x70
    out 0xa1,al
    mov al,0x04
    out 0x21,al
    mov al,0x02
    out 0xa1,al
    mov al,0x01
    out 0x21,al
    out 0xa1,al
    mov al,0xb8
    out 0x21,al
    mov al,0x8f
    out 0xa1,al
 
    mov al,0x20                  ;;Indicate the interrupt chip we have fin-
                                 ;;ished handle the interrupt.
                                 ;;:-)
                                 ;;It is mandatory when switch back real
                                 ;;mode from protect mode.In the switching
                                 ;;process,interrupt may occur but is disabled.
                                 ;;When re-initialize the 8259 chip and enable
                                 ;;the interrupt,pending interrupt may cause
                                 ;;system crash,since the interrupt vector is
                                 ;;different between protect mode and real
                                 ;;mode.
                                 ;;So this code segment cancels all pending
                                 ;;interrupt(s) in real mode.
    out 0x20,al
    out 0xa0,al
 
    pop ax
    ret
 
__REAL_MODE_ENTRY:
    jmp __REALCODE_BEGIN
    align 4
__ESP dd 0x00  ;Save ESP
__REALCODE_BEGIN:
    mov bx,cs
    mov ds,bx
    mov es,bx
    mov fs,bx
    mov gs,bx
	;Save ESP.
    mov dword [__ESP],esp
	;ss and esp must be updated together.
	mov ss,bx
    mov sp,0xff0
    call __REINIT_8259_EX         ;Set 8259 to BIOS mode
    sti ;Enable interrupt.
    ;OK,can run BIOS code now.
__BIOS_BEGIN:
    ;mov bx,ax
	mov ax,0x5301
    xor bx,bx
    int 0x15
    mov ax,0x530e
    mov cx,0x102
    int 0x15
    mov ax,5307
    mov bl,0x01
    mov cx,0x03
    int 0x15
    mov eax,0x01   ;Indicate the calling is success.
    ret