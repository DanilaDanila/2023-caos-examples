.intel_syntax noprefix

.global main

.text

main:
    mov rax, 0x1
    mov rdi, 1
    mov rsi, offset hw_str
    mov rdx, 12
    syscall

    xor rax, rax
    ret

.data

hw_str: .ascii "Hello, ASM!\n"

