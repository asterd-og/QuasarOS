[extern ISR_Handler]
[extern IRQ_Handler]
[global IDT_IntTable]

%macro Pusha_ 0
    push rax
	push rcx
	push rdx
	push rbx
	push rbp
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro Popa_ 0
    pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rbp
	pop rbx
	pop rdx
	pop rcx
	pop rax
%endmacro

%macro IsrNoErrStub 1
IntStub%+%1:
    push 0
    push %1
    Pusha_

    mov rdi, rsp
    call ISR_Handler

    Popa_

    add rsp, 16
    iretq
%endmacro

%macro IsrErrStub 1
IntStub%+%1:
    push %1
    Pusha_

    mov rdi, rsp
    call ISR_Handler

    Popa_

    add rsp, 16
    iretq
%endmacro

%macro IrqStub 1
IntStub%+%1:
    push 0
    push %1
    Pusha_

    mov rdi, rsp
    call IRQ_Handler

    Popa_

    add rsp, 16
    iretq
%endmacro

IsrNoErrStub 0
IsrNoErrStub 1
IsrNoErrStub 2
IsrNoErrStub 3
IsrNoErrStub 4
IsrNoErrStub 5
IsrNoErrStub 6
IsrNoErrStub 7
IsrErrStub    8
IsrNoErrStub 9
IsrErrStub    10
IsrErrStub    11
IsrErrStub    12
IsrErrStub    13
IsrErrStub    14
IsrNoErrStub 15
IsrNoErrStub 16
IsrErrStub    17
IsrNoErrStub 18
IsrNoErrStub 19
IsrNoErrStub 20
IsrNoErrStub 21
IsrNoErrStub 22
IsrNoErrStub 23
IsrNoErrStub 24
IsrNoErrStub 25
IsrNoErrStub 26
IsrNoErrStub 27
IsrNoErrStub 28
IsrNoErrStub 29
IsrErrStub    30
IsrNoErrStub 31

IrqStub 32 ; 0
IrqStub 33 ; 1
IrqStub 34 ; 2
IrqStub 35 ; 3
IrqStub 36 ; 4
IrqStub 37 ; 5
IrqStub 38 ; 6
IrqStub 39 ; 7
IrqStub 40 ; 8
IrqStub 41 ; 9
IrqStub 42 ; 10
IrqStub 43 ; 11
IrqStub 44 ; 12
IrqStub 45 ; 13
IrqStub 46 ; 14
IrqStub 47 ; 15
IrqStub 48 ; syscall

section .data

IDT_IntTable:
    %assign i 0
    %rep 49
        dq IntStub%+i
        %assign i i+1
    %endrep