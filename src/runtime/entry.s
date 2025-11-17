.global _start
.extern main

_start:
    call main
    
    mov %rax, %rdi
    mov $60, %rax

    syscall