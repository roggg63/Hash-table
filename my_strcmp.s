default rel

section .text
global my_strcmp

my_strcmp:
    push rbx

    xor rax, rax
    xor rbx, rbx

    .continue:

        inc rbx
        mov r8, [rdi]
        mov r9, [rsi]

        cmp r8, r9
        jne .not_equal

        add rdi, 8
        add rsi, 8

        cmp rbx, 4
        jl .continue

        mov rax, 0
        jmp .end

    .not_equal:

    mov rax, 1

    .end:

    pop rbx
    ret
