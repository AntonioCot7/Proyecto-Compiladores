.data
print_fmt_int: .string "%ld\n"
.text
.global main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $5, %rax
    movq %rax, -8(%rbp)
    movq $10, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    setg %al
    movzbq %al, %rax
    cmpq $0, %rax
    je ternary_0_false
    movq -8(%rbp), %rax
    jmp ternary_0_end
ternary_0_false:
    movq -16(%rbp), %rax
ternary_0_end:
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rax
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
