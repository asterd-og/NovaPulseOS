[extern IntHandler]
[global g_pIsrTable]

%macro Pusha 0
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

%macro Popa 0
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

%macro IsrStub 1
IntStub%+%1:
    %if !(%1 == 8 || (%1 >= 10 && %1 <= 14) || %1 == 17 || %1 == 21 || %1 == 29 || %1 == 30)
        push 0
    %endif
    push %1
    Pusha

    mov rdi, rsp
    call IntHandler

    Popa

    add rsp, 16
    iretq
%endmacro

%macro IrqStub 1
IntStub%+%1:
    push 0
    push %1
    Pusha

    mov rdi, rsp
    call IntHandler

    Popa

    add rsp, 16
    iretq
%endmacro

%assign x 0
%rep 32
    IsrStub x
    %assign x x+1
%endrep

%assign y 32
%rep 16
    IrqStub y
    %assign y y+1
%endrep

%assign z 48
%rep 224
    IsrStub z
    %assign z z+1
%endrep

g_pIsrTable:
    %assign i 0
    %rep 256
        dq IntStub%+i
        %assign i i+1
    %endrep