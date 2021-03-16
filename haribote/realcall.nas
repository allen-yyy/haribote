	
[BITS 32]
[INSTRSET "i486p"]
[FORMAT "WCOFF"]
[FILE "realcall.nas"]

		GLOBAL __START
	
[SECTION .text]
__START:
    JMP __32CODE_BEGIN
   
    ALIGN 16
__P_IDTR:
    DW 00     ;LIMITATION OF IDTR
    DD 00     ;Base of IDTR
__CR3 DD 0X00 ;Used to save CR3
 
    
__R_IDTR:
    DW 1024
    DD 0x00

__32CODE_BEGIN:
    PUSH EBX
    PUSH ECX
    PUSH EDX
    PUSH ESI
    PUSH EDI
    PUSH EBP
    CLI  ;Disable interrupt
    SIDT [__P_IDTR] ;SAVE IDTR
    PUSH EAX
    ;MOV EAX,CR3
    ;MOV DWORD [__CR3],EAX ;SAVE CR3.
    ;MOV EAX,CR0
    ;and eax,0x7FFFFFFF ;Clear PG bit
    ;mov cr0,eax  ;Disable paging.
    ;xor eax,eax
    ;mov cr3,eax  ;Flush TLB.
    POP EAX
    JMP __16CODE_BEGIN ;Jump to 16 bits code.

	
__16CODE_BEGIN:
    MOV AX,8190*8
    MOV DS,AX
    MOV SS,AX
    MOV ES,AX
    MOV FS,AX
    MOV GS,AX
    LIDT [__R_IDTR] ;Load interrupt vector table,the lidt use physical address.
    MOV EAX,CR0
    AND AL,0XFE ;Clear PE bit
    MOV CR0,EAX
    jmp __REAL_MODE_ENTRY  ;Jump to real mode.
 
    ALIGN 4
__REINIT_8259:           ;Re-initialize the 8259 chip to comply real mode,
                         ;since it has been configured into different mode
                         ;for protected mode.
    PUSH AX
    MOV AL,0X13
    OUT 0X20,AL
    MOV AL,0X08
    OUT 0X21,AL
    MOV AL,0X09
    OUT 0X21,AL
    POP AX
    RET
 
__REINIT_8259_EX:
    PUSH AX
    MOV AL,0X11
    OUT 0X20,AL
    OUT 0XA0,AL
    MOV AL,0X08
    OUT 0X21,AL
    MOV AL,0X70
    OUT 0XA1,AL
    MOV AL,0X04
    OUT 0X21,AL
    MOV AL,0X02
    OUT 0XA1,AL
    MOV AL,0X01
    OUT 0X21,AL
    OUT 0XA1,AL
    MOV AL,0XB8
    OUT 0X21,AL
    MOV AL,0X8F
    OUT 0XA1,AL
 
    MOV AL,0X20                  ;;Indicate the interrupt chip we have fin-
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
    OUT 0X20,AL
    OUT 0XA0,AL
 
    POP AX
    RET
 
__REAL_MODE_ENTRY:
    JMP __REALCODE_BEGIN
	
    ALIGN 4
__ESP dd 0x00  ;Save ESP
__REALCODE_BEGIN:
    MOV AX,CS
    MOV DS,AX
    MOV ES,AX
    MOV FS,AX
    MOV GS,AX
	;Save ESP.
    MOV DWORD [__ESP],ESP
	;ss and esp must be updated together.
	MOV SS,BX
    MOV SP,0XFF0
    CALL __REINIT_8259_EX         ;Set 8259 to BIOS mode
    STI ;Enable interrupt.
    ;OK,can run BIOS code now.
__BIOS_BEGIN:
	MOV AX,0x5301
    XOR BX,BX
    INT 0x15
    MOV AX,0x530E
    MOV CX,0x102
    INT 0x15
    MOV AX,5307
    MOV BX,0x01
    MOV CX,0x02
    INT 0x15
    MOV EAX,0x01   ;INDICATE THE CALLING IS SUCCESS.
    RET
    ;mov eax,0x01   ;Indicate the calling is success.
  