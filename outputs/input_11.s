.data
print_fmt_int: .string "%ld\n"
.text
.global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $0, -8(%rbp)
    movq $1, %rax
    movq %rax, -16(%rbp)
    movq $0, %rax
    movq %rax, -8(%rbp)
    movq -16(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq $0, %rax
    jmp .end_main
.end_main:
    leave
    ret
.section .note.GNU-stack,"",@progbits
