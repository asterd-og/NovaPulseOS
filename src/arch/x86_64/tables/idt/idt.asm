[extern IntHandler]
[global g_pIntTable]

%macro Pusha_ 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
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
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rbp
    pop rdi
    pop rsi
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
%endmacro

%macro IsrNoErrStub 1
IntStub%+%1:
    push 0
    push %1
    Pusha_

    mov rdi, rsp
    call IntHandler

    Popa_

    add rsp, 16
    iretq
%endmacro

%macro IsrErrStub 1
IntStub%+%1:
    push %1
    Pusha_

    mov rdi, rsp
    call IntHandler

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
    call IntHandler

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

IrqStub 32
IrqStub 33
IrqStub 34
IrqStub 35
IrqStub 36
IrqStub 37
IrqStub 38
IrqStub 39
IrqStub 40
IrqStub 41
IrqStub 42
IrqStub 43
IrqStub 44
IrqStub 45
IrqStub 46
IrqStub 47

g_pIntTable:
    %assign i 0
    %rep 48
        dq IntStub%+i
        %assign i i+1
    %endrep