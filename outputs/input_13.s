.data
print_fmt_int: .string "%ld\n"
.text
.global main
make_pair:
    pushq %rbp
    movq %rsp, %rbp
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    subq $48, %rsp
    movq $0, -24(%rbp)
    movq $0, -32(%rbp)
    movq -8(%rbp), %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rax
    movq -24(%rbp), %rax
    movq -32(%rbp), %rdx
    jmp .end_make_pair
.end_make_pair:
    leave
    ret
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $10, %rax
    pushq %rax
    movq $20, %rax
    pushq %rax
    popq %rsi
    popq %rdi
    movl $0, %eax
    call make_pair
    movq %rax, -8(%rbp)
    movq %rdx, -16(%rbp)
    movq -8(%rbp), %rax
    movq %rax, %rsi
    leaq print_fmt_int(%rip), %rdi
    movl $0, %eax
    call printf@PLT
    movq -16(%rbp), %rax
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
