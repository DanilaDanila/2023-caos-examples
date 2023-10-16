.intel_syntax noprefix

.global _start

.text

_start:
    mov rax, 0x1
    mov rdi, 1
    mov rsi, offset hw_str
    mov rdx, 12
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall
    

.data
hw_str: .ascii "Hello, ASM!\n"

